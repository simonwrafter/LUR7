#ifndef _SPI_ROUTINES_H_
#define _SPI_ROUTINES_H_

void SPI_init(void);
uint8_t SPI_transmit(uint8_t);
uint8_t SPI_receive(void);

void SPI_select(void);
void SPI_deselect(void);

void SPI_high_speed(void);

#endif
