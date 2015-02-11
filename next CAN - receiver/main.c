#include "../header_and_config/LUR7.h"
#include "can_lib.h"

st_cmd_t received_cmd;
uint8_t rec_data;

void loop(void);
void setup(void);

int main(void) {
	setup();
	while(1) {
		loop();
	}
	return(0);
}

inline void setup() {
	//CAN
	CLKPR = (1<<CLKPCE); //Enable clock prescaler
	CLKPR = 0x00; //set prescaler to 1

	can_init(); //init the CAN bus
	CANGIE = (1<<ENIT) | (1<<ENRX); //enable RX interrupts
	CANIE2 = (1<<IEMOB5) | (1<<IEMOB4) | (1<<IEMOB3) | (1<<IEMOB2) | (1<<IEMOB1);

	//set up receiver
	received_cmd.pt_data = &rec_data;
	received_cmd.dlc = 1;
	received_cmd.id.ext = 0x80;
	received_cmd.status = 0x00;
	received_cmd.cmd = CMD_RX_DATA;

	//LUR7
	init_io();
	interrupts_on();

	can_cmd(&received_cmd);
}

inline void loop() {
	;
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

ISR(CAN_INT_vect) {
	if (CANSTMOB & 1<<RXOK) {
		can_get_status(&received_cmd);
		toggle_output(OUT1);
	} else {
		can_get_status(&received_cmd);
	}
	can_cmd(&received_cmd);
}
