/******************************************************************************
**                                                                           **
**                        RASPBERRY PI PICO PINOUT                           **
**                                                                           **
*******************************************************************************
**                                  _____                                    **
**             GPIO  0 (01) -|^^^^^|^^^^^|^^^^^|- (40) VBUS                  **
**             GPIO  1 (02) -|     | USB |     |- (39) VSYS                  **
**             GND     (03) -|     ^^^^^^^     |- (38) GND                   **
**             GPIO  2 (04) -|                 |- (37) 3V3 EN                **
**             GPIO  3 (05) -|                 |- (36) 3V3 (OUT)             **
**             GPIO  4 (06) -|                 |- (35) ADC VREF              **
**             GPIO  5 (07) -|                 |- (34) GPIO 28               **
**             GND     (08) -|                 |- (33) GND                   **
**             GPIO  6 (09) -|                 |- (32) GPIO 27               **
**             GPIO  7 (10) -|    |^^^^^^^|    |- (31) GPIO 26               **
**             GPIO  8 (11) -|    |   RP  |    |- (30) RUN                   **
**             GPIO  9 (12) -|    |  2040 |    |- (29) GPIO 22               **
**             GND     (13) -|    ^^^^^^^^^    |- (28) GND                   **
**             GPIO 10 (14) -|                 |- (27) GPIO 21               **
**             GPIO 11 (15) -|                 |- (26) GPIO 20               **
**             GPIO 12 (16) -|                 |- (25) GPIO 19               **
**             GPIO 13 (17) -|                 |- (24) GPIO 18               **
**             GND     (18) -|                 |- (23) GND                   **
**             GPIO 14 (19) -|                 |- (22) GPIO 17               **
**             GPIO 15 (20) -|_________________|- (21) GPIO 16               **
**                                                                           **
******************************************************************************/

#ifndef __INCL_LED_DEF
#define __INCL_LED_DEF

#define LED_ITERATE_ENABLE          20

#define NUM_LEDS                     5

#define STAR_LED_WHITE               4
#define STAR_LED_BLUE                6
#define STAR_LED_GREEN               8
#define STAR_LED_RED                16
#define STAR_LED_PINK               18

#define STAR_LED_1                  STAR_LED_WHITE
#define STAR_LED_2                  STAR_LED_BLUE
#define STAR_LED_3                  STAR_LED_GREEN
#define STAR_LED_4                  STAR_LED_RED
#define STAR_LED_5                  STAR_LED_PINK

#endif
