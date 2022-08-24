#ifndef IRSENSORS_H
#define	IRSENSORS_H

#include <xc.h> // include processor files - each processor file is guarded.  

float getLeftIR(float updateFrac);
float getRightIR(float updateFrac);
float getFrontIR(float updateFrac);


#endif	/* IRSENSORS_H */