#ifndef CONTROLLER_H
#define	CONTROLLER_H

/* PROJECT INCLUDE FILES */

#include <stdbool.h>.

/* TYPEDEFS */

typedef struct{
	float Vel_Right, Vel_Left, dutyCycle_Right, dutyCycle_Left;
} differentialDrive;

/* GLOBAL DATA DEFINITIONS */

extern float antiWindupFB;
extern float velErrorIntegral;
extern float thetaDiffIntegral;
extern float velAbs;
extern differentialDrive differentialWheels;

/* FUNCTION PROTOTYPES */

extern void controlLoop(float posX_cmd, float posY_cmd, float posX_est, float posY_est, float vel_R_est, float vel_L_est);
extern void turnLeft(float vel_R_est, float vel_L_est);
extern void turnRight(float vel_R_est, float vel_L_est);
extern float speedControl(float velX_cmd, float velX_est);

#endif	/* CONTROLLER_H */




