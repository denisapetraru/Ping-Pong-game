/*
 * CAN_driver.h
 *
 * Created: 10/19/2024 11:22:02 PM
 * Author: KETEVAN KUTCHAVA
 * Company: NTNU 
 */ 


#ifndef CAN_DRIVER_H_
#define CAN_DRIVER_H_

#include "spi.h"
#include <stdint.h>
#include "mcp2515.h"
#include "MCP2515_Driver.h"
#include "mcp2515.h"
#include "CAN_driver.h"

typedef struct dataFrame {
	unsigned int id;
	uint8_t len;
	char data[8];
}dataFrame;

void can_init();
void CAN_send(dataFrame datafrm);
dataFrame CAN_receive();
void CAN_time_init();

#endif /* CAN_DRIVER_H_ */