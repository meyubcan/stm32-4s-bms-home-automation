/**
 ******************************************************************************
 * @file    balance.c
 * @brief   Pasif hucre dengeleme mantigi (%50 duty).
 ******************************************************************************
 */

#include "balance.h"
#include "adc_read.h"   /* cell_v_raw[] */

/* Balans pinleri (port B). Index, cell_v_raw[] ile ayni sirada:
 *   cell 0 -> BALANS_1 (PB9)
 *   cell 1 -> BALANS_2 (PB8)
 *   cell 2 -> BALANS_3 (PB7)
 *   cell 3 -> BALANS_4 (PB6)
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
    /* 1) ortalama hucre gerilimi (hassas, yuvarlanmamis) */
    float avg = (cell_v_raw[0] + cell_v_raw[1] +
                 cell_v_raw[2] + cell_v_raw[3]) / 4.0f;

    /* 2) %50 duty fazi: periyodun ilk yarisinda ON */
    uint32_t now = HAL_GetTick();
    uint8_t duty_on = ((now % BALANCE_PERIOD_MS) <
                       (BALANCE_PERIOD_MS * BALANCE_DUTY_PCT / 100));

    /* 3) ortalamanin esikten fazla uzerindeki hucreyi %50 duty ile dengele */
    for (int i = 0; i < 4; i++) {
        uint8_t qualifies = (cell_v_raw[i] > (avg + BALANCE_THRESHOLD));
        balance_active[i] = qualifies;
        bal_write(bal_pin[i], qualifies && duty_on);
    }
}
