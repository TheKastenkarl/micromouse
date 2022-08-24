#include "utils.h"
#include "dma.h"

#define INITIAL_VAL 1400.

/**
 * Read left IR value and filter with exp. moving average.
 * 
 * @param updateFrac: Value between 0-1 indicates how much to trust new value.
 *  Choose 1.0 to disregard old values.
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
 *  Choose 1.0 to disregard old values.
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
 *  Choose 1.0 to disregard old values.
 * @return front IR value after applying the filter.
 */
float getFrontIR(float updateFrac) {
    static float oldVal = INITIAL_VAL;
    oldVal = expMovingAvg(oldVal, IR_FRONT, updateFrac);
    return oldVal;
}

