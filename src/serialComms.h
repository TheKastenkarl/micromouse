#ifndef SERIALCOMMS_H
#define	SERIALCOMMS_H

#include <xc.h>

#define DELIMITER "\r\n"

void setupUART1(unsigned short baudrate);
void sendUART1(char* buffer, int waitIfBusy);

#endif	/* SERIALCOMMS_H */

