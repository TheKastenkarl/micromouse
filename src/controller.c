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

/* DEFINES */
#define SPEED_CONTROL_TP 20.0f
#define SPPED_CONTROL_INV_TJ 1.0f
#define POSITION_X_CONTROL_KP 0.010f
#define POSITION_Y_CONTROL_KP_HIGH_VEL 0.1f
#define THETA_CONTROL_KP_HIGH_VEL 0.4f
#define POSITION_Y_CONTROL_KP_LOW_VEL 0.001f
#define THETA_CONTROL_KP_LOW_VEL 0.8f
#define DUTY_CYCLE_MIN_MOVEMENT 0.0f
#define DUTY_CYCLE_THRESHOLD 0.3f
#define BASELINE 0.15f
#define TURN_90 1.57f
#define ALPHA_FILTER 0.7f
#define SAMPLE_TIME 0.05F

// Mid lvl fncs defines
#define ONE_CELL_DISTANCE_RAD 5.5F
#define IS_WALL_THRESHOLD_CONTROL 1500.0F
#define TARGET_DISTANCE_TO_WALL_LEFT 1800.0F
#define TARGET_DISTANCE_TO_WALL_RIGHT 1800.0F

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

/* GLOBAL DATA DEFINITIONS */
differentialDrive differentialWheels;
float positionInRad_L;
float velocityInRadPerSample_L;
float positionInRad_R;
float velocityInRadPerSample_R;


/* FUNCTION BODIES */

float speedControl(float velX_cmd, float velX_est, bool applyLim)
{	
    float lim;
    if (applyLim)
        lim=0.0035f;
    else
        lim=0.01f;
    
    if (velX_cmd>lim)
    velX_cmd=lim;

    if (velX_cmd<-lim)
    velX_cmd=-lim;
    
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
	wheelVelocity_L_R.Vel_Right = (2.0f*velAbs + omega*BASELINE)/2.0f;
	wheelVelocity_L_R.Vel_Left  = (2.0f*velAbs - omega*BASELINE)/2.0f;
	return wheelVelocity_L_R;
}

float estimateTheta(float vel_R_est, float vel_L_est)
{
	float velDiff = vel_R_est - vel_L_est; 
	thetaDiffIntegral = thetaDiffIntegral + velDiff * SAMPLE_TIME;
	return thetaDiffIntegral/BASELINE;
}

bool controlLoop(float posX_cmd, float posY_cmd, float posX_est, float posY_est, float vel_R_est, float vel_L_est, int turnCount, int direction)
{
	float velAbs    = positionXControl(posX_cmd, posX_est);
    float KP_Pos_Y  = POSITION_Y_CONTROL_KP_HIGH_VEL;
    float KP_Theta  = THETA_CONTROL_KP_HIGH_VEL;
 
	float theta_cmd = positionYControl(posY_cmd, posY_est,KP_Pos_Y*direction);
	float theta_est = estimateTheta(vel_R_est, vel_L_est);
	float omega_cmd = thetaControl(theta_cmd+(float)turnCount*1.57F, theta_est, KP_Theta);
	differentialWheels = differentialVelocity(velAbs, omega_cmd);
	differentialWheels.dutyCycle_Right   = speedControl(differentialWheels.Vel_Right, vel_R_est, true);
	differentialWheels.dutyCycle_Left    = speedControl(differentialWheels.Vel_Left, vel_L_est, true);
    
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

bool maintainPosOrient(int timeDelay){
    if (delayCounter>timeDelay)
        return true;
    else
    {
        delayCounter=delayCounter+1;
        return false;
    }
}

void Forward(float target)
{
    bool isFinished=false;
    float target_distance_to_wall;
    float distance_to_wall;
    LED1=~LED1;
    while(isFinished==false)
    {   
        updateStates();
        int direction;
        if (is_wall_left_ctrl())
        {
            distance_to_wall = getLeftIR(1.0F);
            target_distance_to_wall = TARGET_DISTANCE_TO_WALL_LEFT;
            direction = -1;
        }
        else if (is_wall_right_ctrl())
        {
            distance_to_wall = getRightIR(1.0F);
            target_distance_to_wall = TARGET_DISTANCE_TO_WALL_RIGHT;
            direction = 1;
        }
        else
        {
            distance_to_wall = 0.0F;
            target_distance_to_wall = 0.0F;
            direction = 1;
        }
        
        if ((target_distance_to_wall)-(distance_to_wall)<50.0F)
            target_distance_to_wall=distance_to_wall;

        isFinished = controlLoop(target,target_distance_to_wall,positionInRad_L,distance_to_wall,velocityInRadPerSample_R,velocityInRadPerSample_L,turnCount, direction);
        runMotor(differentialWheels.dutyCycle_Right, 1, 0);
        runMotor(differentialWheels.dutyCycle_Left, 0, 0);
        
    }
}

void Move_Forward(int cells)
{   
    updateStates();
    float target = positionInRad_L+(float)cells*ONE_CELL_DISTANCE_RAD;
    bool terminate=false;
    delayCounter = 0;
    while (terminate==false)
    {
        
        Forward(target);
        LED0=0;
        if (maintainPosOrient(1000))
        {
            runMotor(0, 1, 0);
            runMotor(0, 0, 0);
            terminate = true;
            LED0=1;
        }
    }
}

bool turnLeftRight(float cmd, float vel_R_est, float vel_L_est)
{
    float theta_est = estimateTheta(vel_R_est, vel_L_est);
    float omega_cmd = thetaControl(cmd, theta_est,THETA_CONTROL_KP_LOW_VEL);
	differentialWheels = differentialVelocity(0.0f, omega_cmd);
	differentialWheels.dutyCycle_Right   = speedControl(differentialWheels.Vel_Right, vel_R_est, false);
	differentialWheels.dutyCycle_Left    = speedControl(differentialWheels.Vel_Left, vel_L_est, false);
    
    if (abs(cmd-theta_est)<0.01f)
        return true;
    else
        return false;
}

void TurnMotion(float cmd) // 1 for Left and -1 for Right
{       
    bool isFinished=false;
    while(isFinished==false)
    {   
        updateStates();
        isFinished=turnLeftRight(cmd, velocityInRadPerSample_R, velocityInRadPerSample_L);
        runMotor(differentialWheels.dutyCycle_Right, 1, 0);
        runMotor(differentialWheels.dutyCycle_Left, 0, 0);
    }
}

void TurnAction(int direction) // 1 for Left and -1 for Right
{
    updateStates();
    float current_theta = estimateTheta(velocityInRadPerSample_R, velocityInRadPerSample_L);
    float target = direction*TURN_90+current_theta;
    turnCount =turnCount+direction;
    bool terminate=false;
    delayCounter = 0;
    
    while (terminate==false)
    {
        
        TurnMotion(target);
        LED0=0;
        if (maintainPosOrient(5000))
        {
            runMotor(0, 1, 0);
            runMotor(0, 0, 0);
            terminate = true;
            LED0=1;
        }

    }    
}

int is_wall_right_ctrl() {
    if (getRightIR(1.0F)>IS_WALL_THRESHOLD_CONTROL)
        return 1;
    else
        return 0;
}

int is_wall_left_ctrl() {
    if (getLeftIR(1.0F)>IS_WALL_THRESHOLD_CONTROL)
        return 1;
    else
        return 0;
}
