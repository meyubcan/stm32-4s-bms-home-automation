/* M. Eyub Can, TECH-OHM (https://tech-ohm.com) */
#include "bluetooth.h"
#include "adc_read.h"
#include "protection.h"
#include "balance.h"
#include <stdio.h>

extern TIM_HandleTypeDef htim1;

static UART_HandleTypeDef *bt_uart = 0;
static uint8_t  rx_byte;
static char     cmd_letter = 0;
static uint8_t  expect_arg = 0;

static void servo_door(uint32_t ch, uint8_t open)
{
    __HAL_TIM_SET_COMPARE(&htim1, ch, open ? 1500 : 1000);
}

static void BT_Apply(char c, uint8_t on)
{
    GPIO_PinState s = on ? GPIO_PIN_SET : GPIO_PIN_RESET;
    switch (c) {
        case 'a': HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, s); break;
        case 'b': HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, s); break;
        case 'c': HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, s); break;
        case 'd': HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8,  s); break;
        case 'e': HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9,  s); break;
        case 'f': HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,  s); break;
        case 'g': servo_door(TIM_CHANNEL_1, on); break;
        case 'h': servo_door(TIM_CHANNEL_2, on); break;
        case 'i': servo_door(TIM_CHANNEL_3, on); break;
        case 'j': servo_door(TIM_CHANNEL_4, on); break;
        default: break;
    }
}

void BT_Init(UART_HandleTypeDef *huart)
{
    bt_uart = huart;
    HAL_UART_Receive_IT(bt_uart, &rx_byte, 1);
}

void BT_SendTelemetry(void)
{
    if (!bt_uart) return;

    int pack_i = (int)(pack_v * 10.0f + 0.5f);
    int c1 = (int)(cell_v[0] * 10.0f + 0.5f);
    int c2 = (int)(cell_v[1] * 10.0f + 0.5f);
    int c3 = (int)(cell_v[2] * 10.0f + 0.5f);
    int c4 = (int)(cell_v[3] * 10.0f + 0.5f);
    int cur_i = (int)(current_a * 100.0f + 0.5f);
    int pw_i  = (int)(power_w * 10.0f + 0.5f);
    int wh_i  = (int)(energy_wh * 10.0f + 0.5f);
    int rem   = (remaining_h < 0.0f) ? -1 : (int)(remaining_h * 60.0f + 0.5f);

    char buf[110];
    int n = snprintf(buf, sizeof(buf),
        "%d,%d.%d,%d.%d,%d.%d,%d.%d,%d.%d,%d,%d,%d.%02d,%d.%d,%d.%d,%d,%d,%d,%d,%d,%d\n",
        (int)soc_pct,
        pack_i/10, pack_i%10,
        c1/10, c1%10, c2/10, c2%10, c3/10, c3%10, c4/10, c4%10,
        (int)temp_c[0], (int)temp_c[1],
        cur_i/100, cur_i%100,
        pw_i/10, pw_i%10,
        wh_i/10, wh_i%10,
        rem,
        (int)balance_active[0], (int)balance_active[1],
        (int)balance_active[2], (int)balance_active[3],
        (int)temp_alarm);

    if (n > 0) HAL_UART_Transmit(bt_uart, (uint8_t*)buf, (uint16_t)n, 100);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != USART1) return;

    uint8_t b = rx_byte;

    if (!expect_arg) {
        if (b == 'z') {
            Energy_Reset();
        } else if (b >= 'a' && b <= 'j') {
            cmd_letter = (char)b;
            expect_arg = 1;
        }
    } else {
        if (b == '1')      BT_Apply(cmd_letter, 1);
        else if (b == '0') BT_Apply(cmd_letter, 0);
        expect_arg = 0;
    }

    HAL_UART_Receive_IT(huart, &rx_byte, 1);
}
