#include "../header_and_config/LUR7.h"

uint8_t string_test(uint8_t *, uint8_t *, uint8_t);

int main(void) {
	io_init();

	can_init();
	can_setup_rx(0x0f0f0f00, 0xffffff00, 0);

	interrupts_on();
	can_enable();

	set_output(OUT1, OFF);

	while(1) {
	}
	return(0);
}
/*
uint8_t string_test(uint8_t * s1, uint8_t * s2, uint8_t nbr_to_compare) {
	for (uint8_t pos = 0; pos < nbr_to_compare; pos++) {
		if (s1[pos] != s2[pos]) {
			return FALSE;
		} else if (s1[pos] == '\0') {
			return TRUE;
		}
	}
	return TRUE;
}*/

void pcISR_in1(void) {}
void pcISR_in2(void) {}
void pcISR_in3(void) {}
void pcISR_in4(void) {}
void pcISR_in5(void) {}
void pcISR_in6(void) {}
void pcISR_in7(void) {}
void pcISR_in8(void) {}
void pcISR_in9(void) {}

void CAN_ISR_RXOK(uint32_t id, uint8_t dlc, uint8_t * data) {
	toggle_output(OUT1);
}

void CAN_ISR_TXOK(uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER() {}


void timer1_isr_100Hz(uint8_t interrupt_nbr) {}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
