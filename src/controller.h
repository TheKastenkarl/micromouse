#ifndef CONTROLLER_H
#define	CONTROLLER_H

/* PROJECT INCLUDE FILES */



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

extern void controlLoop(float posX_cmd, float posY_cmd, float posX_est, float posY_est, float deltaT, float vel_R_est, float vel_L_est);
extern void turnLeft(float deltaT, float vel_R_est, float vel_L_est);
extern void turnRight(float deltaT, float vel_R_est, float vel_L_est);
extern void turnAround(float deltaT, float vel_R_est, float vel_L_est);
extern float speedControl(float velX_cmd, float velX_est, float deltaT);
extern float posControl(float posX_cmd, float posX_est, float deltaT, float velX_est);

#endif	/* CONTROLLER_H */




