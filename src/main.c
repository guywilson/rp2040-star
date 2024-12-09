#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "pico/flash.h"
#include "pico/multicore.h"
#include "hardware/irq.h"
#include "hardware/flash.h"
#include "hardware/watchdog.h"

#include "led_def.h"
#include "leds.h"
#include "rtc_sleep.h"
#include "timer.h"

#define FLASH_TARGET_OFFSET         (uintptr_t)(256 * 1024)

static inline int getSliceFromFlash(void) {
    int slice;

    const uint8_t* flash_target_contents = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);
    memcpy(&slice, flash_target_contents, sizeof(int));

    return slice;
}

static inline void flashCurrentSlice(void) {
    uint32_t interrupts = save_and_disable_interrupts();

    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, (uint8_t *)getCurrentSliceAddress(), sizeof(int));

    restore_interrupts(interrupts);
}

int main(void) {
    watchdog_disable();
    setupLEDPins();
    initialisePWM();
    setupTimer();

    if (isLEDIterateEnabled()) {
        enableLEDSlice(getCurrentSlice());
    }
    else {
        int slice = getSliceFromFlash();
        enableLEDSlice(slice);

        setCurrentSlice(slice);
    }

    while (1) {
        if (isSleepRequired()) {
            if (!isLEDIterateEnabled()) {
                flashCurrentSlice();
            }

            setupWakeTimer();
            sleep_ms(400);

            deepSleep();
        }

        __wfi();
    }
}
