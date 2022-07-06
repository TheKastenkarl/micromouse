#ifndef PWM_H
#define	PWM_H

#define PWMTIMER_MAX_COUNT 32767

int setupPWM(float PWMPeriodMs);
void setPWMDutyCycle(float perc, unsigned char numDutyCycleGenerator);
void periodicDutyCycleSwitching(void);
void sinusoidDutyCycleSwitching(int periodSteps);

#endif	/* PWM_H */

