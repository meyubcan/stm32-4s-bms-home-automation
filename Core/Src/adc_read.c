/**
 ******************************************************************************
 * @file    adc_read.c
 * @author  M. Eyub Can, TECH-OHM (https://tech-ohm.com)
 * @brief   ADC sampling with DMA, 10-sample averaging filter,
 *          real VDDA from VREFINT, cell voltage, temperature, current and SOC.
 ******************************************************************************
 */

#include "adc_read.h"
#include <math.h>   /* roundf, lroundf */

/* ---- cell divider ratios (calibrated with a multimeter) ----
 * Each divider brings the maximum voltage of its node down to about 3.0 V.
 *   node1: 1.4    node3: 4.26 (calibrated)
 *   node2: 2.8    node4: 5.67 (calibrated)
 */
const float DIV_BAT[4] = { 1.4f, 2.8f, 4.26f, 5.67f };

/* ACS712 zero point, can be recalibrated at run time */
volatile float acs_zero_v = ACS_ZERO_V_DEF;

/* ---- DMA buffer, ordered [sample][channel], ADC_OVERSAMPLE rounds ---- */
static uint16_t adc_dma_buf[ADC_CH_COUNT * ADC_OVERSAMPLE];

/* ---- values other modules read ---- */
volatile uint16_t adc_filt[ADC_CH_COUNT] = {0};
volatile float    vdda      = 3.3f;
volatile float    node_v[4] = {0};
volatile float    cell_v[4] = {0};
volatile float    cell_v_raw[4] = {0};
volatile float    pack_v    = 0.0f;
volatile int16_t  temp_c[2] = {0};   /* rounded temperature (C) */
volatile float    current_a = 0.0f;
volatile uint8_t  soc_pct   = 0;
volatile uint8_t  adc_ready = 0;
volatile float    power_w   = 0.0f;
volatile float    energy_wh = 0.0f;
volatile float    remaining_h = -1.0f;

/* ===================================================================
 * Li-ion OCV -> SOC table, per cell, with linear interpolation.
 * =================================================================== */
static uint8_t soc_from_ocv(float v_cell)
{
    static const float ocv[]  = {4.20f,4.10f,4.00f,3.90f,3.80f,3.70f,3.60f,3.50f,3.40f,3.30f,3.00f};
    static const float soc[]  = {100.0f,90.0f,80.0f,70.0f,60.0f,50.0f,40.0f,30.0f,20.0f,10.0f, 0.0f};
    const int n = sizeof(ocv)/sizeof(ocv[0]);

    if (v_cell >= ocv[0])     return 100;
    if (v_cell <= ocv[n-1])   return 0;

    for (int i = 0; i < n-1; i++) {
        if (v_cell <= ocv[i] && v_cell > ocv[i+1]) {
            float t = (v_cell - ocv[i+1]) / (ocv[i] - ocv[i+1]);
            float s = soc[i+1] + t * (soc[i] - soc[i+1]);
            return (uint8_t)(s + 0.5f);
        }
    }
    return 0;
}

/* ===================================================================
 * Start: continuous circular sampling with DMA
 * =================================================================== */
void ADC_Filter_Start(ADC_HandleTypeDef *hadc)
{
    HAL_ADC_Start_DMA(hadc, (uint32_t*)adc_dma_buf,
                      ADC_CH_COUNT * ADC_OVERSAMPLE);
}

/* ===================================================================
 * Call with ZERO current: the present sensor output becomes the zero point.
 * =================================================================== */
void ADC_CalibrateACSZero(void)
{
    if (adc_ready) {
        acs_zero_v = (vdda * (float)adc_filt[CH_CURRENT] / 4095.0f) * DIV_ACS;
    }
}

/* ===================================================================
 * Energy integration (Wh). Call it often from the main loop.
 * =================================================================== */
