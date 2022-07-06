#ifndef TIMER1_H
#define	TIMER1_H

#define TIMER1_MAX_COUNT 65535

int initTimer1(float periodMs);
void startTimer1(void);
void stopTimer1(void);
void virtualTimer(int actionEveryXCalls);

#endif	/* TIMER1_H */