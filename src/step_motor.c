#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
//#include <util/delay.h>
#include <math.h>
#include "step_motor.h"
#include "serial_printf.h"

#define BOTTOM (256-50)
#define TIME 0.0002

volatile int _STEPS_2=0, _STEP_CNT_2=0, _LAST_STEP_2=0, _STEP_DELAY_2=2;
volatile int _STEPS_0=0, _STEP_CNT_0=0, _LAST_STEP_0=0, _STEP_DELAY_0=2;
volatile uint64_t COUNTER=0;

void _step(StepMotor *Motor, int steps);

void setPORTB(uint8_t PIN, uint8_t ON){
    if(ON)
        PORTB |= (1<<PIN);
    else
        PORTB &= ~(1<<PIN);
}

void setPORTD(uint8_t PIN, uint8_t ON){
    if(ON)
        PORTD |= (1<<PIN);
    else
        PORTD &= ~(1<<PIN);
}

double _abs(double X){
    return (X<0)? -X : X;
}

double toSteps(double angle)
{
    return (SCALE*angle/1.8);
}

double toAngle(double steps)
{
    return (1.8*steps/SCALE);
}

void stepMotor_init(StepMotor *Motor_1, StepMotor *Motor_2)
{
    // ########################## MOTOR 1 ############################

    Motor_1->current_step = toSteps(0); // which step the motor is on
    Motor_1->direction = 0;                   // motor direction
    Motor_1->max_steps = SCALE*200;           // total number of steps for Motor motor
    Motor_1->step_delay = MAX_SPEED;          // delay between steps, in us, based on speed

    // Driver pins
    Motor_1->motor_pin_dir = PD2;
    Motor_1->motor_pin_step = PD3;
    Motor_1->motor_timer = 2;     //Using timer 0

    // setup the pins on the microcontroller:
    DDRD |= (1<<PD4) | (1<<PD5);

    // ######################## MOTOR 2 ###############################

    Motor_2->current_step = toSteps(-90);     // which step the motor is on
    Motor_2->direction = 0;                   // motor direction
    Motor_2->max_steps = SCALE*200;           // total number of steps for Motor motor
    Motor_2->step_delay = MAX_SPEED;          // delay between steps, in us, based on speed

    // Driver pins
    Motor_2->motor_pin_dir = PD4;
    Motor_2->motor_pin_step = PD5;
    Motor_2->motor_timer = 0;     //Using timer 2

    // setup the pins on the microcontroller:
    DDRD |= (1<<PD2) | (1<<PD3);  

    //########################### SETUP TIMER 0 ####################################

    TCCR0B = 0;
    TIFR0 = (7<<TOV0);
    TCCR0A = 0;
    TCNT0 = BOTTOM;
    TIMSK0 = 1;
    TCCR0B |= (0<<CS02) | (1<<CS01) | (1<<CS00);

    sei();
}

int updateMotor(StepMotor *Motor)
{
  int step_update = Motor->current_step;
  if(Motor->motor_timer == 0){
    if(Motor->direction == 1){
      cli();
      step_update += _STEPS_0 - _STEP_CNT_0;
      sei();
    }
    else{
      cli();
      step_update -= _STEPS_0 - _STEP_CNT_0;
      sei();
    }
  }
  else if(Motor->motor_timer == 2){
    if(Motor->direction == 1){
      cli();
      step_update += _STEPS_2 - _STEP_CNT_2;
      sei();
    }
    else{
      cli();
      step_update -= _STEPS_2 - _STEP_CNT_2;
      sei();
    }
  }
  if(step_update<0)
    step_update += Motor->max_steps;
  else if(step_update>=Motor->max_steps)
    step_update -= Motor->max_steps;

  return step_update;
}

void motor_Stop(StepMotor *Motor)
{
  Motor->current_step = updateMotor(Motor);

  if(Motor->motor_timer == 0){
    _STEPS_0 = 0;
    _STEP_CNT_0 = 0;
  } else if(Motor->motor_timer == 2){
    _STEPS_2 = 0;
    _STEP_CNT_2 = 0;
  }
}

