#include "config.h"
#include "can_lib.h"
#include <util/delay.h>

st_cmd_t send_cmd;
st_cmd_t received_cmd;
uint8_t send_data;
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
	CLKPR = (1<<CLKPCE); //Enable clock prescaler
	CLKPR = 0x00; //set prescaler to 1
	
	DDRB = (1<<DDB3) | (1<<DDB4); //PB3 = pin23, PB4 = pin24
	//DDRC = 1<<DDC2;
	
	PCICR = 1<<PCIE0; // set pin change interrupt enable for pcint0:7 in Pin Change Interrupt Control Register
	PCMSK0 = 1<<PCINT2; // set pin change interrupt active for PCINT2 (PB2, pin16) in Pin Change Mask Register 0
	PORTB = 1<<PORTB2; //pull up resistor on PB2
	
	can_init(); //init the CAN bus
	CANGIE = 0xFF; // (1<<ENIT) | (1<<ENTX) | (1<<ENRX); //enable RX and TX interrupts
	
	send_data = 0xFF; // data to send
	
	//set up frame to send
	send_cmd.pt_data = &send_data;
	send_cmd.ctrl.ide = 0;
	send_cmd.dlc = 1;
	send_cmd.id.std = 0x81;
	send_cmd.cmd = CMD_TX_DATA;
	
	//set up receiver
	received_cmd.pt_data = &rec_data;
	received_cmd.ctrl.ide = 0;
	received_cmd.dlc = 1;
	received_cmd.id.std = 0x80;
	received_cmd.cmd = CMD_RX_DATA_MASKED;
	
	sei();
}

inline void loop() {
	;
}

ISR(PCINT0_vect) {
	if (PINB & 1<<PINB2) {
		PINB = 1<<PINB3;
		can_cmd(&send_cmd);
	}
}

ISR(CAN_INT_vect) {
	if (CANSTMOB & 1<<TXOK) {
		PINB = 1<<PINB3; //toggle light to indicate TXOK
		can_get_status(&send_cmd);
	} else if (CANSTMOB & 1<<RXOK) {
		can_get_status(&received_cmd);
		PINB = (rec_data ? 1 : 0)<<PORTB4;
	} else {
		//while (1) {
			PINB = 1<<PINB3;
			_delay_ms(1000);
			can_get_status(&send_cmd);
			can_get_status(&received_cmd);
		//}
	}
}
