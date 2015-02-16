#include "../header_and_config/LUR7.h"

uint8_t string_test(uint8_t *, uint8_t *, uint8_t);

int main(void) {
	init_io();

	can_init();
	can_setup_rx(0x000f0f00, 0xffffff00, 8);

	interrupts_on();
	can_enable();

	while(1) {
		;
	}
	return(0);
}

uint8_t string_test(uint8_t * s1, uint8_t * s2, uint8_t nbr_to_compare) {
	for (uint8_t pos = 0; pos < nbr_to_compare; pos++) {
		if (s1[pos] != s2[pos]) {
			return FALSE;
		} else if (s1[pos] == '\0') {
			return TRUE;
		}
	}
	return TRUE;
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

void CAN_ISR_RXOK(uint32_t id, uint8_t dlc, uint8_t * data) {
	if (string_test(data, (uint8_t *) "onononon", 8)) {
		set_output(OUT6, ON);
	} else if (string_test(data, (uint8_t *) "offoffof", 8)) {
		set_output(OUT6, OFF);
	}
}

void CAN_ISR_TXOK(uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER() {}
