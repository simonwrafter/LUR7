#define F_CPU 8000000UL		//freq 8 MHz
#include <avr/io.h>
//#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SPI_routines.h"
#include "SD_routines.h"
//#include "UART_routines.h"
//#include "RTC_routines.h"
//#include "i2c_routines.h"
#include "FAT32.h"


int main(void) {
	
	unsigned char option, error, data, FAT32_active;
	unsigned int i;
	unsigned char fileName[13];
	
	//**********************
	// Init
	//**********************
	
	//init_devices();
	
	for (i=0; i<10; i++) {
		error = SD_init();
		if (!error) {
			break;
		}
	}

	if(error) {
		if (error == 1) {
			transmitString_F(PSTR("SD card not detected.."));
		}
		if (error == 2) {
			transmitString_F(PSTR("Card Initialization failed.."));
		}
		while(1){
			;  //wait here forever if error in SD init 
		}
	}
	
	SPI_high_speed();
}