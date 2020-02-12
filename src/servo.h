//######################## LIB FOR SERVO #########################
#include <avr/io.h>
//#include <ctype.h>

#pragma once

#define SERVO_PIN PB1

//init servo pwm
void init_servoPWM();

//set angle to servo
void servo_setAngle(float angle);
