#include "../header_and_config/LUR7.h"
#include "SPI_routines.h"
#include "SD_routines.h"

static volatile uint8_t CardType = 0;
static uint16_t wait_ready(void);

uint8_t SD_init(void) {
	uint8_t cmd = 0;
	uint8_t ty = 0;
	uint8_t ocr[4];
	
	SPI_init();
	SPI_select();
	
	for (uint8_t n = 10; n; n--) {
		SPI_receive();	/* 80 dummy clocks */
	}
	
	if (SD_sendCommand(CMD0, 0) == 1) {			/* Enter Idle state */
		if (SD_sendCommand(CMD8, 0x1AA) == 1) {	/* SDv2? */
			for (uint8_t n = 0; n < 4; n++) {
				ocr[n] = SPI_receive();		/* Get trailing return value of R7 resp */
			}
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* The card can work at vdd range of 2.7-3.6V */
				for (uint16_t i = 0; i < 1000; i++) {
					if (SD_sendCommand(ACMD41, 1UL << 30) == 0) {
						break;
					}
				} /* Wait for leaving idle state (ACMD41 with HCS bit) */
				
				if (SD_sendCommand(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (uint8_t n = 0; n < 4; n++) {
						ocr[n] = SPI_receive();
					}
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 */
				}
			}
		} else {							/* SDv1 or MMCv3 */
			if (SD_sendCommand(ACMD41, 0) <= 1) 	{
				ty = CT_SD1;
				cmd = ACMD41;	/* SDv1 */
			} else {
				ty = CT_MMC;
				cmd = CMD1;	/* MMCv3 */
			}
			for (uint16_t i = 0; i < 1000; i++) {
				if (SD_sendCommand(cmd, 0) != 0) {	/* Wait for leaving idle state */
					break;
				}
			}
			if (SD_sendCommand(CMD16, 512) != 0) {	/* Set R/W block length to 512 */
				ty = 0;
			}
		}
	}
	
	CardType = ty;
	SPI_deselect();
	
	if (ty) {			/* Initialization succeded */
		SPI_high_speed();
		return 0;
	}
	return 1;
}

/*-----------------------------------------------------------------------*
 * Wait for card ready                                                   *
 *-----------------------------------------------------------------------*/
static uint16_t wait_ready(void) { /* 1:OK, 0:Timeout */
	/* Wait for ready in timeout of 500ms */
	SPI_receive();
	for (uint8_t i = 0; i < 50; i++) {
		if (SPI_receive() == 0xFF) {
			return 1;
		}
		_delay_ms(10);
	}
	return 0;
}

/*-----------------------------------------------------------------------*
 * Deselect the card and release SPI bus                                 *
 *-----------------------------------------------------------------------*/
void SD_deselect(void) {
	SPI_deselect();
	SPI_receive();
}

/*-----------------------------------------------------------------------*
 * Select the card and wait for ready                                    *
 *-----------------------------------------------------------------------*/
uint16_t SD_select(void) {	/* 1:Successful, 0:Timeout */
	SPI_select();
	if (!wait_ready()) {
		SD_deselect();
		return 0;
	}
	return 1;
}

uint8_t SD_sendCommand(uint8_t cmd, uint32_t arg) {
	uint8_t res = 0;
	
	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = SD_sendCommand(CMD55, 0);
		if (res > 1) {
			return res;
		}
	}
	
	/* Select the card and wait for ready */
	SD_deselect();
	if (!SD_select()) {
		return 0xFF;
	}
	
	/* Send command packet */
	SPI_transmit(0x40 | cmd);				/* Start + Command index */
	SPI_transmit((uint8_t) (arg >> 24));	/* Argument[31..24] */
	SPI_transmit((uint8_t) (arg >> 16));	/* Argument[23..16] */
	SPI_transmit((uint8_t) (arg >> 8));		/* Argument[15..8] */
	SPI_transmit((uint8_t) arg);			/* Argument[7..0] */
	uint8_t n = 0x01;								/* Dummy CRC + Stop */
	
	if (cmd == CMD0) {
		n = 0x95;			/* Valid CRC for CMD0(0) */
	}
	if (cmd == CMD8) {
		n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	}
	SPI_transmit(n);
	
	/* Receive command response */
	if (cmd == CMD12) {
		SPI_receive();		/* Skip a stuff byte when stop reading */
	}
	
	n = 10;					/* Wait for a valid response in timeout of 10 attempts */
	
	do {
		res = SPI_receive();
	} while ((res & 0x80) && --n);
	
	return res;				/* Return with the response value */
}

