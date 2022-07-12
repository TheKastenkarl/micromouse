#ifndef SERIALCOMMS_H
#define	SERIALCOMMS_H

#include <xc.h>

#define DELIMITER '\n'

void setupUART1(void);
void sendUART1(char* buffer, int waitIfBusy);

#endif	/* SERIALCOMMS_H */

