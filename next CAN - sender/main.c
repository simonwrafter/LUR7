#include "../header_and_config/LUR7.h"

int main(void) {
	io_init();
	set_output(OUT1, ON);
	set_output(OUT2, TRI);
	can_init();

	timer0_init();

	interrupts_on();
	can_enable();

	while(1) {

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
	//toggle_output(OUT1);
}

void CAN_ISR_OTHER() {
}


void timer0_isr_100Hz(uint8_t interrupt_nbr) {
	if (interrupt_nbr % 5 == 0) {
		if (can_setup_tx(0x0f0f0f00, '\0', 0) !=0xFF) {
			toggle_output(OUT1);
		}
	}
}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
