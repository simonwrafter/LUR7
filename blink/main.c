#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

int main (void) {
	DDRC = 1 << DDC4;

	while (1) {
		PINC = 1 << PINC4;
		_delay_ms(550);
	}
}
