#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "pico/time.h"
#include "pico/multicore.h"
#include "hardware/structs/scb.h"
#include "hardware/clocks.h"
#include "hardware/rtc.h"
#include "hardware/watchdog.h"

#include "led_def.h"
#include "leds.h"
#include "rtc_sleep.h"
#include "timer.h"

static datetime_t dt;
static datetime_t alarm_dt;

inline void deepSleep(void) {
    disableLEDPins();
    disableTimer();
    
    clocks_hw->sleep_en0 = CLOCKS_SLEEP_EN0_CLK_RTC_RTC_BITS;
    clocks_hw->sleep_en1 = 0x0;

    uint save = scb_hw->scr;
    
    // Enable deep sleep at the proc
    scb_hw->scr = save | M0PLUS_SCR_SLEEPDEEP_BITS;

    __wfi();
}

void _wakeCallback(void) {
    /*
    ** This will reset the device in 50ms...
    */
    watchdog_enable(50, false);
}

void setupWakeTimer(void) {
    /*
    ** Set the date as midnight 1st Jan 2020...
    */
    dt.year = 2020;
    dt.month = 1;
    dt.day = 1;
    dt.dotw = 0;
    dt.hour = 0;
    dt.min = 0;
    dt.sec = 0;

    /*
    ** Set an alarm for 24 - SLEEP_TIME_HOURS
    */
    alarm_dt.year = -1;
    alarm_dt.month = -1;
    alarm_dt.day = -1;
    alarm_dt.dotw = -1;
    alarm_dt.hour = WAKE_TIME_HOURS;
    alarm_dt.min = -1;
    alarm_dt.sec = -1;

    rtc_init();
    rtc_set_datetime(&dt);
    rtc_set_alarm(&alarm_dt, _wakeCallback);
}
