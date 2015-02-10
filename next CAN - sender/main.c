#include "../header_and_config/LUR7.h"
#include "can_lib.h"

st_cmd_t send_cmd;
uint8_t send_data;

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
	// CAN
	CLKPR = (1<<CLKPCE); //Enable clock prescaler
	CLKPR = 0x00; //set prescaler to 1

	can_init(); //init the CAN bus
	CANGIE = (1<<ENIT) | (1<<ENTX); //enable TX interrupts

	CANIE2 = (1<<IEMOB5) | (1<<IEMOB4) | (1<<IEMOB3) | (1<<IEMOB2) | (1<<IEMOB1);

	send_data = 0xFF; // data to send

	//set up frame to send
	send_cmd.pt_data = &send_data;
	send_cmd.dlc = 1;
	send_cmd.id.ext = 0x80;
	send_cmd.status = 0x00;
	send_cmd.cmd = CMD_TX_DATA;

	//LUR7
	init_io();
	pc_int_on(IN6);
	set_output(OUT1, LOW);
	interrupts_on();
}

inline void loop() {
	;
}

void pcISR_in1(void) {}
void pcISR_in2(void) {}
void pcISR_in3(void) {}
void pcISR_in4(void) {}
void pcISR_in5(void) {}
void pcISR_in6(void) {
	if (get_input(IN6)) {
		set_output(OUT1, HIGH);
		can_cmd(&send_cmd);
	}
}
void pcISR_in7(void) {}
void pcISR_in8(void) {}
void pcISR_in9(void) {}

ISR(CAN_INT_vect) {
	if (CANSTMOB & 1<<TXOK) {
		
		set_output(OUT1, LOW);
		can_get_status(&send_cmd);
	} else {
		can_get_status(&send_cmd);
	}
}
