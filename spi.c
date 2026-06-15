//
#define F_CPU 4915200UL
#define BAUD 9600
#define MYUBRR (((F_CPU/(16UL * BAUD))-1))
//
#include "spi.h"
#include <avr/pgmspace.h>
#include <stdint.h>
#include "mcp2515.h"
#include <avr/delay.h>



	
#define DDR_SPI DDRB
#define DD_SS PB4
#define DD_MOSI PB5
#define DD_MISO PB6
#define DD_SCK PB7
	
	
	
void spi_set_ss() {
		PORTB |= (1 << DD_SS);
}

void spi_clear_ss() {
		PORTB &= ~(1 << DD_SS);
}
	
	
void spi_master_init() {	
    DDRB |= (1 << PB4) | (1 << PB5) | (1 << PB7);
    DDRB &= ~(1 << PB1) & ~(1 << PB2) & ~(1 << PB3) & ~(1 << PB6);
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	
	spi_set_ss();
}



void spi_write(char cData) {
	//spi_clear_ss();
	SPDR = cData;
	while(!(SPSR & (1 << SPIF)));
//	spi_set_ss();
//	_delay_ms(1);
//	printf("\nSENDING signal %x to MCP\n\r", cData);
}
 
uint8_t spi_read() {
//	printf("\nStart SPI read\n");
//	spi_clear_ss();
    spi_write(0xff); //dummy byte
//	spi_set_ss();
    return SPDR;
	
//	printf("\nEnd SPI read\n");
}

/*


*/
