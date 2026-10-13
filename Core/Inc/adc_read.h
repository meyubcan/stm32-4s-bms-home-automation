/**
 ******************************************************************************
 * @file    adc_read.h
 * @author  M. Eyub Can, TECH-OHM (https://tech-ohm.com)
 * @brief   ADC sampling with DMA, 10-sample digital filter (average),
 *          real VDDA from VREFINT calibration, cell voltage, temperature and current.
 *          STM32F407VG, 4S pack, 2x LM35DZ, 1x ACS712-20A
 ******************************************************************************
 */

#ifndef ADC_READ_H
#define ADC_READ_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/* ============================ SETTINGS ============================ */

/* Total channel count (7 sensors + internal VREFINT) */
#define ADC_CH_COUNT     8

/* Digital filter: how many samples are averaged */
#define ADC_OVERSAMPLE   10

/* VREFINT_CAL address (STM32F407, factory calibration at 3.3 V) */
#define VREFINT_CAL      ((uint16_t*)0x1FFF7A2AU)

/* ---- PACK CAPACITY (for the remaining-time estimate) ----
 * Pack: 4S10P, 2500 mAh cells.
 * 10 in parallel -> capacity = 10 * 2500 mAh = 25000 mAh = 25.0 Ah
 * (series cells raise the voltage, not the Ah)
 */
#define BATT_CAPACITY_AH     25.0f

/* Below this current the remaining time is treated as unknown (idle) */
#define RUNTIME_MIN_CURRENT  0.05f

/* Per-board trim factor. 1.0 = no correction */
#define VDDA_CORR        1.000f

/* ---- CELL VOLTAGE DIVIDER RATIOS ----
 * Each divider brings the full-charge voltage of its node down to 3.0 V.
 * DIV_BAT[i] = max_voltage / 3.0  (used to get the real node voltage back)
 *   node1: 4.2V  -> 3.0V  => 1.4
 *   node2: 8.4V  -> 3.0V  => 2.8
 *   node3: 12.6V -> 3.0V  => 4.2
 *   node4: 16.8V -> 3.0V  => 5.6
 */
extern const float DIV_BAT[4];

/* ---- ACS712-20A ---- */
#define ACS_SENS         0.100f   /* 20 A version: 100 mV/A */
#define ACS_ZERO_V_DEF   2.520f   /* measured zero point (no divider, direct) */
#define DIV_ACS          1.000f   /* no divider -> 1.0 (sensor goes straight to the pin) */
/* Extra smoothing factor for current (EMA). Smaller = smoother and slower.
 * 0.10 light, 0.05 medium, 0.02 heavy */
#define CURRENT_EMA_ALPHA  0.05f
/* zero point calibrated at run time (default = ACS_ZERO_V_DEF) */
extern volatile float acs_zero_v;

/* ============================ CHANNEL INDEXES ============================ */
enum {
    CH_BAT1 = 0, CH_BAT2, CH_BAT3, CH_BAT4,
    CH_TEMP1, CH_TEMP2, CH_CURRENT, CH_VREF
};

/* ============================ PUBLISHED VALUES ============================ */
extern volatile uint16_t adc_filt[ADC_CH_COUNT]; /* filtered raw value */
extern volatile float    vdda;                   /* real supply voltage (V) */
extern volatile float    node_v[4];              /* RAW node voltages (for calibration) */
extern volatile float    cell_v[4];              /* cell voltages (V, one decimal) */
extern volatile float    cell_v_raw[4];          /* RAW cell voltages (for balancing) */
extern volatile float    pack_v;                 /* pack voltage (V, one decimal) */
extern volatile int16_t  temp_c[2];              /* LM35DZ temperatures (C, integer) */
extern volatile float    current_a;              /* ACS712 current (A, never negative) */
extern volatile uint8_t  soc_pct;                /* state of charge (%) */
extern volatile uint8_t  adc_ready;              /* set after the first valid reading */
extern volatile float    power_w;                /* instantaneous power (W) = pack_v * current_a */
extern volatile float    energy_wh;              /* total consumed energy (Wh) */
extern volatile float    remaining_h;            /* remaining time estimate (hours). <0 = unknown */

/* ============================ FUNCTIONS ============================ */

/* Call once from USER CODE 2 in main.c */
void ADC_Filter_Start(ADC_HandleTypeDef *hadc);

/* Call with ZERO current: stores the present sensor output as the zero point */
void ADC_CalibrateACSZero(void);

/* Energy integration: call OFTEN from the main loop (it measures dt itself) */
void Energy_Update(void);

/* Clear the energy counter (for the "reset" command from the app) */
void Energy_Reset(void);

#ifdef __cplusplus
}
#endif

#endif /* ADC_READ_H */
