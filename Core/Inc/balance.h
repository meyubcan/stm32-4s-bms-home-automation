/**
 ******************************************************************************
 * @file    balance.h
 * @author  M. Eyub Can, TECH-OHM (https://tech-ohm.com)
 * @brief   Passive cell balancing.
 *          The average cell voltage is computed. Cells that sit above it by
 *          BALANCE_THRESHOLD get their balancing switch driven at 50% duty.
 *          That discharges the fuller cells toward the others.
 ******************************************************************************
 */

#ifndef BALANCE_H
#define BALANCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/* A cell this far ABOVE the average gets balanced (V).
 * NOTE: 0.5 V was chosen for testing. Real use needs about 0.03-0.05 V. */
#define BALANCE_THRESHOLD   0.5f

/* 50% duty. Period and ratio. */
#define BALANCE_PERIOD_MS   2000   /* 2 s period */
#define BALANCE_DUTY_PCT    50     /* 50% -> 1 s on, 1 s off */

/* Set to 1 if the balancing outputs are active low */
#define BAL_ACTIVE_LOW      0

/* Which cells are being balanced right now (for status and the app) */
extern volatile uint8_t balance_active[4];

/* Call OFTEN from the main loop */
void Balance_Update(void);

#ifdef __cplusplus
}
#endif

#endif /* BALANCE_H */
