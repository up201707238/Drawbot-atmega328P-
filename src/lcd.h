// ################### LIB FOR LCD MENU #######################
#include <avr/io.h>
//#include <ctype.h>

#pragma once

#define H_SCALE 1
#define T_SCALE 1

uint8_t curr_y;

void lcd_newLine(int scale, int old_y);
void lcd_printCenter(int scale, const char *str);
void lcd_printHeader(const char *str);
void lcd_printNext(char *str);
void lcd_printBack(char *str);
void page1();
void page2();
void Letras();
void Drawing(const char *alfa, const char *beta, const char *x, const char *y);
void manualMode();