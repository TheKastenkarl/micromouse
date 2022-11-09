/* PROJECT INCLUDE FILES */

#include "controller.h"

/* SYTSTEM INCLUDE FILES */

#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stdlib.h>
#include "motorEncoders.h"
#include "tests.h"
#include "IOconfig.h" 
#include "irSensors.h"
#include "motors.h"
#include "robot.h"

/* DEFINES */
#define SPEED_CONTROL_TP 2.0f
#define SPEED_CONTROL_TP_ADJUST 0.7f

#define SPPED_CONTROL_INV_TJ 1.0f

#define POSITION_X_CONTROL_KP_FORWARD 0.2f
#define POSITION_X_CONTROL_KP_ADJUST 0.2f
#define THETA_CONTROL_KP 5.0f

#define WALL_KP 0.1F

#define DUTY_CYCLE_MIN_MOVEMENT 0.0f
#define DUTY_CYCLE_THRESHOLD 0.6f

#define BASELINE 0.16f
#define TURN_90 1.57f
#define ALPHA_FILTER 0.7f
#define SAMPLE_TIME 0.05F

#define VEL_BASE_LIM 0.2F
#define VEL_WALL_LIM 0.03F

#define ONE_CELL_DISTANCE_RAD 5.9F
#define FOLLOW_WALL_THRESHOLD 1300.0F

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
int turnCount;
int delayCounter;
float theta_estimation;

/* GLOBAL DATA DEFINITIONS */
differentialDrive differentialWheels;
float positionInRad_L;
float velocityInRadPerSample_L;
float positionInRad_R;
float velocityInRadPerSample_R;
float target_forward;
float target_turn;
int instruction;


/* FUNCTION BODIES */

