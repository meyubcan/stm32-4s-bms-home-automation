/**
 ******************************************************************************
 * @file    balance.h
 * @author  M. Eyub Can, TECH-OHM (https://tech-ohm.com)
 * @brief   Pasif hucre dengeleme.
 *          Ortalama hucre gerilimi bulunur; ortalamanin BALANCE_THRESHOLD
 *          uzerindeki hucrelerin balans FET'i %50 duty ile acilir (yan-son).
 *          Boylece fazla dolu hucreler bosaltilip dengeye getirilir.
 ******************************************************************************
 */

#ifndef BALANCE_H
#define BALANCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/* Ortalamanin bu kadar UZERINDEKI hucre dengelenir (V).
 * NOT: 0.5 V test icin secildi. Gercek kullanimda 0.03-0.05 V civarina cekilmeli. */
#define BALANCE_THRESHOLD   0.5f

/* %50 duty (yan-son). Periyot ve oran. */
#define BALANCE_PERIOD_MS   2000   /* 2 sn periyot */
#define BALANCE_DUTY_PCT    50     /* %50 -> 1 sn acik, 1 sn kapali */

/* Balans cikislari LOW ile tetikleniyorsa 1 yap */
#define BAL_ACTIVE_LOW      0

/* Hangi hucre su an dengeleniyor (durum izleme / app) */
extern volatile uint8_t balance_active[4];

/* Ana donguden SIK SIK cagir */
void Balance_Update(void);

#ifdef __cplusplus
}
#endif

#endif /* BALANCE_H */
