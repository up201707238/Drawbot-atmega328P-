//########################## DRAWBOT ###########################
#include <avr/io.h>
//#include <util/delay.h>
#include <avr/eeprom.h>
#include <ctype.h>
#include <math.h>
#include "step_motor.h"
#include "nokia5110.h"
#include "lcd.h"
#include "serial_printf.h"
#include "servo.h"

#define ARM_L1 205       
#define ARM_L2 200      
#define BASE_X -150
#define BASE_Y 220

#define WINDOW_X 210        //A4 paper
#define WINDOW_Y 249.4

StepMotor m_base, m_elbow;
uint8_t servo_isup=1;

uint16_t EEMEM nA, nE, nI, nO, nU;

typedef struct
{
    float x, y;
} Point;

typedef struct
{
    Point p;
    uint8_t en;
} Command;

typedef struct
{
    Command *cmd;
    unsigned int len;
} l_Command;

Point pinit = {0,0};        //origem
Point cursor;

Command easteregg[8] = {{{0,180},0},
                        {{0,140},1},
                        {{80,140},1},
                        {{80,100},1},
                        {{80,180},0},
                        {{40,180},1},
                        {{40,100},1},
                        {{0,100},1}};

Command quadrado [5] = {{{50,50},0},
                        {{150,50},1},
                        {{150,150},1},
                        {{50,150},1},
                        {{50,50},1}};

Command cruz [5] =      {{{50,50},0},
                        {{150,150},1},
                        {{50,150},0},
                        {{150,50},1},
                        {{150,50},1}};

Command A [5] =         {{{0,0},0},
                        {{5,10},1},
                        {{10,0},1},
                        {{0,5},0},
                        {{7.6,5},1}};

Command E [6] =         {{{10,0},0},
                        {{0,0},1},
                        {{0,10},1},
                        {{10,10},1},
                        {{10,5},0},
                        {{0,5},1}};

Command I [2] =         {{{5,0},0},
                        {{5,10},1}};

Command O [5] =         {{{0,0},0},
                        {{0,10},1},
                        {{10,10},1},
                        {{10,0},1},
                        {{0,0},1}};

Command U [4] =         {{{0,10},0},
                        {{0,0},1},
                        {{10,0},1},
                        {{10,10},1}};

uint8_t mode_select=0;

//auxiliar functions
double sqr(double x);
Point circle_intersection(Point c_1, Point c_2);
uint8_t isValidPoint(Point p);

//Pen functions
void penUP();
void penDOWN();

//arm functions
void arm_toPoint(Point p);
void arm_Draw(l_Command l_cmds);
void arm_Move(Point p);
uint8_t arm_isMoving();
void arm_Reset();
void arm_Update();
void arm_Right();
void arm_Left();
void arm_Up();
void arm_Down();

