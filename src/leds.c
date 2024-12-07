#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

#include "led_def.h"
#include "leds.h"

#define NUM_LEVEL_VALUES           257

static const uint16_t levelBuffer[NUM_LEVEL_VALUES] = {
    0x0000, 0x0001, 0x0004, 0x0009, 0x0010, 0x0019, 0x0024, 0x0031, 0x0040, 0x0051, 0x0064, 0x0079, 0x0090, 0x00A9, 0x00C4, 0x00E1,
    0x0100, 0x0121, 0x0144, 0x0169, 0x0190, 0x01B9, 0x01E4, 0x0211, 0x0240, 0x0271, 0x02A4, 0x02D9, 0x0310, 0x0349, 0x0384, 0x03C1,
    0x0400, 0x0441, 0x0484, 0x04C9, 0x0510, 0x0559, 0x05A4, 0x05F1, 0x0640, 0x0691, 0x06E4, 0x0739, 0x0790, 0x07E9, 0x0844, 0x08A1,
    0x0900, 0x0961, 0x09C4, 0x0A29, 0x0A90, 0x0AF9, 0x0B64, 0x0BD1, 0x0C40, 0x0CB1, 0x0D24, 0x0D99, 0x0E10, 0x0E89, 0x0F04, 0x0F81,
    0x1000, 0x1081, 0x1104, 0x1189, 0x1210, 0x1299, 0x1324, 0x13B1, 0x1440, 0x14D1, 0x1564, 0x15F9, 0x1690, 0x1729, 0x17C4, 0x1861,
    0x1900, 0x19A1, 0x1A44, 0x1AE9, 0x1B90, 0x1C39, 0x1CE4, 0x1D91, 0x1E40, 0x1EF1, 0x1FA4, 0x2059, 0x2110, 0x21C9, 0x2284, 0x2341,
    0x2400, 0x24C1, 0x2584, 0x2649, 0x2710, 0x27D9, 0x28A4, 0x2971, 0x2A40, 0x2B11, 0x2BE4, 0x2CB9, 0x2D90, 0x2E69, 0x2F44, 0x3021,
    0x3100, 0x31E1, 0x32C4, 0x33A9, 0x3490, 0x3579, 0x3664, 0x3751, 0x3840, 0x3931, 0x3A24, 0x3B19, 0x3C10, 0x3D09, 0x3E04, 0x3F01,
    0x4000, 0x4101, 0x4204, 0x4309, 0x4410, 0x4519, 0x4624, 0x4731, 0x4840, 0x4951, 0x4A64, 0x4B79, 0x4C90, 0x4DA9, 0x4EC4, 0x4FE1,
    0x5100, 0x5221, 0x5344, 0x5469, 0x5590, 0x56B9, 0x57E4, 0x5911, 0x5A40, 0x5B71, 0x5CA4, 0x5DD9, 0x5F10, 0x6049, 0x6184, 0x62C1,
    0x6400, 0x6541, 0x6684, 0x67C9, 0x6910, 0x6A59, 0x6BA4, 0x6CF1, 0x6E40, 0x6F91, 0x70E4, 0x7239, 0x7390, 0x74E9, 0x7644, 0x77A1,
    0x7900, 0x7A61, 0x7BC4, 0x7D29, 0x7E90, 0x7FF9, 0x8164, 0x82D1, 0x8440, 0x85B1, 0x8724, 0x8899, 0x8A10, 0x8B89, 0x8D04, 0x8E81,
    0x9000, 0x9181, 0x9304, 0x9489, 0x9610, 0x9799, 0x9924, 0x9AB1, 0x9C40, 0x9DD1, 0x9F64, 0xA0F9, 0xA290, 0xA429, 0xA5C4, 0xA761,
    0xA900, 0xAAA1, 0xAC44, 0xADE9, 0xAF90, 0xB139, 0xB2E4, 0xB491, 0xB640, 0xB7F1, 0xB9A4, 0xBB59, 0xBD10, 0xBEC9, 0xC084, 0xC241,
    0xC400, 0xC5C1, 0xC784, 0xC949, 0xCB10, 0xCCD9, 0xCEA4, 0xD071, 0xD240, 0xD411, 0xD5E4, 0xD7B9, 0xD990, 0xDB69, 0xDD44, 0xDF21,
    0xE100, 0xE2E1, 0xE4C4, 0xE6A9, 0xE890, 0xEA79, 0xEC64, 0xEE51, 0xF040, 0xF231, 0xF424, 0xF619, 0xF810, 0xFA09, 0xFC04, 0xFE01,
    0xFFFF
};

static uint slices[NUM_LEDS];
static int fadeValues[NUM_LEDS] = {0, 0, 0, 0, 0};
static bool goingUpFlags[NUM_LEDS] = {true, true, true, true, true};

int currentSlice = 0;

static inline int getLEDIndex(uint slice) {
    for (int i = 0;i < NUM_LEDS;i++) {
        if (slice == slices[i]) {
            return i;
        }
    }

    return 0;
}

static inline void setupIterateEnablePin(void) {
    gpio_init(LED_ITERATE_ENABLE);
    gpio_set_function(LED_ITERATE_ENABLE, GPIO_FUNC_SIO);
    gpio_set_dir(LED_ITERATE_ENABLE, GPIO_IN);
    gpio_pull_down(LED_ITERATE_ENABLE);
}

