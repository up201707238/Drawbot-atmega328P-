#include <avr/io.h>
#include "servo.h"

void init_servoPWM()
{
    //Stop Timer 1
    TCCR1B = 0;

    //Mode 14
    TCCR1B = (1<<WGM13) | (1<<WGM12);

    //clear on compare with A, and mode
    TCCR1A = (1<<COM1A1) | (1<<WGM11);

    //Restart timer counter
    TCNT1 = 0;

    //TOP = 20ms T
    ICR1 = 40000;

    //Compare. Pulse 1.5ms
    OCR1A = 3000;

    //Prescaler 8
    TCCR1B |= (1<<CS11);

    DDRB |= (1<<SERVO_PIN);
    
}

void servo_setAngle(float angle)
{
    OCR1A = (int)(1000.0/90.0*(angle) + 3000.0);
}