float speedControl(float velX_cmd, float velX_est, bool applyLim, bool adjust)
{	    
    float Kp_Speed;
    if (adjust==false)
        Kp_Speed= SPEED_CONTROL_TP;
    else
        Kp_Speed= SPEED_CONTROL_TP_ADJUST;
    
    float lim;
    if (applyLim)
        lim=0.1f;
    else
    {
        if (adjust==false)
            lim=0.3f;
        else
            lim=0.1f;
    }
    
    if (velX_cmd>lim)
        velX_cmd=lim;

    if (velX_cmd<-lim)
        velX_cmd=-lim;
    
	velX_est_Filtered = ALPHA_FILTER * (velX_est - velX_est_Filtered) + velX_est_Filtered;
	float velError = velX_cmd-velX_est_Filtered;
	float velError_TP = velError * Kp_Speed;
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

static float positionXControl(float posX_cmd, float posX_est, float Kp)
{   
	return Kp * (posX_cmd - posX_est);
}

static float thetaControl(float theta_cmd, float theta_est, float KP)
{
	return KP * (theta_cmd - theta_est);
}

static float positionYControl(float posY_cmd, float posY_est, float KP)
{
	return KP * (posY_cmd - posY_est);
}

static differentialDrive differentialVelocity(float velAbs, float omega)
{
	differentialDrive wheelVelocity_L_R;
	wheelVelocity_L_R.Vel_Right = (2.0f*velAbs + omega)/2.0f;
	wheelVelocity_L_R.Vel_Left  = (2.0f*velAbs - omega)/2.0f;
	return wheelVelocity_L_R;
}

float estimateTheta(float vel_R_est, float vel_L_est)
{
	float velDiff = vel_R_est - vel_L_est; 
	thetaDiffIntegral = thetaDiffIntegral + velDiff * SAMPLE_TIME;
	return thetaDiffIntegral/BASELINE;
}

bool controlLoop(float posX_cmd, float posY_cmd, float posX_est, float posY_est, float vel_R_est, float vel_L_est, bool adjust, int direction)
{
    float Kp_Pos_X;
    if (adjust==false)
        Kp_Pos_X = POSITION_X_CONTROL_KP_FORWARD;
    else
        Kp_Pos_X = POSITION_X_CONTROL_KP_ADJUST;
    
    if (adjust==true && abs(posX_cmd - posX_est)<50.0f)
        posX_cmd=posX_est;
            
	float velAbs    = positionXControl(posX_cmd, posX_est, Kp_Pos_X);
    float distance_to_wall_Left;
    float distance_to_wall_Right;
    float Vel_wall;
    
    if (velAbs>VEL_BASE_LIM)
        velAbs=VEL_BASE_LIM;
    if (velAbs<-VEL_BASE_LIM)
        velAbs=-VEL_BASE_LIM;
    
    if (is_wall_left() && is_wall_right())
    {
        distance_to_wall_Left  = getLeftIR(1.0F);
        distance_to_wall_Right = getRightIR(1.0F);
        Vel_wall = WALL_KP * (distance_to_wall_Left-distance_to_wall_Right);
    }
    else if(is_wall_left())
    {
        distance_to_wall_Left  = getLeftIR(1.0F);
        Vel_wall = WALL_KP * (distance_to_wall_Left-FOLLOW_WALL_THRESHOLD);
    }
    else if(is_wall_right())
    {
        distance_to_wall_Right  = getRightIR(1.0F);
        Vel_wall = WALL_KP * (FOLLOW_WALL_THRESHOLD-distance_to_wall_Right);
    }

    if (Vel_wall>VEL_WALL_LIM)
        Vel_wall=VEL_WALL_LIM;

    if (Vel_wall<-VEL_WALL_LIM)
        Vel_wall=-VEL_WALL_LIM;
	
	differentialWheels = differentialVelocity(velAbs, 0);
	differentialWheels.dutyCycle_Right   = speedControl(velAbs - Vel_wall, vel_R_est, false, adjust);
	differentialWheels.dutyCycle_Left    = speedControl(velAbs + Vel_wall, vel_L_est, false, adjust);
    
   char sendData[2][100];
        sprintf(sendData[1], "pos: %.2f", differentialWheels.dutyCycle_Right);
        for (int i = 0; i < 2; i++) 
            //sendUART1(sendData[i], 1);
        
    if (abs(posX_cmd-posX_est)<0.05f)
        return true;
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

void mid_Level_Forward(int cells)
{           
    LED0=1;
    instruction = 0;
    target_forward = positionInRad_L+(float)cells*ONE_CELL_DISTANCE_RAD;
    bool terminate = false;
    
    while (terminate==false)
    {
        if (abs(target_forward-positionInRad_L)<0.05f)
        {   
            if(is_wall_front())
                sleep(1500000);
            else
                sleep(1500000);
            terminate = true;
            LED0=0;
        }
    }
}

void low_level_Forward()
{
        updateStates();
        controlLoop(target_forward,0,positionInRad_L,0,velocityInRadPerSample_R,velocityInRadPerSample_L,false,0);
        runMotor(differentialWheels.dutyCycle_Right, 1, 0);
        runMotor(differentialWheels.dutyCycle_Left, 0, 0);

}

void low_level_Turn()
{   
    updateStates();
    theta_estimation = estimateTheta(velocityInRadPerSample_R, velocityInRadPerSample_L);
    float omega_cmd = thetaControl(target_turn, theta_estimation,THETA_CONTROL_KP);
	differentialWheels = differentialVelocity(0.0f, omega_cmd);
	differentialWheels.dutyCycle_Right   = speedControl(differentialWheels.Vel_Right, velocityInRadPerSample_R, true, false);
	differentialWheels.dutyCycle_Left    = speedControl(differentialWheels.Vel_Left, velocityInRadPerSample_L, true, false);
    runMotor(differentialWheels.dutyCycle_Right, 1, 0);
    runMotor(differentialWheels.dutyCycle_Left, 0, 0);
}

void mid_Level_Turn(int direction)
{   
    if(is_wall_front())
    {
        instruction = -1;
    }
    //sleep(10000000);
    LED1=1;
    instruction = 1;
    target_turn = direction*TURN_90+theta_estimation;
    bool terminate=false;
    
    while (terminate==false)
    {
        if (direction*(target_turn-theta_estimation)<0.005f)
        {    
            sleep(2000000);
            terminate = true;
            LED1=0;
        }
    }
}

bool front_wall_corr()
{   
    updateStates();
    controlLoop(1700.0F,0,getFrontIR(1.0F),0,velocityInRadPerSample_R,velocityInRadPerSample_L,true,0);
    runMotor(differentialWheels.dutyCycle_Right, 1, 0);
    runMotor(differentialWheels.dutyCycle_Left, 0, 0);
}