void Energy_Update(void)
{
    static uint32_t last_tick = 0;
    uint32_t now = HAL_GetTick();

    if (last_tick == 0) { last_tick = now; return; }  /* first call */

    float dt_h = (float)(now - last_tick) / 3600000.0f; /* ms -> hours */
    last_tick = now;

    power_w    = pack_v * current_a;       /* instantaneous power */
    energy_wh += power_w * dt_h;           /* accumulated energy */

    /* remaining time estimate: (capacity_Ah * SOC%) / present current */
    if (current_a >= RUNTIME_MIN_CURRENT) {
        float remaining_ah = BATT_CAPACITY_AH * ((float)soc_pct / 100.0f);
        remaining_h = remaining_ah / current_a;
    } else {
        remaining_h = -1.0f;               /* current too low to estimate */
    }
}

/* Clear the energy counter */
void Energy_Reset(void)
{
    energy_wh = 0.0f;
}

/* ===================================================================
 * Called when a full DMA transfer completes (after ADC_OVERSAMPLE rounds)
 * =================================================================== */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance != ADC1) return;

    /* 1) average of 10 samples, this is the digital filter */
    uint32_t acc[ADC_CH_COUNT] = {0};
    for (int s = 0; s < ADC_OVERSAMPLE; s++) {
        for (int c = 0; c < ADC_CH_COUNT; c++) {
            acc[c] += adc_dma_buf[s * ADC_CH_COUNT + c];
        }
    }
    for (int c = 0; c < ADC_CH_COUNT; c++) {
        adc_filt[c] = (uint16_t)(acc[c] / ADC_OVERSAMPLE);
    }

    /* 2) Real VDDA (VREFINT calibration) */
    if (adc_filt[CH_VREF] != 0) {
        vdda = 3.3f * (float)(*VREFINT_CAL) / (float)adc_filt[CH_VREF];
        vdda *= VDDA_CORR;
    }

    /* 3) Cell node voltages, kept as float internally */
    float node[4];
    for (int i = 0; i < 4; i++) {
        node[i]   = (vdda * (float)adc_filt[CH_BAT1 + i] / 4095.0f) * DIV_BAT[i];
        node_v[i] = node[i];   /* raw, for calibration */
    }
    float c0 = node[0];
    float c1 = node[1] - node[0];
    float c2 = node[2] - node[1];
    float c3 = node[3] - node[2];
    float pack_full = node[3];

    /* Compute SOC from the pack voltage before any rounding */
    soc_pct = soc_from_ocv(pack_full / 4.0f);

    /* RAW cell voltages for balancing */
        cell_v_raw[0] = c0;
        cell_v_raw[1] = c1;
        cell_v_raw[2] = c2;
        cell_v_raw[3] = c3;

        /* round the published voltages to one decimal (4.0779 -> 4.1) */
        cell_v[0] = roundf(c0 * 10.0f) / 10.0f;
        cell_v[1] = roundf(c1 * 10.0f) / 10.0f;
        cell_v[2] = roundf(c2 * 10.0f) / 10.0f;
        cell_v[3] = roundf(c3 * 10.0f) / 10.0f;
        pack_v    = roundf(pack_full * 10.0f) / 10.0f;

    /* 4) LM35DZ temperatures: T = Vpin * 100, rounded to an integer */
    temp_c[0] = (int16_t)lroundf((vdda * (float)adc_filt[CH_TEMP1] / 4095.0f) * 100.0f);
    temp_c[1] = (int16_t)lroundf((vdda * (float)adc_filt[CH_TEMP2] / 4095.0f) * 100.0f);

    /* 5) ACS712 current */
    float acs_v   = (vdda * (float)adc_filt[CH_CURRENT] / 4095.0f) * DIV_ACS;
    float cur_raw = (acs_zero_v - acs_v) / ACS_SENS;   /* sensor is wired in the reverse direction */
    if (cur_raw < 0.0f) cur_raw = 0.0f;

    static float cur_ema = 0.0f;
    cur_ema += CURRENT_EMA_ALPHA * (cur_raw - cur_ema);

    /* publish it rounded to two decimals */
    current_a = roundf(cur_ema * 100.0f) / 100.0f;

    adc_ready = 1;
}
