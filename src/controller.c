/* PROJECT INCLUDE FILES */

#include "controller.h"

/* SYTSTEM INCLUDE FILES */

#include <math.h>
#include <stdio.h>

/* DEFINES */

#define SC_TP 5.0f
#define SC_invTJ 1.0f
#define PXC_KP 0.1f
#define PYC_KP 25.0f
#define TC_KP 1.0f
#define DUTY_CYCLE_MIN_MOVEMENT 0.05f
#define DUTY_CYCLE_THRESHOLD 0.66f
#define BASELINE 0.3f
#define TURN_90 90.0f
#define TURN_180 180.0f
#define ALPHA_FILTER 0.7f


/* GLOBAL DATA DEFINITIONS */

float antiWindupFB;
float velErrorIntegral;
float thetaDiffIntegral;
float dutyCycle_Left;
float dutyCycle_Right;
differentialDrive differentialWheels;
float velX_est_Filtered;
float velAbs;


/* LOCAL DATA DEFINITIONS */

/* FUNCTION BODIES */

float speedControl(float velX_cmd, float velX_est, float deltaT)
{	
	velX_est_Filtered = ALPHA_FILTER * (velX_est - velX_est_Filtered) + velX_est_Filtered;
	float velError = velX_cmd-velX_est_Filtered;
	float velError_TP = velError * SC_TP;
	float velError_TP_J = velError_TP * SC_invTJ;
	float velError_Windup = velError_TP_J + antiWindupFB;
	velErrorIntegral = velErrorIntegral + velError_Windup * deltaT;
	float vel_PI = velErrorIntegral + velError_TP;

	float dutyCycle = vel_PI;
	if (vel_PI > 0.0f)
		dutyCycle = vel_PI + DUTY_CYCLE_MIN_MOVEMENT;
	if (vel_PI < 0.0f)
		dutyCycle = vel_PI - DUTY_CYCLE_MIN_MOVEMENT;
	
 	float dutyCycle_Sat = dutyCycle;
	if (dutyCycle > DUTY_CYCLE_THRESHOLD)
		dutyCycle_Sat = DUTY_CYCLE_THRESHOLD;
	if (dutyCycle < -DUTY_CYCLE_THRESHOLD)
		dutyCycle_Sat = -DUTY_CYCLE_THRESHOLD;
	antiWindupFB = (dutyCycle_Sat - dutyCycle) * SC_invTJ;
    
    // return velError;
    return dutyCycle_Sat;
}

static float positionXControl(float posX_cmd, float posX_est)
{
	return PXC_KP * (posX_cmd - posX_est);
}

static float thetaControl(float theta_cmd, float theta_est)
{
	return TC_KP * (theta_cmd - theta_est);
}

static float positionYControl(float posY_cmd, float posY_est)
{
	return PYC_KP * (posY_cmd - posY_est);
}

static differentialDrive differentialVelocity(float velAbs, float omega)
{
	differentialDrive wheelVelocity_L_R;
	wheelVelocity_L_R.Vel_Right = (2.0f*velAbs + omega*BASELINE)/2.0f;
	wheelVelocity_L_R.Vel_Left  = (2.0f*velAbs - omega*BASELINE)/2.0f;
	return wheelVelocity_L_R;
}

static float estimateTheta(float vel_R_est, float vel_L_est, float deltaT)
{
	float velDiff = vel_R_est - vel_L_est; 
	thetaDiffIntegral = thetaDiffIntegral + velDiff * deltaT;
	return thetaDiffIntegral/BASELINE;
}

void controlLoop(float posX_cmd, float posY_cmd, float posX_est, float posY_est, float deltaT, float vel_R_est, float vel_L_est)
{
	float velAbs    = positionXControl(posX_cmd, posX_est);
	float theta_cmd = positionYControl(posY_cmd, posY_est);
	float theta_est = estimateTheta(vel_R_est, vel_L_est, deltaT);
	float omega_cmd = thetaControl(theta_cmd, theta_est);
	differentialWheels = differentialVelocity(velAbs, omega_cmd);
	differentialWheels.dutyCycle_Right   = speedControl(differentialWheels.Vel_Right, velAbs, deltaT);
	differentialWheels.dutyCycle_Left    = speedControl(differentialWheels.Vel_Left, velAbs, deltaT);
}

void turnLeft(float deltaT, float vel_R_est, float vel_L_est)
{
	float theta_est = estimateTheta(vel_R_est, vel_L_est, deltaT);
	float omega_cmd = thetaControl(-TURN_90, theta_est);
	differentialWheels = differentialVelocity(0.0f, omega_cmd);
	differentialWheels.dutyCycle_Right   = speedControl(differentialWheels.Vel_Right, vel_R_est, deltaT);
	differentialWheels.dutyCycle_Left    = speedControl(differentialWheels.Vel_Left, vel_L_est, deltaT);
}

void turnRight(float deltaT, float vel_R_est, float vel_L_est)
{
	float theta_est = estimateTheta(vel_R_est, vel_L_est, deltaT);
	float omega_cmd = thetaControl(TURN_90, theta_est);
	differentialWheels = differentialVelocity(0.0f, omega_cmd);
	differentialWheels.dutyCycle_Right   = speedControl(differentialWheels.Vel_Right, vel_R_est, deltaT);
	differentialWheels.dutyCycle_Left    = speedControl(differentialWheels.Vel_Left, vel_L_est, deltaT);
}

void turnAround(float deltaT, float vel_R_est, float vel_L_est)
{
	float theta_est = estimateTheta(vel_R_est, vel_L_est, deltaT);
	float omega_cmd = thetaControl(TURN_180, theta_est);
	differentialWheels = differentialVelocity(0.0f, omega_cmd);
	differentialWheels.dutyCycle_Right   = speedControl(differentialWheels.Vel_Right, vel_R_est, deltaT);
	differentialWheels.dutyCycle_Left    = speedControl(differentialWheels.Vel_Left, vel_L_est, deltaT);
}

float posControl(float posX_cmd, float posX_est, float deltaT, float velX_est)
{
	velAbs    = positionXControl(posX_cmd, posX_est);
	return speedControl(velAbs, velX_est, deltaT);
}