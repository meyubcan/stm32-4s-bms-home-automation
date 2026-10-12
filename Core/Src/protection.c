/**
 ******************************************************************************
 * @file    protection.c
 * @brief   Batarya sicaklik koruma mantigi (fan / buzzer / led).
 ******************************************************************************
 */

#include "protection.h"
#include "adc_read.h"   /* temp_c[] buradan gelir */

/* ---- Cikis pinleri ---- */
#define FAN_PORT     GPIOD
#define FAN_PIN      GPIO_PIN_14   /* batarya fani */
#define BUZZER_PORT  GPIOD
#define BUZZER_PIN   GPIO_PIN_15   /* buzzer */
#define LED1_PORT    GPIOD
#define LED1_PIN     GPIO_PIN_5    /* USER_LED_1 */
#define LED2_PORT    GPIOD
#define LED2_PIN     GPIO_PIN_6    /* USER_LED_2 */

volatile uint8_t temp_alarm  = 0;
volatile uint8_t fan_state   = 0;
volatile uint8_t alarm_state = 0;

static void pin_write(GPIO_TypeDef *port, uint16_t pin, uint8_t on)
{
#if OUT_ACTIVE_LOW
    HAL_GPIO_WritePin(port, pin, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
#else
    HAL_GPIO_WritePin(port, pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif
}

void Thermal_Update(void)
{
    int16_t t = temp_c[BATT_TEMP_IDX];   /* tam sayi sicaklik */

    /* ---------- FAN: 35 ustu AC, 31 ve alti KAPAT (4C histerezis) ---------- */
    if (t >= FAN_ON_TEMP)       fan_state = 1;
    else if (t <= FAN_OFF_TEMP) fan_state = 0;
    pin_write(FAN_PORT, FAN_PIN, fan_state);

    /* ---------- ALARM: 55 ustu AC, 52 ve alti KAPAT (3C histerezis) ---------- */
    if (t >= ALARM_ON_TEMP)       alarm_state = 1;
    else if (t <= ALARM_OFF_TEMP) alarm_state = 0;
    temp_alarm = alarm_state;     /* app bunu okuyup uyari verecek */

    if (alarm_state) {
        /* buzzer surekli, ledler yanip soner */
        pin_write(BUZZER_PORT, BUZZER_PIN, 1);

        static uint32_t last_blink = 0;
        static uint8_t  blink = 0;
        uint32_t now = HAL_GetTick();
        if (now - last_blink >= BLINK_PERIOD_MS) {
            last_blink = now;
            blink ^= 1;
        }
        pin_write(LED1_PORT, LED1_PIN, blink);
        pin_write(LED2_PORT, LED2_PIN, blink);
    } else {
        /* alarm yok: buzzer + ledler kapali (fan kendi histereziyle devam) */
        pin_write(BUZZER_PORT, BUZZER_PIN, 0);
        pin_write(LED1_PORT, LED1_PIN, 0);
        pin_write(LED2_PORT, LED2_PIN, 0);
    }
}
