#include <avr/io.h>
#include "nokia5110.h"
#include "lcd.h"
#include <string.h>

void lcd_newLine(int scale, int old_y)
{
    curr_y= old_y +7*scale +2; 
    nokia_lcd_set_cursor(0, curr_y);
        
}


void lcd_printCenter(int scale, const char *str)
{
    
    int x= 42-(((strlen(str)*5*scale)+strlen(str)-1)/2);
    int y=24-(7*scale)/2;
    nokia_lcd_set_cursor(x, y);
    nokia_lcd_write_string(str, scale);
}

void lcd_printHeader(const char *str)
{
    
    int x= 42-(((strlen(str)*5*H_SCALE)+strlen(str)-1)/2);
    int y=0;
    nokia_lcd_set_cursor(x, y);
    nokia_lcd_write_string(str, H_SCALE);
}

void lcd_printBack(char *str)
{
    
    nokia_lcd_set_cursor(0, 41);
    nokia_lcd_write_string(str, H_SCALE);
}

void lcd_printNext(char *str)
{
    int x=84-((strlen(str)*5*H_SCALE)+strlen(str));
    nokia_lcd_set_cursor(x, 41);
    nokia_lcd_write_string(str, H_SCALE);

}

void page1()
{
    nokia_lcd_clear();
    lcd_printHeader("SelectMode1/2");
    lcd_printNext("9<Next");
    curr_y=0;
    lcd_newLine(H_SCALE, curr_y);
    nokia_lcd_write_string("1<Manual", H_SCALE);
    lcd_newLine(H_SCALE, curr_y);
    nokia_lcd_write_string("2<Demo1", H_SCALE);
    lcd_newLine(H_SCALE, curr_y);
    nokia_lcd_write_string("3<Demo2", H_SCALE);

    nokia_lcd_render();
}

void page2()
{
    nokia_lcd_clear();
    lcd_printHeader("SelectMode2/2");
    lcd_printBack("0<Back");
    curr_y=0;
    lcd_newLine(H_SCALE, curr_y);
    nokia_lcd_write_string("4<Letras", H_SCALE);

    nokia_lcd_render();
}


void Letras()
{
    nokia_lcd_clear();
    lcd_printHeader("Letras");
    lcd_printBack("0<Back");
    curr_y=0;
    lcd_newLine(H_SCALE, curr_y);
    lcd_newLine(H_SCALE, curr_y);
    nokia_lcd_write_string("A, E, I, O, U", H_SCALE);
    
    nokia_lcd_render();
}

void Drawing(const char *alfa, const char *beta, const char *x, const char *y)
{
    nokia_lcd_clear();
    lcd_printHeader("Drawing");
    lcd_printBack("0<Back");
    curr_y=0;
    lcd_newLine(H_SCALE, curr_y);
    nokia_lcd_write_string("alfa:", H_SCALE);
    nokia_lcd_write_string(alfa, H_SCALE);
    nokia_lcd_set_cursor(55, curr_y);
    nokia_lcd_write_string("x:", H_SCALE);
    nokia_lcd_write_string(x, H_SCALE);
    lcd_newLine(H_SCALE, curr_y);
    lcd_newLine(H_SCALE, curr_y);
    nokia_lcd_write_string("beta:", H_SCALE);
    nokia_lcd_write_string(beta, H_SCALE);
    nokia_lcd_set_cursor(55, curr_y);
    nokia_lcd_write_string("y:", H_SCALE);
    nokia_lcd_write_string(y, H_SCALE);

    nokia_lcd_render();
}

void manualMode()
{
    nokia_lcd_clear();
    lcd_printHeader("ManualMode");
    lcd_printBack("0<Back");
    curr_y=0;
    lcd_newLine(H_SCALE, curr_y);
    nokia_lcd_set_cursor(40,curr_y);
    nokia_lcd_write_string("W", H_SCALE);
    lcd_newLine(H_SCALE, curr_y);
    nokia_lcd_set_cursor(30,curr_y);
    nokia_lcd_write_string("A", H_SCALE);
    nokia_lcd_set_cursor(40,curr_y);
    nokia_lcd_write_string("S", H_SCALE);
    nokia_lcd_set_cursor(50,curr_y);
    nokia_lcd_write_string("D", H_SCALE);
    lcd_newLine(H_SCALE, curr_y);
    nokia_lcd_write_string("R<reset", H_SCALE);
    nokia_lcd_set_cursor(55,curr_y);
    nokia_lcd_write_string("X<ref", H_SCALE);
    
    nokia_lcd_render();
}