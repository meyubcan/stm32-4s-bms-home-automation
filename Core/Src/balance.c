/**
 ******************************************************************************
 * @file    balance.c
 * @author  M. Eyub Can, TECH-OHM (https://tech-ohm.com)
 * @brief   Passive cell balancing logic (50% duty).
 ******************************************************************************
 */

#include "balance.h"
#include "adc_read.h"   /* cell_v_raw[] */

/* Balancing pins (port B), in the same order as cell_v_raw[]:
 *   cell 0 -> BALANCE_1 (PB9)
 *   cell 1 -> BALANCE_2 (PB8)
 *   cell 2 -> BALANCE_3 (PB7)
 *   cell 3 -> BALANCE_4 (PB6)
 */
static const uint16_t bal_pin[4] = {
    GPIO_PIN_9,   /* cell 0 */
    GPIO_PIN_8,   /* cell 1 */
    GPIO_PIN_7,   /* cell 2 */
    GPIO_PIN_6    /* cell 3 */
};

volatile uint8_t balance_active[4] = {0};

static void bal_write(uint16_t pin, uint8_t on)
{
#if BAL_ACTIVE_LOW
    HAL_GPIO_WritePin(GPIOB, pin, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
#else
    HAL_GPIO_WritePin(GPIOB, pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif
}

void Balance_Update(void)
{
    /* 1) average cell voltage (precise, not rounded) */
    float avg = (cell_v_raw[0] + cell_v_raw[1] +
                 cell_v_raw[2] + cell_v_raw[3]) / 4.0f;

    /* 2) 50% duty phase: ON during the first half of the period */
    uint32_t now = HAL_GetTick();
    uint8_t duty_on = ((now % BALANCE_PERIOD_MS) <
                       (BALANCE_PERIOD_MS * BALANCE_DUTY_PCT / 100));

    /* 3) balance any cell that is above the average by more than the threshold */
    for (int i = 0; i < 4; i++) {
        uint8_t qualifies = (cell_v_raw[i] > (avg + BALANCE_THRESHOLD));
        balance_active[i] = qualifies;
        bal_write(bal_pin[i], qualifies && duty_on);
    }
}