int main(void)
{
    //Setup
    uint8_t data_;
    l_Command demos;
    init_servoPWM();
    printf_init();
    nokia_lcd_init();
    nokia_lcd_power(1);
    stepMotor_init(&m_base, &m_elbow);
    motor_setSpeed(&m_base, MAX_SPEED);
    motor_setSpeed(&m_elbow, MAX_SPEED);
    penUP();
    printf("Select mode.\n");
    
    // * * * DRAWING LOOP * * *
    while(1)
    {
        if(serial_receive_ready()){
            data_ = serial_receive();
            printf("%c\n", toupper(data_));

        if(mode_select == 0){       //Select Mode
                if(data_ == '1'){
                    mode_select = 1;
                    printf("Manual mode\n");
                }
                else if(data_ == '2'){
                    mode_select = 2;
                    printf("Demo1\n");
                }
                else if(data_ == '3'){
                    mode_select = 3;
                    printf("Demo2\n");
                }
                else if(data_ == '4'){
                    mode_select = 4;
                    printf("Letras\n");
                    penUP();
                    Point px = {59,220};
                    arm_toPoint(px);
                    while(arm_isMoving());
                }
                else if(data_ == '9'){
                    page2();
                }
                else if(data_ == '0'){
                    page1();
                }
        }

        if(mode_select == 1){       // ############### MANUAL ###################
                //manualMode();
                switch(data_)
                {
                    case 'a':
                    arm_Left();
                    break;

                    case 'd':
                    arm_Right();
                    break;

                    case 'w':
                    arm_Up();
                    break;

                    case 's':
                    arm_Down();
                    break;

                    case 'f':
                    if(!isMoving(&m_base))
                        motor_Move(&m_base, 5);
                        arm_Update();
                        break;
                    
                    case 'h':
                    if(!isMoving(&m_base))
                        motor_Move(&m_base, -5);
                        arm_Update();
                        break;

                    case 'j':
                        if(!isMoving(&m_elbow))
                        motor_Move(&m_elbow, 5);
                        arm_Update();
                        break;

                    case 'l':
                        if(!isMoving(&m_elbow))
                        motor_Move(&m_elbow, -5);
                        arm_Update();
                        break;

                    case ' ':
                        if(servo_isup){
                            penDOWN();
                            //printf("Down.");
                            servo_isup = 0;
                        }
                        else{
                            penUP();
                            //printf("Up.");
                            servo_isup = 1;
                        }
                        break;

                    case '+':
                        motor_setSpeed(&m_base, 0.0004);
                        motor_setSpeed(&m_elbow, 0.0004);
                        break;

                    case '-':
                        motor_setSpeed(&m_base, MIN_SPEED);
                        motor_setSpeed(&m_elbow, MIN_SPEED);
                        break;

                    case 'z':
                        demos.cmd = easteregg;
                        demos.len = 8;
                        arm_Draw(demos);
                        break;

                    case 'x':
                        m_base.current_step = toSteps(0);
                        m_elbow.current_step = toSteps(-90);
                        arm_Update();
                        break;

                    case 'r':
                        arm_Reset();
                        while(arm_isMoving());
                        
                        break;

                    case '0':
                        mode_select = 0;
                        break;

                    default:
                    break;
            }
        }

        if(mode_select == 2){      // ################## DEMO 1 #####################
            demos.cmd=quadrado;
            demos.len=5;
            arm_Draw(demos);
            while(arm_isMoving());
            mode_select = 0;
        }

        if(mode_select == 3){      // ################## DEMO 2 #####################
            demos.cmd=cruz;
            demos.len=5;
            arm_Draw(demos);
            while(arm_isMoving());
            mode_select = 0;
        }

        if(mode_select == 4){       // ################### LETRAS #####################
            penUP();
            switch (data_)
            {
            case 'a':
                eeprom_update_word(&nA, eeprom_read_word(&nA)+1);
                demos.cmd = A;
                demos.len = 5;
                arm_Draw(demos);
                while(arm_isMoving()){
                    
                    /*sprintf(a, "%d", (int)toAngle(m_base.current_step));
                    sprintf(b, "%d", (int)toAngle(m_base.current_step));
                    sprintf(x, "%d", (int)cursor.x);
                    sprintf(y, "%d", (int)cursor.y);
                    Drawing(a,b,x,y);*/
                }
                break;
                
            case 'e':
                eeprom_update_word(&nE, eeprom_read_word(&nE)+1);
                demos.cmd = E;
                demos.len = 6;
                arm_Draw(demos);
                while(arm_isMoving());
                break;
            
            case 'i':
                eeprom_update_word(&nI, eeprom_read_word(&nI)+1);
                demos.cmd = I;
                demos.len = 2;
                arm_Draw(demos);
                while(arm_isMoving());
                break;

            case 'o':
                eeprom_update_word(&nO, eeprom_read_word(&nO)+1);
                demos.cmd = O;
                demos.len = 5;
                arm_Draw(demos);
                while(arm_isMoving());
                break;

            case 'u':
                eeprom_update_word(&nU, eeprom_read_word(&nU)+1);
                demos.cmd = U;
                demos.len = 4;
                arm_Draw(demos);
                while(arm_isMoving());
                break;
                
            case '0':
                mode_select = 0;
                break;
            }
        }
        } //serial
    } //loop
} //main

// ################## FUNCTIONS ###########################

void penUP()
{
    servo_setAngle(-90);
    servo_isup=1;
    //_delay_ms(10);
}

void penDOWN()
{
    servo_setAngle(-180);
    servo_isup=0;
    //_delay_ms(10);
}

double sqr(double x)
{
    return x*x;
}

Point circle_intersection(Point c_1, Point c_2)
{
    double d = sqrt(sqr(c_1.x-c_2.x) + sqr(c_1.y-c_2.y));       // =sqr((x1−x2)2+(y1−y2)2) distance

    double l = (sqr(ARM_L1)-sqr(ARM_L2)+sqr(d)) / (2*d);

    double h = sqrt(sqr(ARM_L1) - sqr(l));

    Point P;
    P.x = l/d*(c_2.x - c_1.x) - h/d*(c_2.y - c_1.y) + c_1.x;
    P.y = l/d*(c_2.y - c_1.y) + h/d*(c_2.x - c_1.x) + c_1.y;

    return P;
}

