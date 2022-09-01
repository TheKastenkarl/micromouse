/* PROJECT INCLUDE FILES */

#include "controller.h"

/* SYTSTEM INCLUDE FILES */

#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stdlib.h>
#include "motorEncoders.h"

/* DEFINES */
#define SPEED_CONTROL_TP 2.0f
#define SPPED_CONTROL_INV_TJ 1.0f
#define POSITION_X_CONTROL_KP 0.05f
#define POSITION_Y_CONTROL_KP 10.0f
#define THETA_CONTROL_KP 0.55f
#define DUTY_CYCLE_MIN_MOVEMENT 0.05f
#define DUTY_CYCLE_THRESHOLD 0.66f
#define BASELINE 0.3f
#define TURN_90 1.57f
#define ALPHA_FILTER 0.7f
#define SAMPLE_TIME 0.05F

/* LOCAL DATA DEFINITIONS */
float antiWindupFB;
float velErrorIntegral;
float thetaDiffIntegral;
float dutyCycle_Left;
float dutyCycle_Right;
float velX_est_Filtered;
float velAbs;
float current_theta;
bool theta_init;

/* GLOBAL DATA DEFINITIONS */
differentialDrive differentialWheels;
float positionInRad_L;
float velocityInRadPerSample_L;
float positionInRad_R;
float velocityInRadPerSample_R;


/* FUNCTION BODIES */

float speedControl(float velX_cmd, float velX_est)
{	
	velX_est_Filtered = ALPHA_FILTER * (velX_est - velX_est_Filtered) + velX_est_Filtered;
	float velError = velX_cmd-velX_est_Filtered;
	float velError_TP = velError * SPEED_CONTROL_TP;
	float velError_TP_J = velError_TP * SPPED_CONTROL_INV_TJ;
	float velError_Windup = velError_TP_J + antiWindupFB;
	velErrorIntegral = velErrorIntegral + velError_Windup * SAMPLE_TIME;
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
	antiWindupFB = (dutyCycle_Sat - dutyCycle) * SPPED_CONTROL_INV_TJ;
    
    // return velError;
    return dutyCycle_Sat;
}

static float positionXControl(float posX_cmd, float posX_est)
{
	return POSITION_X_CONTROL_KP * (posX_cmd - posX_est);
}

static float thetaControl(float theta_cmd, float theta_est)
{
	return THETA_CONTROL_KP * (theta_cmd - theta_est);
}

static float positionYControl(float posY_cmd, float posY_est)
{
	return POSITION_Y_CONTROL_KP * (posY_cmd - posY_est);
}

static differentialDrive differentialVelocity(float velAbs, float omega)
{
	differentialDrive wheelVelocity_L_R;
	wheelVelocity_L_R.Vel_Right = (2.0f*velAbs + omega*BASELINE)/2.0f;
	wheelVelocity_L_R.Vel_Left  = (2.0f*velAbs - omega*BASELINE)/2.0f;
	return wheelVelocity_L_R;
}

static float estimateTheta(float vel_R_est, float vel_L_est)
{
	float velDiff = vel_R_est - vel_L_est; 
	thetaDiffIntegral = thetaDiffIntegral + velDiff * SAMPLE_TIME;
	return thetaDiffIntegral/BASELINE;
}

bool controlLoop(float posX_cmd, float posY_cmd, float posX_est, float posY_est, float vel_R_est, float vel_L_est)
{
	float velAbs    = positionXControl(posX_cmd, posX_est);
	float theta_cmd = positionYControl(posY_cmd, posY_est);
	float theta_est = estimateTheta(vel_R_est, vel_L_est);
	float omega_cmd = thetaControl(theta_cmd, theta_est);
	differentialWheels = differentialVelocity(velAbs, omega_cmd);
	differentialWheels.dutyCycle_Right   = speedControl(differentialWheels.Vel_Right, vel_R_est);
	differentialWheels.dutyCycle_Left    = speedControl(differentialWheels.Vel_Left, vel_L_est);
    if (abs(posX_cmd-posX_est)<0.05f)
        return true;
    else
        return false;
}

bool turnLeft(float vel_R_est, float vel_L_est)
{
        
	float theta_est = estimateTheta(vel_R_est, vel_L_est);
    if (theta_init==false)
    {
        current_theta = theta_est;
        theta_init=true;
    }
	float omega_cmd = thetaControl(TURN_90+current_theta, theta_est);
	differentialWheels = differentialVelocity(0.0f, omega_cmd);
	differentialWheels.dutyCycle_Right   = speedControl(differentialWheels.Vel_Right, vel_R_est);
	differentialWheels.dutyCycle_Left    = speedControl(differentialWheels.Vel_Left, vel_L_est);
    if ((abs(TURN_90+current_theta)-theta_est)<0.3f)
    {   
        theta_init = false;
        return true;
    }
    else
        return false;
}

bool turnRight(float vel_R_est, float vel_L_est)
{
	float theta_est = estimateTheta(vel_R_est, vel_L_est);
    if (theta_init==false)
    {
        current_theta = theta_est;
        theta_init=true;
    }
	float omega_cmd = thetaControl(-TURN_90+current_theta, theta_est);
	differentialWheels = differentialVelocity(0.0f, omega_cmd);
	differentialWheels.dutyCycle_Right   = speedControl(differentialWheels.Vel_Right, vel_R_est);
	differentialWheels.dutyCycle_Left    = speedControl(differentialWheels.Vel_Left, vel_L_est);
    if (abs((-TURN_90+current_theta)-theta_est)<0.05f)
    {   
        theta_init = false;
        return true;
    }
    else
        return false;

}
void updateStates()
{
    char motorID_L = 0;
    updateEncoderStates(motorID_L);

    // read left motor encoder values and send via UART
    long positionInCounts_L = g_counts[motorID_L];
    positionInRad_L = convertCountsToRad(positionInCounts_L);
    int velocityInCountsPerSample_L = g_deltaCountsSinceLastCall[motorID_L];
    velocityInRadPerSample_L = convertCountsToRad(velocityInCountsPerSample_L);
    
    char motorID_R=1;
    updateEncoderStates(motorID_R);

    // read motor encoder values and send via UART
    long positionInCounts_R = g_counts[motorID_R];
    positionInRad_R = convertCountsToRad(positionInCounts_R);
    int velocityInCountsPerSample_R = g_deltaCountsSinceLastCall[motorID_R];
    velocityInRadPerSample_R = convertCountsToRad(velocityInCountsPerSample_R);
}
