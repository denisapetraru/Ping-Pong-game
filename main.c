/****************
 * GROUP #45, PLACE #22
 * USED HARDWARE :
 * ATMEGA162,
 * MAX233,
 * MC7805 Voltage Regulator
 * ************ */
#define F_CPU 4915200UL
#define BAUD 9600
#define MYUBRR (((F_CPU/(16UL * BAUD))-1))

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include "fonts.h"
#include "oled.h"
#include "User_Interface.h"
#include "spi.h"
#include <stdint.h>
#include "mcp2515.h"
#include "MCP2515_Driver.h"
#include "mcp2515.h"
#include "CAN_driver.h"



#define MCP_TXB0SIDH 0x31 //0b0011 0001
#define MCP_TXB0SIDL 0x32 //0b0011 0010
#define MCP_TXB0EID8 0x33
#define MCP_TXB0EID0 0x34
#define MCP_TXB0DLC 0x35 //0b0011 0101
#define MCP_TXB0D0 0x36 //0b0011 0110
#define MCP_TXB0D1 0x37
#define MCP_TXB0D2 0x38
#define MCP_TXB0D3 0x39
#define MCP_TXB0D4 0x3A
#define MCP_TXB0D5 0x3B
#define MCP_TXB0D6 0x3C
#define MCP_TXB0D7 0x3D




volatile unsigned char received = 0;
volatile uint8_t data_received_flag = 0; //for uart
volatile unsigned char* extract_adc_data = (volatile unsigned char*) 0x1400; //for ADC
volatile unsigned int counter = 0; //for diplsaying x in the manu
//volatile unsigned char* oled_data = (volatile unsigned char*) 0x1200; //OLED data address 
//volatile unsigned char* oled_command = (volatile unsigned char*) 0x1000; //OLED command address 
volatile uint8_t button_pressed = 0;

void Port_init() {
	DDRA = 0xFF; //setting port A as output
	DDRE |= (1<<PE1); //enabling pe1 ALE (latch enable) i/o
	//DDRB |= (1<<PB0) | (1<<PB1); //using pb0 and pb1 for the NAND (MUX) select NOT DOING IT ANYMORE
}

void External_MEM_Init() {
	MCUCR |= (1<<SRE); //enable the external memory interface
	SFIOR |= (1<<XMM2); // External Memory is Enabled, Released Port Pins PC7-PC3
}

void UART_Init(unsigned int ubrr) {
	UBRR0H = (unsigned char)(ubrr >> 8); //9600
	UBRR0L = (unsigned char)ubrr; 

	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); //enabling rx tx and rxc which enables interrupt on rxc flag
	UCSR0C = (1 << URSEL0) | (0 << USBS0) | (3 << UCSZ00); //8data bits and 1 stop bit

    sei(); //enbale interrupts
}
/*
void PWM_init () {
	DDRD |= (1 << PD4); //using pd5 as pwm clock output
	// Set up Fast PWM mode with ICR1 as TOP, non-inverting mode on OC1A (PD5)
	TCCR1A |= (1 << COM1A1);          // Non-inverting mode on OC1A
	TCCR1A &= ~(1 << COM1A0);         // Clear COM1A0 to complete non-inverting mode

	TCCR1B |= (1 << WGM12) | (1 << WGM13);  // Fast PWM mode with ICR1 as TOP
	TCCR1A |= (1 << WGM11);           // WGM bits for Fast PWM
	TCCR1A &= ~(1 << WGM10);          // Clear WGM10

	TCCR1B |= (1 << CS11);            // Prescaler = 8
	TCCR1B &= ~(1 << CS10) & ~(1 << CS12);  // Clear CS10 and CS12

	ICR1 = 100;    // Set TOP value to control frequency
	OCR1A = 50;    // Set duty cycle (50% in this case)
}*/

void PWM_init () {
	DDRD |= (1 << PD4);//using pd5 as pwm clock output
	// Set up Fast PWM mode with ICR1 as TOP, non-inverting mode on OC1A (PD5)
	TCCR3A |= (1 << COM3A1);          // Non-inverting mode on OC1A
	TCCR3A &= ~(1 << COM3A0);         // Clear COM1A0 to complete non-inverting mode

	TCCR3B |= (1 << WGM32) | (1 << WGM33);  // Fast PWM mode with ICR1 as TOP
	TCCR3A |= (1 << WGM31);           // WGM bits for Fast PWM
	TCCR3A &= ~(1 << WGM30);          // Clear WGM10

	TCCR3B |= (1 << CS30);            // Prescaler = 8
	TCCR3B &= ~(1 << CS31) & ~(1 << CS32);  // Clear CS10 and CS12

	ICR3 = 2;    // Set TOP value to control frequency
	OCR3A = ICR3/2;    // Set duty cycle (50% in this case)

}

