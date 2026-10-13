/**
 ******************************************************************************
 * @file    protection.h
 * @author  M. Eyub Can, TECH-OHM (https://tech-ohm.com)
 * @brief   Battery thermal protection:
 *            >35C  -> fan (PD14)
 *            >55C  -> buzzer (PD15), user LED 1 and 2 (PD5/PD6) blink
 *          Wide hysteresis keeps the relay from chattering. Sets an alarm flag for the app.
 ******************************************************************************
 */

#ifndef PROTECTION_H
#define PROTECTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/* Which LM35 is the battery temperature? (0 = temp_c[0], 1 = temp_c[1]) */
#define BATT_TEMP_IDX     0

/* Thresholds (C).
 * Turns on at ON and stays on until the temperature falls to OFF. The wide gap
 * keeps the relay from switching on and off at the threshold.
 *   FAN  : on at 35, off at 31  (4 C gap)
 *   ALARM: on at 55, off at 52  (3 C gap)
 */
#define FAN_ON_TEMP       35
#define FAN_OFF_TEMP      31
#define ALARM_ON_TEMP     55
#define ALARM_OFF_TEMP    52

/* LED blink period (ms) */
#define BLINK_PERIOD_MS   300

/* Set to 1 if the outputs are active low */
#define OUT_ACTIVE_LOW    0

/* ---- Published state ---- */
extern volatile uint8_t temp_alarm;   /* 1 = temperature passed 55 (app warning) */
extern volatile uint8_t fan_state;    /* fan is on */
extern volatile uint8_t alarm_state;  /* alarm is active */

/* Call OFTEN from the main loop (reads the temperature, drives the outputs) */
void Thermal_Update(void);

#ifdef __cplusplus
}
#endif

#endif /* PROTECTION_H */
