//###################### LIB FOR STEP MOTOR ######################
//#include <ctype.h>
#include <avr/eeprom.h>

#pragma once

#define F 16000000
#define _8_bit 256
#define MAX_SPEED 0.002
#define MIN_SPEED 1
#define DIV_PER_STEP 16 //Mode: 1/16 step
#define R_RATIO (28/7)  //ratio between motor/cog radius
#define SCALE (R_RATIO*DIV_PER_STEP)

extern uint16_t EEMEM alfa_eeprom, beta_eeprom;

typedef struct{
    int direction;              // Direction of rotation
    float step_delay;           // delay between steps, in us, based on speed
    int max_steps;              // total number of steps this motor can take
    int current_step;           // which step the motor is on

    // motor pin numbers:
    int motor_pin_dir;
    int motor_pin_step;
    int motor_timer;            //id of the motor
} StepMotor;

//init configurations (pins, speed...)
void stepMotor_init(StepMotor *Motor_1, StepMotor *Motor_2);

//Move number of steps
void stepMotor(StepMotor *Motor, int number_of_steps);

//Move angle from current position
void motor_Move(StepMotor *Motor, double angle);

//Set position from reference (0 graus)
void motor_setAngle(StepMotor *Motor, double angle);

//Return the angle to take for target position
double motor_checkAngle(StepMotor *Motor, double angle);

//Set speed to motor
void motor_setSpeed(StepMotor *Motor, float speed);

//Stop the motor
void motor_Stop(StepMotor *Motor);

//flag if the motor is moving
int isMoving(StepMotor *Motor);

//convert angle to steps
double toSteps(double angle);

//convert steps to angle
double toAngle(double steps);