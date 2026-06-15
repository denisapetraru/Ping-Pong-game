/*
 * MCP2515_Driver.c
 *
 * Created: 10/17/2024 9:01:33 PM
 *  Author: AptDamp
 */ 
#define F_CPU 4915200UL
#include "MCP2515_Driver.h"
#include "spi.h"
#include <stdio.h>
#include <avr/delay.h>


void mcp_init() {
	uint8_t val;
	spi_master_init(); //initialize SPI
	mcp_reset();
	_delay_ms(3); //somebody had it so idk, not in the slides idk:/

	val = mcp_read(MCP_CANSTAT); //it was pass by reference in the slides://
	if ((val & MODE_MASK) != MODE_CONFIG) {
		printf("\n----- MCP2515 is NOT in configuration mode after reset!!!\r\n", val, MCP_CANSTAT);
		//printf("reading val=%x from address MCP_CANSTAT=%x BUT\n\r", val, MCP_CANSTAT);
		//sprintf("\nMODE_MASK & value != MODE_CONFIG => %x & %x != %x\n\r", MODE_MASK, val, MODE_CONFIG);
	}
	else {
		//printf("\n:')\n\r");
	}
}

void mcp_reset() {
	//printf("\nSTARTING RESET TO MCP\n\r");
	spi_clear_ss(); // MCP select with CS low
	spi_write(MCP_RESET); // reset 1100 0000 = 0xC0 (SPI instr)
	spi_set_ss(); // rise back CS => deselect
}

uint8_t mcp_read(uint8_t address) {
	uint8_t res;
	spi_clear_ss(); //select MCP with CS aka SS
	spi_write(MCP_READ); //read instruction
	spi_write(address); //address
	res = spi_read(); //
	spi_set_ss(); //deselect MCP
	return res;
}

void mcp_write(uint8_t address, uint8_t data) {
	spi_clear_ss();
	spi_write(MCP_WRITE);
	spi_write(address);
	spi_write(data);
	spi_set_ss();
}


//---------------------------------------------------------------//

char mcp_read_status() {
	spi_clear_ss();
	spi_write(MCP_READ_STATUS);
	char data = spi_read();
//	spi_write(0xff);//might add
	spi_set_ss();

	return data;
}

void mcp_bit_modify(uint8_t address, uint8_t mask, uint8_t data) {
	spi_clear_ss();
	spi_write(MCP_BITMOD);
	spi_write(address);
	spi_write(mask);
	spi_write(data);
	spi_set_ss();
}

void mcp_set_mode(uint8_t mode) {
	mcp_bit_modify(MCP_CANCTRL, 0b11100000, mode);
}

void mcp_request_to_send(int buffer_number) {
	spi_clear_ss();
	buffer_number = buffer_number % 3; //we have 3 uffers 0 , 1, 2
	char data = MCP_RTS_TX0;
	if (buffer_number == 0) {
		data = MCP_RTS_TX0;
		} else if (buffer_number == 1) {
		data = MCP_RTS_TX1;
		} else if (buffer_number == 2) {
		data = MCP_RTS_TX2;
	}
	spi_write(data);
	spi_set_ss();
}

void mcp_request_to_send_buffer0() {
	spi_clear_ss();
	char data = MCP_RTS_TX0;
	spi_write(data);
	spi_set_ss();
}

typedef struct Message {
	unsigned int id;
	uint8_t length;
	char data[8];
} message_t, *message_ptr;
