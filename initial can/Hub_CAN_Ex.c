/*
 * Hub_CAN_Ex.c
 *
 * Created: 10/21/2013 4:14:22 PM
 *  Author: Digi-Key Design Support
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "LED.h"
#include "Timer.h"
#include "can_lib.h"

#define ID_TAG_BASE 0x80
#define NB_TARGET   2 // 2 Sensor Nodes
#define DATA_BUFFER_SIZE 2 // Up to 8 bytes Max
#define BUTTON_0 7 // PortC, pin 7
#define BUTTON_PRESS (!(PINC & (1 << BUTTON_0)))

volatile uint8_t CTC_flag;

void sys_init(void);
void io_init(void);
uint8_t Read_Button (void);

int main(void)
{
	uint8_t i;
	uint8_t j;
	
	// Initialize I/O, Timer, and CAN peripheral
	sys_init();
	
	// Enable global interrupts for Timer execution
	sei();
	
	// Flash LEDs to indicate program startup
	LED_Reg_Write(0xAA); 
	delay_ms(50);
	LED_Reg_Write(0x00);
	
	uint8_t tx_remote_buffer[DATA_BUFFER_SIZE];
	st_cmd_t tx_remote_msg; // Remote Tx MOb (Requests remote data from Sensor Nodes)
	
	uint8_t response_buffer[DATA_BUFFER_SIZE]; // buffer to hold payload from Sensor Nodes
	st_cmd_t response_msg; // Response Mob
	
	tx_remote_msg.pt_data = &tx_remote_buffer[0]; // Point Remote Tx MOb to first element of buffer
	tx_remote_msg.status = 0; // clear status
	
	response_msg.pt_data = &response_buffer[0]; // Point Response MOb to first element of buffer
	response_msg.status = 0; // clear status
	
    while(1)
    {	
		// If SW0 is pressed
		if(Read_Button()) { 
			
			// For all Sensor Nodes
			for(j=0; j<NB_TARGET; j++) {
				
				// Configure Response MOb
				for(i=0; i<DATA_BUFFER_SIZE; i++) {response_buffer[i] = 0;} // clear message object data buffer
				response_msg.id.std = ID_TAG_BASE + j; // This message object only accepts frames from Target IDs (0x80) to (0x80 + NB_TARGET)
				response_msg.ctrl.ide = 0; // This message object accepts only standard (2.0A) CAN frames
				response_msg.ctrl.rtr = 0; // this message object is not requesting a remote node to transmit data back
				response_msg.dlc = DATA_BUFFER_SIZE; // Number of bytes (8 max) of data to expect
				response_msg.cmd = CMD_RX_DATA_MASKED; // assign this as a "Receive Standard (2.0A) CAN frame" message object
				
				while(can_cmd(&response_msg) != CAN_CMD_ACCEPTED); // Wait for MOb to configure (Must re-configure MOb for every transaction)
			
				// Configure Remote Tx MOb
				for(i=0; i<DATA_BUFFER_SIZE; i++) {tx_remote_buffer[i]=0;} // clear message object data buffer
				tx_remote_msg.id.std = ID_TAG_BASE + j; // This message object only sends frames to Target IDs (0x80) to (0x80 + NB_TARGET)
				tx_remote_msg.ctrl.ide = 0; // This message object sends standard (2.0A) CAN frames
				tx_remote_msg.ctrl.rtr = 1; // This message object is requesting a remote node to transmit data back
				tx_remote_msg.dlc = DATA_BUFFER_SIZE; // Number of data bytes (8 max) requested from remote node
				tx_remote_msg.cmd = CMD_TX_REMOTE; // assign this as a "Request Standard (2.0A) Remote Data Frame" message object

				while(can_cmd(&tx_remote_msg) != CAN_CMD_ACCEPTED); // Wait for MOb to configure (Must re-configure MOb for every transaction) and send request

				while(can_get_status(&tx_remote_msg) == CAN_STATUS_NOT_COMPLETED); // Wait for Tx to complete
			
				delay_ms(50); // Wait 50ms for a response
				
				// If response is received
				if (can_get_status(&response_msg) == CAN_STATUS_COMPLETED) {
					// Display received data (Target ID and data byte) on LEDs (1 second delay between bytes)
					for(i=0; i<DATA_BUFFER_SIZE; i++) {
						LED_Reg_Write(response_buffer[i]);
						delay_ms(1000);
					}
					LED_Reg_Write(0x00); // Clear LEDs
				}else{
					// If no response is received in 50ms, send abort-frame
					response_msg.cmd = CMD_ABORT;
					while (can_cmd(&response_msg) != CAN_CMD_ACCEPTED);
				}
			}				
		}
    }
}

// Read button with crude debounce
uint8_t Read_Button (void) {
	// button debounce
	if(BUTTON_PRESS) {
		delay_ms(10);
		while(BUTTON_PRESS); // wait for button to be released
		delay_ms(10);
		return 1;
	}else{
		return 0;
	}
}

void sys_init(void) {
	// Make sure sys clock is at least 8MHz
	CLKPR = 0x80;
	CLKPR = 0x00;
	
	io_init();
	timer_init();
	can_init(0);
}

void io_init(void) {
	
	// Init PORTB[7:0] // LED port
	DDRB = 0xFF;
	LED_Reg_Write(0x00); // clear all LEDs
	
	// Init PORTC[7:0] // PORTC[3:2] => RXCAN:TXCAN
	DDRC = 0x00;
	PORTC = 0x80; // activate pull-up on PC[7] (button)
	
	// Init PORTD[7:0] // not used
	DDRD = 0x00;
	PORTD = 0x00;
	
	// Init PORTE[2:0] // not used
	DDRE = 0x00;
	PORTD = 0x00;	
}