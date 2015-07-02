#ifndef _SPI_ROUTINES_H_
#define _SPI_ROUTINES_H_

void SPI_init(void);

void SPI_transmit(uint8_t data);
uint8_t SPI_receive(void);

void SPI_high_speed(void);

void SPI_select(void);
void SPI_deselect(void);

#endif
