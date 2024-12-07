#include "timer.h"

#ifndef __INCL_RTC_SLEEP
#define __INCL_RTC_SLEEP

#define WAKE_TIME_HOURS             (24 - _SLEEP_COUNTER_HOURS)

bool isSleepActive(void);
void deepSleep(void);
void setupSleepTimer(void);
void setupWakeTimer(void);

#endif