void stepMotor(StepMotor *Motor, int steps_to_move)
{
  if(steps_to_move == 0) return;
  Motor->current_step = updateMotor(Motor);

  int steps_left = _abs(steps_to_move);  // how many steps to take

  // determine direction based on whether steps_to_mode is + or -:
  if (steps_to_move > 0) Motor->direction = 0;
  if (steps_to_move < 0) Motor->direction = 1;

  //set direction
  if(Motor->direction==1)
      setPORTD(Motor->motor_pin_dir, 1);
  else
      setPORTD(Motor->motor_pin_dir, 0);

  //Begin stepping
  _step(Motor, steps_left);

  //if motor does a full revolution, update it
  Motor->current_step += steps_to_move;
  if(Motor->current_step<0)
    Motor->current_step += Motor->max_steps;
  else if(Motor->current_step>=Motor->max_steps)
    Motor->current_step -= Motor->max_steps;
}

void motor_Move(StepMotor *Motor, double angle)
{
    motor_Stop(Motor);

    int steps_totake = (int) round(toSteps(angle));

    stepMotor(Motor, steps_totake);
}

void motor_setAngle(StepMotor *Motor, double angle)
{
    motor_Stop(Motor);
  
    double angle_totake = motor_checkAngle(Motor, angle);

    motor_Move(Motor, angle_totake);
}

double motor_checkAngle(StepMotor *Motor, double angle)
{
    int update_step = updateMotor(Motor);

    double remainder = (angle-toAngle(update_step))-(int)(angle-toAngle(update_step));
    double angle_totake = (int)(angle-toAngle(update_step)) % 360;

    if(angle_totake < -180){
        angle_totake += 360+remainder;
    }
    else if(angle_totake > 179){
        angle_totake -= 360+remainder;
    }
    else{
        angle_totake += remainder;
    }

    return angle_totake;
}

void motor_setSpeed(StepMotor *Motor, float speed)
{
  Motor->step_delay = speed;
}

int isMoving(StepMotor *Motor)
{
  if(Motor == NULL) return -1;

  if(Motor->motor_timer == 0) return _STEPS_0;
  if(Motor->motor_timer == 2) return _STEPS_2;

  return -1;
}

void _step(StepMotor *Motor, int steps)
{
  if(Motor->step_delay < MAX_SPEED) Motor->step_delay = MAX_SPEED;
  if(Motor->motor_timer == 0){
    _STEP_DELAY_0 = (int)round(Motor->step_delay/TIME);
    _STEP_CNT_0 = 0;
    _STEPS_0 = steps;
    _LAST_STEP_0 = COUNTER;
  }
  else if(Motor->motor_timer == 2){
    _STEP_DELAY_2 = (int)round(Motor->step_delay/TIME);
    _STEPS_2 = steps;
    _LAST_STEP_2 = COUNTER;
  }
}

// ################### INTERRUPT STUFF ######################

ISR(TIMER0_OVF_vect)
{
  TCNT0 = BOTTOM;

  if((COUNTER-_LAST_STEP_0) % _STEP_DELAY_0 == 0){
    if(_STEP_CNT_0 < _STEPS_0){
      setPORTD(PD5, 1);
      _STEP_CNT_0++;
    }
    else{
      _STEP_CNT_0 = 0;
      _STEPS_0 = 0;
    }
  }
  else {
    setPORTD(PD5, 0);
  }

  if((COUNTER-_LAST_STEP_2) % _STEP_DELAY_2 == 0){
    if(_STEP_CNT_2 < _STEPS_2){
      setPORTD(PD3, 1);
      _STEP_CNT_2++;
    }
    else{
      _STEP_CNT_2 = 0;
      _STEPS_2 = 0;
    }
  }
  else {
    setPORTD(PD3, 0);
  }

  COUNTER++;
  //printf("%d\n", COUNTER);
}