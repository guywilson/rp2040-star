#ifndef __INCL_LEDS
#define __INCL_LEDS

int * getCurrentSliceAddress(void);
int getCurrentSlice(void);
void setCurrentSlice(int slice);
void enableLEDSlice(int slice);
void disableLEDSlice(int slice);
void disablePWMs(void);
void disableLEDPins(void);
void setupLEDPins(void);
void initialisePWM(void);
bool isLEDIterateEnabled(void);

#endif
