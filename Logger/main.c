#define F_CPU 8000000UL		//freq 8 MHz
#define CAN_BAUDRATE 1000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "SPI_routines.h"
#include "SD_routines.h"
#include "FAT32.h"

#include "../header_and_config/LUR7_can.h"


int main(void) {
	
	unsigned char option, error, data, FAT32_active;
	unsigned int i;
	unsigned char fileName[13];
	
	can_init(); // etc.!!!
	
	//**********************
	// Init
	//**********************
	
	for (i=0; i<10; i++) {
		error = SD_init();
		if (!error) {
			break;
		}
	}

	if(error) {
		while(1){
			;  //wait here forever if error in SD init 
		}
	}
	
	SPI_high_speed();
}