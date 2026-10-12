/* M. Eyub Can, TECH-OHM (https://tech-ohm.com) */
#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void BT_Init(UART_HandleTypeDef *huart);
void BT_SendTelemetry(void);

#ifdef __cplusplus
}
#endif

#endif
