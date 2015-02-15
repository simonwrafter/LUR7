#include "../header_and_config/LUR7.h"
#include "can_lib.h"

uint8_t flag = 0;

int main(void) {
	init_io();
	set_output(OUT1, OFF);
	can_init();

	interrupts_on();
	can_enable();

	while(1) {
		if (flag == 1) {
			//set_output(OUT1, ON);
		}
		toggle_output(OUT2);
		can_setup_tx(0x000f0f00, '\0', 0);
		_delay_ms(1000);
	}
	return(0);
}


void pcISR_in1(void) {}
void pcISR_in2(void) {}
void pcISR_in3(void) {}
void pcISR_in4(void) {}
void pcISR_in5(void) {}
void pcISR_in6(void) {}
void pcISR_in7(void) {}
void pcISR_in8(void) {}
void pcISR_in9(void) {}

void CAN_ISR_RXOK(uint32_t id, uint8_t dlc, uint8_t * data) {}

void CAN_ISR_TXOK(uint32_t id, uint8_t dlc, uint8_t * data) {
	toggle_output(OUT1);
}

void CAN_ISR_OTHER() {
	CANGIT = 0x00;
	flag = 1;
}
