/*
 * IncFile1.h
 *
 * Created: 10/3/2024 2:42:55 PM
 *  Author: AptDamp
 */ 


#ifndef OLED_H
#define OLED_H

//extern volatile unsigned char* oled_data;
//extern volatile unsigned char* oled_command;

void init_program();
void sendCommand(char command);
void sendData(char data);
void setColumn(unsigned char column) ;
void setPage(unsigned char pg);
void displayChar(char c);
void displayString(const char* str);

#endif /* OLED_H_ */