void arm_toPoint(Point p)
{
    //printf("%d, %d\n", (int)p.x, (int)p.y);
    if(!isValidPoint(p)){
        //printf("Unreachable.\n");
        return;
    }

    double alfa, beta, alfa_t, beta_t, ratio;
    Point c1; c1.x=BASE_X; c1.y=BASE_Y;

    Point pt = circle_intersection(c1, p);
    //printf("%d , %d\n", (int)pt.x, (int)pt.y);

    alfa = atan2(pt.y-BASE_Y, pt.x-BASE_X)*180.0/(M_PI);
    beta = atan2(p.y-pt.y, p.x-pt.x)*180.0/(M_PI);
    //printf("Alfa: %d\n", (int)alfa);
    //printf("Beta: %d\n\n", (int)beta);

    alfa_t = motor_checkAngle(&m_base, alfa);
    beta_t = motor_checkAngle(&m_elbow, beta);
    //printf("t_A %d, t_B %d\n", (int)alfa_t, (int)beta_t);

    if(fabs(alfa_t) >= fabs(beta_t))
    {
        if(beta_t==0) beta_t=1;
        ratio = fabs(alfa_t/beta_t);
        motor_setSpeed(&m_base, MAX_SPEED);
        motor_setSpeed(&m_elbow, MAX_SPEED*ratio);
    }
    else
    {
        if(alfa_t==0) alfa_t=1;
        ratio = fabs(beta_t/alfa_t);
        motor_setSpeed(&m_elbow, MAX_SPEED);
        motor_setSpeed(&m_base, MAX_SPEED*ratio);
    }
    //printf("R %d\n", (int)ratio);
    
    motor_setAngle(&m_base, alfa);
    motor_setAngle(&m_elbow, beta);
    arm_Update();
}

void arm_Draw(l_Command l_cmds)
{
    if(mode_select == 4){       //for letters
        arm_Update();
        Point px = cursor;
        Point p1;          
        for(int i=0; i<l_cmds.len; i++)
        {
            while (arm_isMoving());      //wait to finish previous move

            l_cmds.cmd[i].en? penDOWN(): penUP();             //Write or not

            p1.x=px.x+1.5*l_cmds.cmd[i].p.x;
            p1.y=px.y+2.0*l_cmds.cmd[i].p.y;
            
            arm_Move(p1);    //Move to point 'p'
        }
        while(arm_isMoving());
        penUP();
        px.x += 15 + 5;
        arm_toPoint(px);
        return;  
    }
    for(int i=0; i<l_cmds.len; i++)
    {
        while (arm_isMoving());      //wait to finish previous move

        l_cmds.cmd[i].en? penDOWN(): penUP();             //Write or not
        
        arm_Move(l_cmds.cmd[i].p);    //Move to point 'p'
    }
    while(arm_isMoving());
    arm_Reset();
}

void arm_Move(Point p)
{
    const uint16_t nseg = 10;        //n segments between points
    Point vec; vec.x=(p.x-cursor.x)/nseg; vec.y=(p.y-cursor.y)/nseg;
    Point pseg=cursor;

    for(int i=1; i<nseg; i++){      //nseg-1 times
        pseg.x += vec.x;
        pseg.y += vec.y;
        arm_toPoint(pseg);
        while(arm_isMoving());
    }
    arm_toPoint(p);
    while(arm_isMoving());
    arm_Update();
}

void arm_Reset()
{
    penUP();
    motor_setSpeed(&m_base, MAX_SPEED);
    motor_setSpeed(&m_elbow, MAX_SPEED);
    motor_setAngle(&m_base, 0);
    motor_setAngle(&m_elbow, -90);
}

uint8_t arm_isMoving()
{
    return (isMoving(&m_base) || isMoving(&m_elbow));
}

uint8_t isValidPoint(Point p)
{
    float dist = sqrt(sqr(p.x-BASE_X)+sqr(p.y-BASE_Y));

    if(dist < 160 || dist >= 390)
        return 0;

    return 1;
}

void arm_Update()
{
    double alfa, beta;

    alfa = toAngle(m_base.current_step)*M_PI/180.0;
    beta = toAngle(m_elbow.current_step)*M_PI/180.0;

    cursor.x = BASE_X + cos(alfa)*ARM_L1 + cos(beta)*ARM_L2;
    cursor.y = BASE_Y + sin(alfa)*ARM_L1 + sin(beta)*ARM_L2;
    //printf("c %d,%d\n", (int)(cursor.x), (int)(cursor.y));
}

void arm_Right()
{
    Point p;
    arm_Update();
    p.x = cursor.x+10.0;
    p.y = cursor.y;

    arm_toPoint(p);
}

void arm_Left()
{
    Point p;
    arm_Update();
    p.x = cursor.x-10.0;
    p.y = cursor.y;

    arm_toPoint(p);
}

void arm_Up()
{
    Point p;
    arm_Update();
    p.x = cursor.x;
    p.y = cursor.y+8;

    arm_toPoint(p);
}

void arm_Down()
{
    Point p;
    arm_Update();
    p.x = cursor.x;
    p.y = cursor.y-8;

    arm_toPoint(p);
}