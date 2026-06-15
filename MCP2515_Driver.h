/*
 * MCP2515_Driver.h
 *
 * Created: 10/17/2024 9:01:55 PM
 *  Author: AptDamp
 */ 


#ifndef MCP2515_DRIVER_H_
#define MCP2515_DRIVER_H_
#include <stdint.h>
#include "mcp2515.h"
#include <avr/delay.h>


void mcp_init();

void mcp_reset();

uint8_t mcp_read(uint8_t address) ;

void mcp_write(uint8_t address, uint8_t data);

char mcp_read_status();

void mcp_bit_modify(uint8_t address, uint8_t mask, uint8_t data);

void mcp_set_mode(uint8_t mode) ;

void mcp_request_to_send(int buffer_number) ;

void mcp_request_to_send_buffer0();

// typedef struct Message ;


#endif /* MCP2515_DRIVER_H_ */