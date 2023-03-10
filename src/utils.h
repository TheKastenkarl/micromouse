#ifndef UTILS_H
#define	UTILS_H

#define INTERNAL_OSC_PERIOD_MICROSECS 0.0375
#define INTERNAL_OSC_FREQ_HZ 26.667e6

#define BAUDRATE 57600
#define CONTROL_FREQUENCY 0.01

float convertRanges(float aMin, float aMax, float bMin, float bMax, float aVal);
void getPositionSmallestPossiblePrescaler(float periodMsDesired, float timerMaxCount, float internalTimeBaseUs, unsigned int prescaleValues[], unsigned char len, int* positionIndex, int* reqCount);
float expMovingAvg(float oldVal, float newVal, float updateFrac);

#endif	/* UTILS_H */