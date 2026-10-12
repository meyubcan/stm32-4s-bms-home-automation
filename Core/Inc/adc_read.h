/**
 ******************************************************************************
 * @file    adc_read.h
 * @author  M. Eyub Can, TECH-OHM (https://tech-ohm.com)
 * @brief   ADC + DMA okuma, 10 ornek digital filtre (ortalama),
 *          VREFINT kalibrasyonu ile gercek VDDA, pil/sicaklik/akim hesabi.
 *          STM32F407VG - 4S pil + 2x LM35DZ + 1x ACS712-20A
 ******************************************************************************
 */

#ifndef ADC_READ_H
#define ADC_READ_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/* ============================ AYARLAR ============================ */

/* Toplam kanal sayisi (7 sensor + 1 dahili VREFINT) */
#define ADC_CH_COUNT     8

/* Digital filtre: kac ornegin ortalamasi alinacak */
#define ADC_OVERSAMPLE   10

/* VREFIN_CAL adresi (STM32F407, 3.3V'ta fabrika kalibrasyonu) */
#define VREFINT_CAL      ((uint16_t*)0x1FFF7A2AU)

/* ---- PIL KAPASITESI (tahmini kalan sure icin) ----
 * Pak: 4S10P, hucre 2500mAh.
 * 10 paralel -> kapasite = 10 * 2500mAh = 25000mAh = 25.0 Ah
 * (seri Ah'i degistirmez, sadece gerilimi artirir)
 */
#define BATT_CAPACITY_AH     25.0f

/* Bu akimin altinda "kalan sure belirsiz" kabul edilir (idle) */
#define RUNTIME_MIN_CURRENT  0.05f

/* Karta ozel ince ayar. 1.0 = duzeltme yok */
#define VDDA_CORR        1.000f

/* ---- PIL GERILIM BOLUCU ORANLARI ----
 * Her dugumun bolucusu, o dugumun dolu haldeki maks gerilimini 3.0V yapar.
 * DIV_BAT[i] = maks_gerilim / 3.0  (gercek dugum gerilimini geri almak icin)
 *   node1: 4.2V  -> 3.0V  => 1.4
 *   node2: 8.4V  -> 3.0V  => 2.8
 *   node3: 12.6V -> 3.0V  => 4.2
 *   node4: 16.8V -> 3.0V  => 5.6
 */
extern const float DIV_BAT[4];

/* ---- ACS712-20A ---- */
#define ACS_SENS         0.100f   /* 20A versiyon: 100 mV/A */
#define ACS_ZERO_V_DEF   2.520f   /* olculen sifir noktasi (bol. yok, dogrudan) */
#define DIV_ACS          1.000f   /* bolucu YOK -> 1.0 (sensor pine dogrudan) */
/* Akim ek yumusatma katsayisi (EMA). Kucuk = daha yumusak/yavas.
 * 0.10 hafif, 0.05 orta, 0.02 cok yumusak */
#define CURRENT_EMA_ALPHA  0.05f
/* calisma aninda kalibre edilen sifir noktasi (default = ACS_ZERO_V_DEF) */
extern volatile float acs_zero_v;

/* ============================ KANAL INDEKSLERI ============================ */
enum {
    CH_BAT1 = 0, CH_BAT2, CH_BAT3, CH_BAT4,
    CH_TEMP1, CH_TEMP2, CH_CURRENT, CH_VREF
};

/* ============================ DISARI ACILAN DEGERLER ============================ */
extern volatile uint16_t adc_filt[ADC_CH_COUNT]; /* filtrelenmis ham deger */
extern volatile float    vdda;                   /* gercek besleme gerilimi (V) */
extern volatile float    node_v[4];              /* HAM dugum gerilimleri (kalibrasyon icin) */
extern volatile float    cell_v[4];              /* hucre gerilimleri (V, 1 ondalik) */
extern volatile float    cell_v_raw[4];          /* HAM hucre gerilimleri (dengeleme icin) */
extern volatile float    pack_v;                 /* toplam pak gerilimi (V, 1 ondalik) */
extern volatile int16_t  temp_c[2];              /* LM35DZ sicakliklari (C, tam sayi) */
extern volatile float    current_a;              /* ACS712 akimi (A, negatif olmaz) */
extern volatile uint8_t  soc_pct;                /* sarj durumu (%) */
extern volatile uint8_t  adc_ready;              /* ilk gecerli okuma yapildi mi */
extern volatile float    power_w;                /* anlik guc (W) = pack_v * current_a */
extern volatile float    energy_wh;              /* toplam tuketilen enerji (Wh) */
extern volatile float    remaining_h;            /* tahmini kalan sure (saat). <0 = belirsiz */

/* ============================ FONKSIYONLAR ============================ */

/* main.c USER CODE 2 icinde bir kez cagir */
void ADC_Filter_Start(ADC_HandleTypeDef *hadc);

/* Akim SIFIRKEN cagir: o anki ACS cikisini sifir noktasi olarak kaydeder */
void ADC_CalibrateACSZero(void);

/* Enerji entegrasyonu: ana donguden SIK SIK cagir (dt'yi kendi olcer) */
void Energy_Update(void);

/* Toplam enerji sayacini sifirla (app'ten "reset" komutu icin) */
void Energy_Reset(void);

#ifdef __cplusplus
}
#endif

#endif /* ADC_READ_H */
