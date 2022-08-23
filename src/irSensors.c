#include "irSensors.h"
#include "IOconfig.h"
#include "utils.h"
#include "dma.h"

#define INITIAL_VAL 1400.

/**
 * Read left IR value and filter with exp. moving average.
 * 
 * @param updateFrac: Value between 0-1 indicates how much to trust new value.
 * @return left IR value after applying the filter.
 */
float getLeftIR(float updateFrac) {
    static float oldVal = INITIAL_VAL;
    oldVal = expMovingAvg(oldVal, IR_LEFT, updateFrac);
    return oldVal;
}

/**
 * Read right IR value and filter with exp. moving average.
 * 
 * @param updateFrac: Value between 0-1 indicates how much to trust new value.
 * @return right IR value after applying the filter.
 */
float getRightIR(float updateFrac) {
    static float oldVal = INITIAL_VAL;
    oldVal = expMovingAvg(oldVal, IR_RIGHT, updateFrac);
    return oldVal;
}

/**
 * Read front IR value and filter with exp. moving average.
 * 
 * @param updateFrac: Value between 0-1 indicates how much to trust new value.
 * @return front IR value after applying the filter.
 */
float getFrontIR(float updateFrac) {
    static float oldVal = INITIAL_VAL;
    oldVal = expMovingAvg(oldVal, IR_FRONT, updateFrac);
    return oldVal;
}