void UART_Transmit(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0)));
	
	UDR0 = data;
}

/* //polling method to receive data
unsigned char UART_Receive(void) {

	while (!(UCSR0A & (1 << RXC0)));
	

	return UDR0;
}
*/

//defining Interrupt routine
ISR(USART0_RXC_vect) {
	received = UDR0; //this routine is invoked when RXC is set to 1, meaning buffer is full and data was received, then we can read form the UDR. In order to do this we set RXCIE0 so the interrupt for the RCX is active 
	data_received_flag = 1;
}

//defining putchar for fdevopen()
int UART_putchar(char data, FILE *stream){
	UART_Transmit(data);
	return 0;
}

int UART_getchar(FILE *stream) {
	while (!data_received_flag);  
	data_received_flag = 0; 
	return received;  
} 

//setting up the stream with macro
FILE uart_output = FDEV_SETUP_STREAM(UART_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, UART_getchar, _FDEV_SETUP_READ);



//memory testing sample code for lab#2:
void SRAM_test(void)
    {
	    volatile char *ext_ram = (char *) 0x1800; // Start address for the SRAM
	    uint16_t ext_ram_size = 0x800;
	    uint16_t write_errors = 0;
	    uint16_t retrieval_errors = 0;
	    printf("Starting SRAM test...\n");
	    // rand() stores some internal state, so calling this function in a loop will
	    // yield different seeds each time (unless srand() is called before this function)
	    uint16_t seed = rand();
	    // Write phase: Immediately check that the correct value was stored
	    srand(seed);
	    for (uint16_t i = 0; i < ext_ram_size; i++) {
		    uint8_t some_value = rand();
		    ext_ram[i] = some_value;
		    uint8_t retreived_value = ext_ram[i];
		    if (retreived_value != some_value) {
			    printf("Write phase error: ext_ram[%4d] = %02X (should be %02X)\n", i, retreived_value, some_value);
			    write_errors++;
		    }
	    }
	    // Retrieval phase: Check that no values were changed during or after the write phase
	    srand(seed);
	    // reset the PRNG to the state it had before the write phase
	    for (uint16_t i = 0; i < ext_ram_size; i++) {
		    uint8_t some_value = rand();
		    uint8_t retreived_value = ext_ram[i];
		    if (retreived_value != some_value) {
			    printf("Retrieval phase error: ext_ram[%4d] = %02X (should be %02X)\n", i, retreived_value, some_value);
			    retrieval_errors++;
		    }
	    }
	    printf("SRAM test completed with \n%4d errors in write phase and \n%4d errors in retrieval phase\n\n", write_errors, retrieval_errors);
}

void ADC_init()
{
	char data = 0x00;
	// write to the adc
	extract_adc_data[0x00] = data; //global volatile
}




void buttons_init(void)
{
	//setting the pins as input
	DDRD &= ~(1 << PD2) & ~(1 << PD3);
	PORTD |= (1 << PD2) | (1 << PD3);
	
	DDRE &= ~(1 << PE0);
	PORTE |= (1 << PE0);
	
	//setting interrupts on falling edge
	MCUCR |= (1 << ISC00) | (1 << ISC01) | (1 << ISC10) | (1 << ISC11);
	EMCUCR |= (1 << ISC2);
	//MCUCR &= ~(1 << ISC00) & ~(1 << ISC10);
	
	//enable external interrupts
	GICR |= (1 << INT0) | (1 << INT1) | ( 1<< 5); //INT2 is 5
}

int sliderPosition(int val) {
	return val*100/255;
}

int joystickPosition(int val) {
	if (val < 127) {
		return ((val-128)*100/127);
	}
	return ((val-127)*100)/127;
}


void ADC_read(){
	//setting mux to choose ADC 10=>ADC
//	PORTB |= (1<<PB0);
	//PORTB &= ~(1<<PD1);
	unsigned char adc_value;
	
	_delay_ms(20);
	extract_adc_data[0x00] = 0x80;
	_delay_ms(20);
	adc_value = extract_adc_data[0x00];
	printf("LEFT slider: %d percent\n", sliderPosition(adc_value));
	
	_delay_ms(20);
	extract_adc_data[0x00] = 0x81;
	_delay_ms(20);
	adc_value = extract_adc_data[0x00];
	printf("RIGHT slider: %d percent\n", sliderPosition(adc_value));
	
	
	_delay_ms(20);
	extract_adc_data[0x00] = 0x82;
	_delay_ms(20);
	adc_value = extract_adc_data[0x00];
	printf("JOYSTICK X: %d percent\n", joystickPosition(adc_value));
	
	
	_delay_ms(20);
	extract_adc_data[0x00] = 0x83;
	_delay_ms(20);
	adc_value = extract_adc_data[0x00];
	printf("JOYSTICK Y: %d percent\n\n\n", joystickPosition(adc_value));
	
}

