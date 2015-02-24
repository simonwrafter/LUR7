#include "../header_and_config/LUR7.h"

uint8_t er_flag = 0;
uint8_t txok_flag = 1;

int main(void) {
	init_io();
	set_output(OUT1, ON);
	set_output(OUT2, OFF);
	can_init();

	interrupts_on();
	can_enable();

	while(1) {
		if (er_flag != 0) {
			set_output(OUT2, ON);
		}
		if (txok_flag == 1) {
			set_output(OUT1, OFF);
			can_setup_tx(0x000f0f00, '\0', 0);
			txok_flag = 0;
		}
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
	set_output(OUT1, ON);
	txok_flag = 1;
}

void CAN_ISR_OTHER() {
	CANGIT = 0x00;
	er_flag = 1;
}
