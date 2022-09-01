#ifndef TIMER_H
#define	TIMER_H

#define TIMER1_MAX_COUNT 65535

#include <stdbool.h>


int initTimer1(float periodMs);
void startTimer1(void);
void stopTimer1(void);
bool delay();
#endif	/* TIMER_H */