static uint8_t SD_receive (uint8_t *buff, uint16_t btr) {
	uint8_t token;
	
	/* Wait for data packet in timeout of 200ms */
	for (uint8_t i = 0; i < 20; i++) {
		token = SPI_receive();
		if (token != 0xFF) {
			break;
		}
		_delay_ms(10);
	}
	if(token != 0xFE) {
		return 0;		/* If not valid data token, retutn with error */
	}
	
	do {				/* Receive the data block into buffer */
		buff[0] = SPI_receive();
		buff[1] = SPI_receive();
		buff[2] = SPI_receive();
		buff[3] = SPI_receive();
		buff += 4;
	} while (btr -= 4);
	
	SPI_receive();		/* Discard CRC */
	SPI_receive();
	
	return 1;			/* Return with success */
}

uint8_t SD_readSingleBlock(uint8_t *buff, uint32_t sector) {
	if (!(CardType & CT_BLOCK)) {
		sector *= 512;	/* Convert to byte address if needed */
	}
	if (SD_sendCommand(CMD17, sector) == 0) {
		if (SD_receive(buff, 512)) {
			return 1;
		}
	}
	SD_deselect();
	return 0;
}

uint16_t SD_readMultipleBlock (uint8_t *buff, uint32_t sector, uint16_t count) {
	if (!(CardType & CT_BLOCK)){
		sector *= 512;	/* Convert to byte address if needed */
	}
	
	if (SD_sendCommand(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
		do {
			if (!SD_receive(buff, 512)) {
				break;
			}
			buff += 512;
		} while (--count);
		SD_sendCommand(CMD12, 0);				/* STOP_TRANSMISSION */
	}
	SD_deselect();
	return count;
}

uint8_t SD_writeSingleBlock(uint8_t *buff, uint32_t sector) {
	uint8_t response = 0;
	uint16_t retry = 0;

	response = SD_sendCommand(CMD24, sector); //write a Block command

	if (response != 0x00) {
		return response; //check for SD status: 0x00 - OK (No flags set)
	}

	SPI_transmit(0xfe);     //Send start block token 0xfe (0x11111110)

	for (uint16_t i = 0; i < 512; i++) {    //send 512 bytes data
		SPI_transmit(buff[i]);
	}

	SPI_transmit(0xff);     //transmit dummy CRC (16-bit), CRC is ignored here
	SPI_transmit(0xff);
	response = SPI_receive();

	if ((response & 0x1f) != 0x05) { //response= 0xXXX0AAA1 ; AAA='010' - data accepted
		SD_deselect();               //AAA='101'-data rejected due to CRC error
		return response;             //AAA='110'-data rejected due to write error
	}

	while (!SPI_receive()) { //wait for SD card to complete writing and get idle
		if (retry++ > 0xfffe) {
			SD_deselect();
			return 1;
		}
	}

	SD_deselect();
	SPI_transmit(0xff);   //just spend 8 clock cycle delay before reasserting the CS line
	SD_select();         //re-asserting the CS line to verify if card is still busy

	while (!SPI_receive()) { //wait for SD card to complete writing and get idle
		if (retry++ > 0xfffe) {
			SD_deselect();
			return 1;
		}
	}
	SD_deselect();
	return 0;
}

uint8_t SD_writeMultipleBlock(uint8_t *buff, uint32_t sector, uint16_t count) {
	uint8_t response;
	uint16_t retry = 0;
	
	response = SD_sendCommand(CMD25, sector); //write a Block command
	
	if (response != 0x00) {
		return response; //check for SD status: 0x00 - OK (No flags set)
	}
	
	while( count-- ) {
		SPI_transmit(0xfc); //Send start block token 0xfc (0x11111100)
		
		for(uint16_t i = 0; i < 512; i++) { //send 512 bytes data
			SPI_transmit(buff[i]);
		}
		
		SPI_transmit(0xff); //transmit dummy CRC (16-bit), CRC is ignored here
		SPI_transmit(0xff);
		
		response = SPI_receive();
		if( (response & 0x1f) != 0x05) { //response= 0xXXX0AAA1 ; AAA='010' - data accepted
			SD_deselect();               //AAA='101'-data rejected due to CRC error
			return response;             //AAA='110'-data rejected due to write error
		}
		
		while (!SPI_receive()) { //wait for SD card to complete writing and get idle
			if (retry++ > 0xfffe) {
				SD_deselect();
				return 1;
			}
		}
		
		SPI_receive(); //extra 8 bits
	}
	
	SPI_transmit(0xfd); //send 'stop transmission token'
	
	retry = 0;
	
	while (!SPI_receive()) { //wait for SD card to complete writing and get idle
		if (retry++ > 0xfffe) {
			SD_deselect();
			return 1;
		}
	}
	
	SD_deselect();
	SPI_transmit(0xff); //just spend 8 clock cycle delay before reasserting the CS signal
	SD_select(); //re assertion of the CS signal is required to verify if card is still busy
	
	retry = 0;
	
	while(!SPI_receive()) { //wait for SD card to complete writing and get idle
		if(retry++ > 0xfffe) {
			SD_deselect();
			return 1;
		}
	}
	SD_deselect();
	
	return 0;
}

uint8_t SD_sync() {
	if (SD_select()) {
		SD_deselect();
		return 1;
	}
	SD_deselect();
	return 0;
}

uint8_t SD_get_sector_count(void * buff) {
	uint8_t csd[16];
	if ((SD_sendCommand(CMD9, 0) == 0) && SD_receive(csd, 16)) {
		if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
			uint32_t csize = csd[9] + ((uint16_t)csd[8] << 8) + 1;
			*(uint32_t*)buff = (uint32_t)csize << 10;
		} else {					/* SDC ver 1.XX or MMC*/
			uint8_t n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
			uint32_t csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
			*(uint32_t*)buff = (uint32_t)csize << (n - 9);
		}
		return 1;
	}
	return 0;
}

