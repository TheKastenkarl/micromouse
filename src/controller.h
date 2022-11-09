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
extern float target_forward;
extern float target_turn;
extern int instruction;

/* FUNCTION PROTOTYPES */

extern bool controlLoop(float posX_cmd, float posY_cmd, float posX_est, float posY_est, float vel_R_est, float vel_L_est, bool adjust, int direction);
extern bool turnLeft(float vel_R_est, float vel_L_est);
extern bool turnRight(float vel_R_est, float vel_L_est);
extern float estimateTheta(float vel_R_est, float vel_L_est);
extern float speedControl(float velX_cmd, float velX_est, bool applyLim, bool adjust);
extern void updateStates();
extern void low_level_Forward();
extern void mid_Level_Forward(int cells);
extern void mid_Level_Turn(int direction);
extern void low_level_Turn();
extern bool front_wall_corr();
#endif	/* CONTROLLER_H */




