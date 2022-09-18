#ifndef CONTROLLER_H
#define	CONTROLLER_H

/* PROJECT INCLUDE FILES */

#include <stdbool.h>

/* TYPEDEFS */

typedef struct{
	float Vel_Right, Vel_Left, dutyCycle_Right, dutyCycle_Left;
} differentialDrive;

/* GLOBAL DATA DEFINITIONS */

extern differentialDrive differentialWheels;
extern float positionInRad_L;
extern float velocityInRadPerSample_L;
extern float positionInRad_R;
extern float velocityInRadPerSample_R;
extern int   turnCount;

/* FUNCTION PROTOTYPES */

extern bool controlLoop(float posX_cmd, float posY_cmd, float posX_est, float posY_est, float vel_R_est, float vel_L_est, int turnCount, int direction);
extern bool turnLeft(float vel_R_est, float vel_L_est);
extern bool turnRight(float vel_R_est, float vel_L_est);
extern float estimateTheta(float vel_R_est, float vel_L_est);
extern float speedControl(float velX_cmd, float velX_est, bool applyLim);
extern void updateStates();
extern void Forward(float target);
extern void Move_Forward(int cells); 
extern void TurnMotion(float cmd);
extern void TurnAction(int direction);
bool is_wall_right_ctrl();
bool is_wall_left_ctrl();

#endif	/* CONTROLLER_H */