uint8_t SD_get_block_size(void * buff) {
	uint8_t csd[16];
	if (CardType & CT_SD2) {	/* SDv2? */
		if (SD_sendCommand(ACMD13, 0) == 0) {	/* Read SD status */
			SPI_receive();
			if (SD_receive(csd, 16)) {				/* Read partial block */
				for (uint8_t n = 64 - 16; n; n--) {
					SPI_receive();	/* Purge trailing data */
				}
				*(uint32_t*)buff = 16UL << (csd[10] >> 4);
				return 1;
			}
		}
	} else {					/* SDv1 or MMCv3 */
		if ((SD_sendCommand(CMD9, 0) == 0) && SD_receive(csd, 16)) {	/* Read CSD */
			if (CardType & CT_SD1) {	/* SDv1 */
				*(uint32_t*)buff = (((csd[10] & 63) << 1) + ((uint16_t)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
			} else {					/* MMCv3 */
				*(uint32_t*)buff = ((uint16_t)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
			}
			return 1;
		}
	}
	return 0;
}

uint8_t SD_get_cardType() {
	return CardType;
}

uint8_t SD_read_csd(uint8_t * ptr) {
	if (SD_sendCommand(CMD9, 0) == 0 && SD_receive(ptr, 16)) {
		return 1;
	}
	return 0;
}

uint8_t SD_read_cid(uint8_t * ptr) {
	if (SD_sendCommand(CMD10, 0) == 0 && SD_receive(ptr, 16)) {
		return 1;
	}
	return 0;
}

uint8_t SD_read_ocr(uint8_t * ptr) {
	if (SD_sendCommand(CMD58, 0) == 0) {	/* READ_OCR */
		for (uint8_t n = 4; n; n--) {
			*ptr++ = SPI_receive();
		}
		return 1;
	}
	return 0;
}

uint8_t SD_get_status(uint8_t * ptr) {
	if (SD_sendCommand(ACMD13, 0) == 0) {	/* SD_STATUS */
		SPI_receive();
		if (SD_receive(ptr, 64)) {
			return 1;
		}
	}
	return 0;
}