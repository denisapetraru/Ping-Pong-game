#include "fonts.h"
#include "oled.h"
#include <avr/pgmspace.h>

volatile unsigned char* oled_data = (volatile unsigned char*) 0x1200; 
volatile unsigned char* oled_command = (volatile unsigned char*) 0x1000; 

void init_program()
{
	sendCommand(0xae); // display off
	sendCommand(0xa1); //segment remap
	sendCommand(0xda); //common pads hardware: alternative
	sendCommand(0x12);
	sendCommand(0xc8); //common output scan direction:com63~com0
	sendCommand(0xa8); //multiplex ration mode:63
	sendCommand(0x3f);
	sendCommand(0xd5); //display divide ratio/osc. freq. mode
	sendCommand(0x80);
	sendCommand(0x81); //contrast control
	sendCommand(0x50);
	sendCommand(0xd9); //set pre-charge period
	sendCommand(0x21);
	sendCommand(0x20); //Set Memory Addressing Mode
	sendCommand(0x02);
	sendCommand(0xdb); //VCOM deselect level mode
	sendCommand(0x30);
	sendCommand(0xad); //master configuration
	sendCommand(0x00);
	sendCommand(0xa4); //out follows RAM content
	sendCommand(0xa6); //set normal display
	sendCommand(0xaf); // display on
	
}

void sendCommand(char command) {
	
	oled_command[0x00] = (char)command;
	
}

void sendData(char data) {
	
	oled_data[0x00] = (data);
	
}

void setColumn(unsigned char column) {
	sendCommand(0x00  | (column & 0x0F));  // Set Column Address command
	sendCommand(0x10 | (column >> 4)); // Start column (0 to 127)
	//sendCommand(end);   // End column (0 to 127)
}

void setPage(unsigned char pg) {
	sendCommand(0xB0 | (pg & 0b111));
}

void clearDisplay(){
	
	for(int i=0; i<=8; i++)
	{
		setPage(i);
		for(int j=0; j<=127; j++)
		{
			setColumn(j);
			sendData(0);
		}
		
	}
}

void displayChar(char c) {
	// Get the 8x8 bitmap for the character
	int a = (int)(c - 32);
	//const unsigned char* bitmap = font8[a];  // Subtract 32 to get index

	for (int i = 0; i < 8; i++) {
		sendData(pgm_read_word(&(font8[a][i])));  // Function to send data to GDDRAM
	}
}

void displayString(const char* str) {
	while (*str) {
		displayChar(*str);  // Display each character
		str++;
		// Move the cursor for the next character, typically 8 pixels forward
	}
}