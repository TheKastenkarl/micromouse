#include "xc.h"
#include "utils.h"


/**
 * Calculates the smallest possible prescaler value for timer applications.
 * 
 * Writes back results to passed adresses for "positionIndex" and "reqCount"
 * 
 * @param periodMsDesired: Desired timer period in miliseconds
 * @param timerMaxCount: Maximum count value of timer
 * @param internalTimeBaseUs: Internal cycle time in mikroseconds
 * @param prescaleValues[]: array of available prescale values, assumed to be
 *      in ascending order, e.g [1, 4, 16, 64]
 * @param len: len of "prescaleValues" array
 * @param positionIndex: calculated index in "prescaleValues" to choose
 * @param reqCount: calculated required count to achieve desired period
 */
void getPositionSmallestPossiblePrescaler(
        float periodMsDesired,
        float timerMaxCount, 
        float internalTimeBaseUs,
        unsigned int prescaleValues[], 
        unsigned char len, 
        int* positionIndex, 
        int* reqCount) {
    
    // calculate by which factor our required counting value is too high
    float timeOnePeriodMs = timerMaxCount / 1000 * internalTimeBaseUs; // swapping division by 1000 to front to not have overflow
    float fractionCountPossible = periodMsDesired / timeOnePeriodMs; // 
    
    // return error if not possible
    if (fractionCountPossible > prescaleValues[len - 1]) {
        *positionIndex = -1;
    }
    
    // otherwise choose lowest prescale value
    unsigned char i = 0;
    for (i = 0; i < len; i++){
        if (fractionCountPossible < prescaleValues[i]) {
            *positionIndex = i;
            break;
        }
    }
    
    int prescaleValue = prescaleValues[i];
    *reqCount = (int) (timerMaxCount / prescaleValue * fractionCountPossible);
}
