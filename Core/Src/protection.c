/**
 ******************************************************************************
 * @file    protection.c
 * @author  M. Eyub Can, TECH-OHM (https://tech-ohm.com)
 * @brief   Battery thermal protection logic (fan / buzzer / LEDs).
 ******************************************************************************
 */

#include "protection.h"
#include "adc_read.h"   /* temp_c[] comes from here */

/* ---- Output pins ---- */
#define FAN_PORT     GPIOD
#define FAN_PIN      GPIO_PIN_14   /* battery fan */
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
    int16_t t = temp_c[BATT_TEMP_IDX];   /* temperature as an integer */

    /* ---------- FAN: ON at 35 and above, OFF at 31 and below (4 C hysteresis) ---------- */
    if (t >= FAN_ON_TEMP)       fan_state = 1;
    else if (t <= FAN_OFF_TEMP) fan_state = 0;
    pin_write(FAN_PORT, FAN_PIN, fan_state);

    /* ---------- ALARM: ON at 55 and above, OFF at 52 and below (3 C hysteresis) ---------- */
    if (t >= ALARM_ON_TEMP)       alarm_state = 1;
    else if (t <= ALARM_OFF_TEMP) alarm_state = 0;
    temp_alarm = alarm_state;     /* the app reads this and shows a warning */

    if (alarm_state) {
        /* buzzer on continuously, LEDs blink */
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
        /* no alarm: buzzer and LEDs off (the fan follows its own hysteresis) */
        pin_write(BUZZER_PORT, BUZZER_PIN, 0);
        pin_write(LED1_PORT, LED1_PIN, 0);
        pin_write(LED2_PORT, LED2_PIN, 0);
    }
}
