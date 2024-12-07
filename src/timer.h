#include <stdint.h>

#ifndef __INCL_TIMER
#define __INCL_TIMER

#define _SLEEP_COUNTER_HOURS         10
#define _ON_TIME_THRESHOLD           (3600 * _SLEEP_COUNTER_HOURS)

void setupTimer(void);
void disableTimer(void);
uint32_t getClockCounter(void);
bool isSleepRequired(void);

#endif
