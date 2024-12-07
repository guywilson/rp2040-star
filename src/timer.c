#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"

#include "timer.h"

#define TIMER_INTERRUPT_DELAY_US        1000000U
#define ALARM_NUM                       0

const uint32_t ON_TIME_THRESHOLD = _ON_TIME_THRESHOLD;

static volatile uint32_t clockCounter = 0U;

static inline void setupDebugLED(void) {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

static inline void toggleDebugLED(void) {
    static bool isOn = false;

    if (isOn) {
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        isOn = false;
    }
    else {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        isOn = true;
    }
}

static void irqTick(void) {
    // Clear the alarm irq
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM);

	clockCounter++;

    toggleDebugLED();

    hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);
    timer_hw->alarm[ALARM_NUM] = (uint32_t)(timer_hw->timerawl + TIMER_INTERRUPT_DELAY_US);
}

void setupTimer(void) {
    setupDebugLED();

    hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);

    irq_set_exclusive_handler(TIMER_IRQ_0, irqTick);
    irq_set_enabled(TIMER_IRQ_0, true);

    timer_hw->alarm[ALARM_NUM] = (uint32_t)(timer_hw->timerawl + TIMER_INTERRUPT_DELAY_US);
}

inline void disableTimer(void) {
    hw_clear_bits(&timer_hw->inte, 1u << ALARM_NUM);
    irq_set_enabled(TIMER_IRQ_0, false);
}

inline uint32_t getClockCounter(void) {
    return clockCounter;
}

inline bool isSleepRequired(void) {
    return (clockCounter >= ON_TIME_THRESHOLD);
}
