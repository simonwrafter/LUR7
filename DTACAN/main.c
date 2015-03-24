#include "../header_and_config/LUR7.h"

uint16_t tps = 0;
//uint8_t new = 0;

int main(void) {
	io_init();

	can_init();
	can_setup_rx(0x00002000, 0xffffffff, 8);

	interrupts_on();
	can_enable();

	set_output(OUT2, OFF);

	while(1) {
//		if (new) {
			;
//		}
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

void CAN_ISR_RXOK(uint32_t id, uint8_t dlc, uint8_t * data) {
	toggle_output(OUT2);
	tps = (uint16_t) (data[7] << 8) | data[8];
}

void CAN_ISR_TXOK(uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER() {}


void timer1_isr_100Hz(uint8_t interrupt_nbr) {}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