char joystick_Y_read()
{
	unsigned char adc_value;
	_delay_ms(20);
	extract_adc_data[0x00] = 0x83;
	_delay_ms(20);
	adc_value = extract_adc_data[0x00];
	return (adc_value);
}

char joystick_X_read()
{
	unsigned char adc_value;
	_delay_ms(20);
	extract_adc_data[0x00] = 0x82;
	_delay_ms(20);
	adc_value = extract_adc_data[0x00];
	return (adc_value);
}

ISR(INT0_vect)
{
	printf("LEFT button pressed\n");
	button_pressed = 1;
}

ISR(INT1_vect)
{
	printf("RIGHT button pressed\n");
}

ISR(INT2_vect)
{
	printf("JOYSTICK button pressed\n");
		displayMenu();
		//display x based on counter
		setPage(counter + 1);
		setColumn(120);
		displayChar('S'); //"string"   'char'
			//we dont know which interrupt to use for the SPI so
			//we will be for a while disconnecting this one which
			//is a grey wire coming from the joystick mfb 
}









void moveUpDwon(){
	//switching to ADC 10
//	PORTB |= (1<<PB0);
	//PORTB &= ~(1<<PD1);
	unsigned char adc_value;
	int position;
	unsigned int counterChange = counter;
	
	_delay_ms(20);
	extract_adc_data[0x00] = 0x82;
	_delay_ms(20);
	adc_value = extract_adc_data[0x00];
	position = joystickPosition(adc_value);
	if(position < -90)
	{
					printf("!!!!");
		if (counter > 0) {
			counter=counter-1;
		}
	}
	if (position > 90)
	{
		if (counter < 4){
			counter++;
			printf("counter %d\n", counter);
		}
			
	}
	
	if (counterChange != counter) {
	displayMenu();
	//display x based on counter
	setPage(counter + 1);
	setColumn(120);
	displayChar('x'); //"string"   'char'
	counterChange = counter;
	}
}








int main(void) {
	UART_Init(MYUBRR);
	Port_init();
	External_MEM_Init();
	PWM_init();
	ADC_init();
	buttons_init();
	stdout = &uart_output; //linking printf to uart for lab1
	stdin = &uart_input;  //same but for scanf
	
	//--------OLED----------
	init_program();
	displayMenu();
	
	//--------MCP---------
	/*printf("\nstarting work on the SPI\n");
	mcp_init();
	mcp_set_mode(MODE_LOOPBACK); //set the mode for testing
	printf("Current mode: %x\r\n", mcp_read(MCP_CANSTAT));
	mcp_write(MCP_TXB0SIDH, 0xA1); //write soemthing to buffer 0
	mcp_request_to_send(0);
	
	_delay_ms(10);
	printf("received: %x\r\n", mcp_read(MCP_RXB0SIDH));*/
	
	
	
	
	//-------------CAN------------

	printf("\nstarting work on the CAN\n\r");
	mcp_init();
	CAN_time_init();
	mcp_set_mode(MODE_NORMAL);
	
//	dataFrame dtfrm;
	
//	CAN_send(dtfrm);
	//_delay_ms(100);

/*
	CAN_send(dtfrm);
	_delay_ms(100);

	dataFrame received = CAN_receive();
	printf("Id: %d \r\n", received.id);
	printf("length: %d \r\n", received.len);
	printf("data: %s \r\n\r\n", received.data);
*/
	dataFrame canmsg;
	sei();
    while (1) {
		canmsg = CAN_receive();
		_delay_ms(10);
		printf("Id: %d \r\n", canmsg.id);
		printf("length: %d \r\n", canmsg.len);
		printf("data: %d \r\n\r\n", (uint8_t)canmsg.data);

		
		//-------------ADC------------
		//printf("value: %d \n", joystick_Y_read());
		//printf("\nbutton pressed val tbs:  %d\n\r", button_pressed);
		dataFrame dtfrm = {1,3, joystick_Y_read(), button_pressed, joystick_X_read()};
		CAN_send(dtfrm);
		_delay_ms(100);
		button_pressed = 0;
		moveUpDwon();
	}
}


