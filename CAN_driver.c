/*
 * CAN_driver.c
 *
 * Created: 10/19/2024 11:21:46 PM
 *  Author: AptDamp
 */ 
#include "spi.h"
#include <stdint.h>
#include "mcp2515.h"
#include "MCP2515_Driver.h"
#include "mcp2515.h"
#include "CAN_driver.h"

void can_init() {
	mcp_init();//basically the same
}


void CAN_send(dataFrame datafrm) {

	mcp_write(0x31, datafrm.id / 8); //retrieveing 8 bits fro high
	mcp_write(0x32, (datafrm.id % 8) << 5); //retrieveing 3 bits for low

	mcp_write(0x35, datafrm.len); //data length

	for (int i = 0; i < datafrm.len; i++) {
		mcp_write(0x36 + i, datafrm.data[i]);
	}
	mcp_request_to_send(0);
}

dataFrame CAN_receive() {
	dataFrame data = {};

	uint8_t id_low = mcp_read(0x62)/0b100000;
	uint8_t id_high = mcp_read(0x61);
	data.id = id_high * 0b1000 + id_low;

	data.len = mcp_read(0x65);

	for (int i = 0; i < data.len; i++) {
		data.data[i] = mcp_read(0x66 + i);
	}

	return data;
}

void CAN_time_init() {
	mcp_reset(); //go to config mode
	_delay_ms(3);
	
	mcp_write(MCP_CNF1, 0xc3); //config CnF1
	mcp_write(MCP_CNF2, 0xa9); //config CnF2
	mcp_write(MCP_CNF3, 0x06); //config CnF3
	
	/*
	uint8_t val = mcp_read(MCP_CNF1); 
	printf("\ncnf1 is %x\r\n", val);
	val = mcp_read(MCP_CNF2);
	printf("\ncnf2 is %x\r\n", val);
	val = mcp_read(MCP_CNF3); 
	printf("\ncnf3 is %x\r\n", val);
	
	val = mcp_read(MCP_CANCTRL);
	printf("\ncnf3 is %x\r\n", val);
	*/
}