static void setupSlices(void) {
    slices[0] = pwm_gpio_to_slice_num(STAR_LED_1);
    slices[1] = pwm_gpio_to_slice_num(STAR_LED_2);
    slices[2] = pwm_gpio_to_slice_num(STAR_LED_3);
    slices[3] = pwm_gpio_to_slice_num(STAR_LED_4);
    slices[4] = pwm_gpio_to_slice_num(STAR_LED_5);
}

static inline uint8_t getInterruptingPWMSlice() {
    uint8_t irqChannelMask = pwm_get_irq_status_mask() & 0x000000FF;

    uint8_t channel = 0;

    while (!(irqChannelMask & 0x01)) {
        irqChannelMask = irqChannelMask >> 1;
        channel++;
    }

    return channel;
}

inline int * getCurrentSliceAddress(void) {
    return &currentSlice;
}

inline int getCurrentSlice(void) {
    return currentSlice;
}

inline void setCurrentSlice(int slice) {
    currentSlice = slice;
}

inline bool isLEDIterateEnabled(void) {
    return gpio_get(LED_ITERATE_ENABLE);
}

static inline void nextSlice(void) {
    disableLEDSlice(currentSlice);

    currentSlice++;

    if (currentSlice == NUM_LEDS) {
        currentSlice = 0;
    }

    enableLEDSlice(currentSlice);
}

void _pwmWrapISR(void) {
    uint8_t irqSlice = getInterruptingPWMSlice();
    int ledIndex = getLEDIndex(irqSlice);

    // Clear the interrupt flag that brought us here
    pwm_clear_irq(irqSlice);

    if (goingUpFlags[ledIndex]) {
        fadeValues[ledIndex]++;

        if (fadeValues[ledIndex] == NUM_LEVEL_VALUES - 1) {
            goingUpFlags[ledIndex] = false;
        }
    }
    else {
        fadeValues[ledIndex]--;

        if (fadeValues[ledIndex] == 0) {
            goingUpFlags[ledIndex] = true;

            if (isLEDIterateEnabled()) {
                nextSlice();
            }
        }
    }

    pwm_set_chan_level(irqSlice, PWM_CHAN_A, levelBuffer[fadeValues[ledIndex]]);
}

static void setupIRQ(void) {
    for (int i = 0;i < NUM_LEDS;i++) {
        pwm_clear_irq(slices[i]);
        pwm_set_irq_enabled(slices[i], true);
    }

    irq_set_exclusive_handler(PWM_IRQ_WRAP, _pwmWrapISR);
    irq_set_enabled(PWM_IRQ_WRAP, true);
}

inline void enableLEDSlice(int slice) {
    pwm_set_enabled(slices[slice], true);
}

inline void disableLEDSlice(int slice) {
    pwm_set_enabled(slices[slice], false);
}

inline void disablePWMs(void) {
    irq_set_enabled(PWM_IRQ_WRAP, false);

    for (int i = 0;i < NUM_LEDS;i++) {
        pwm_clear_irq(slices[i]);
        pwm_set_irq_enabled(slices[i], false);
        pwm_set_enabled(slices[i], false);
    }
}

inline void disableLEDPins(void) {
    /*
    ** Claim GPIOs as outputs and drive them all low...
    */
    uint pinMask = 
            (1 << STAR_LED_1) | 
            (1 << STAR_LED_2) | 
            (1 << STAR_LED_3) | 
            (1 << STAR_LED_4) | 
            (1 << STAR_LED_5) | 
            (1 << PICO_DEFAULT_LED_PIN) |
            (1 << LED_ITERATE_ENABLE);

    gpio_init_mask(pinMask);
    gpio_set_dir_out_masked(pinMask);

    gpio_disable_pulls(STAR_LED_1);    
    gpio_disable_pulls(STAR_LED_2);    
    gpio_disable_pulls(STAR_LED_3);    
    gpio_disable_pulls(STAR_LED_4);    
    gpio_disable_pulls(STAR_LED_5);
    gpio_disable_pulls(LED_ITERATE_ENABLE);

    gpio_clr_mask(pinMask);
}

void setupLEDPins(void) {
    setupIterateEnablePin();

    gpio_init(STAR_LED_1);
    gpio_set_function(STAR_LED_1, GPIO_FUNC_PWM);
    gpio_set_drive_strength(STAR_LED_1, GPIO_DRIVE_STRENGTH_12MA);

    gpio_init(STAR_LED_2);
    gpio_set_function(STAR_LED_2, GPIO_FUNC_PWM);
    gpio_set_drive_strength(STAR_LED_2, GPIO_DRIVE_STRENGTH_12MA);
    
    gpio_init(STAR_LED_3);
    gpio_set_function(STAR_LED_3, GPIO_FUNC_PWM);
    gpio_set_drive_strength(STAR_LED_3, GPIO_DRIVE_STRENGTH_12MA);
    
    gpio_init(STAR_LED_4);
    gpio_set_function(STAR_LED_4, GPIO_FUNC_PWM);
    gpio_set_drive_strength(STAR_LED_4, GPIO_DRIVE_STRENGTH_12MA);
    
    gpio_init(STAR_LED_5);
    gpio_set_function(STAR_LED_5, GPIO_FUNC_PWM);
    gpio_set_drive_strength(STAR_LED_5, GPIO_DRIVE_STRENGTH_12MA);
}

void initialisePWM(void) {
    setupSlices();
    setupIRQ();

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv_int(&config, 4);

    for (int i = 0;i < NUM_LEDS;i++) {
        pwm_init(slices[i], &config, false);
    }
}
