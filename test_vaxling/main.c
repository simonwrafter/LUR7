
#include "../header_and_config/LUR7.h"
int up = 0;
int down = 0;
void setup(void);

int main(void){
	setup();

	while(1){
	//Up-shift procedure
	if(up!=0){
		up = 0; //Reset flag
		set_output(OUT6, GND); //Send shift-cut signal to DTA pin14. Output=1 will ground signal
		set_output(OUT4, GND); //Drive solenoid relay, perform up-shift
		_delay_ms(150); //Drive solenoid for specified amount of time
		set_output(OUT4, TRI); //Turn the solenoid off
		set_output(OUT6, TRI); //Disable shift-cut on DTA.
		}
	//Down-shift procedure
	else if(down!=0){
		down = 0; // Reset flag
		//Behöver vi kolla frikoppling?
		set_output(OUT5, GND); //Drive solenoid relay, perform down-shift
		_delay_ms(150); //Drive solenoid for specified amount of time
		set_output(OUT5, TRI); //Turn the solenoid off
		}
	}
	return 0;
}

void setup(void){
	interrupts_off();
	io_init();
	ext_int_on(IN9,1,1); //External interrupt on IN9, trigger on rising edge. Up-shift
	ext_int_on(IN8,1,1); //External interrupt on IN8, trigger on rising edge. Down-shift
	set_output(OUT4, TRI);
	set_output(OUT5, TRI);
	set_output(OUT6, TRI);
	interrupts_on();
	}

//Shift-up procedure
ISR(INT_IN9_vect){
	up = 1; //Set flag
	}

//Shift-down procedure
ISR(INT_IN8_vect){
	down = 1; //Set flag
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

void timer1_isr_100Hz(uint8_t interrupt_nbr) {}

void CAN_ISR_RXOK(uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_TXOK(uint32_t id, uint8_t dlc, uint8_t * data) {}
void CAN_ISR_OTHER(void) {}

void early_bod_warning_ISR(void) {}
void early_bod_safe_ISR(void) {}
