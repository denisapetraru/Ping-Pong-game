#include "User_Interface.h"
#include "fonts.h"
#include "oled.h"
#include <avr/pgmspace.h>



	
void displayMenu() {
	//mux 11 => OLED
	//PORTB |= (1<<PB0);
//	PORTB |= (1<<PD1);
	clearDisplay();
	setPage(0);
	setColumn(0);
	displayString("Menu");
	setPage(1);
	setColumn(0);
	displayString("choose 1");
	setPage(2);
	setColumn(0);
	displayString("choose 2");
	setPage(3);
	setColumn(0);
	displayString("choose 3");
	setPage(4);
	setColumn(0);
	displayString("choose 4");
	setPage(5);
	setColumn(0);
	displayString("choose 5");
}


	

	
void buttonClick () {}