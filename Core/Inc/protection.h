/**
 ******************************************************************************
 * @file    protection.h
 * @brief   Batarya sicaklik korumasi:
 *            >35C  -> fan (PD14)
 *            >55C  -> buzzer (PD15) + user led 1&2 (PD5/PD6) yanip soner
 *          Genis histerezis ile (role titremesini onler), app uyari bayragi.
 ******************************************************************************
 */

#ifndef PROTECTION_H
#define PROTECTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/* Hangi LM35 batarya sicakligi? (0 = temp_c[0], 1 = temp_c[1]) */
#define BATT_TEMP_IDX     0

/* Esik degerleri (C).
 * ON'da acilir, OFF'a DUSENE KADAR kapanmaz -> genis histerezis,
 * esikte role titremesini (ac/kapa) tamamen onler.
 *   FAN  : 35'te ac, 31'e dusunce kapat  (4C bosluk)
 *   ALARM: 55'te ac, 52'ye dusunce kapat (3C bosluk)
 */
#define FAN_ON_TEMP       35
#define FAN_OFF_TEMP      31
#define ALARM_ON_TEMP     55
#define ALARM_OFF_TEMP    52

/* LED yanip-sonme periyodu (ms) */
#define BLINK_PERIOD_MS   300

/* Cikislar LOW ile tetikleniyorsa 1 yap */
#define OUT_ACTIVE_LOW    0

/* ---- Disari acilan durumlar ---- */
extern volatile uint8_t temp_alarm;   /* 1 = sicaklik 55'i gecti (app uyarisi) */
extern volatile uint8_t fan_state;    /* fan acik mi */
extern volatile uint8_t alarm_state;  /* alarm aktif mi */

/* Ana donguden SIK SIK cagir (sicakligi okur, cikislari yonetir) */
void Thermal_Update(void);

#ifdef __cplusplus
}
#endif

#endif /* PROTECTION_H */
