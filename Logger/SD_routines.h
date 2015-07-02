//**************************************************************
// ****** FUNCTIONS FOR SD RAW DATA TRANSFER *******
//**************************************************************
//Controller: ATmega32 (Clock: 8 Mhz-internal)
//Compiler	: AVR-GCC (winAVR with AVRStudio)
//Project V.: Version - 2.4.1
//Author	: CC Dharmani, Chennai (India)
//			  www.dharmanitech.com
//Date		: 24 Apr 2011
//**************************************************************

//Link to the Post: http://www.dharmanitech.com/2009/01/sd-card-interfacing-with-atmega8-fat32.html

#ifndef _SD_ROUTINES_H_
#define _SD_ROUTINES_H_

/* Definitions for MMC/SDC command */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND (MMC) */
#define ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT (MMC) */
#define ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */

/* Card type flags (CardType) */
#define CT_MMC		0x01		/* MMC ver 3 */
#define CT_SD1		0x02		/* SD ver 1 */
#define CT_SD2		0x04		/* SD ver 2 */
#define CT_SDC		(CT_SD1|CT_SD2)	/* SD */
#define CT_BLOCK	0x08		/* Block addressing */

uint8_t SD_init(void);

void SD_deselect(void);
uint16_t SD_select(void);

uint8_t SD_sendCommand(uint8_t cmd, uint32_t arg);

uint8_t SD_readSingleBlock(uint8_t *buff, uint32_t sector);
uint16_t SD_readMultipleBlock (uint8_t *buff, uint32_t sector, uint16_t count);

uint8_t SD_writeSingleBlock(uint8_t *buff, uint32_t sector);
uint8_t SD_writeMultipleBlock(uint8_t *buff, uint32_t sector, uint16_t count);

uint8_t SD_sync(void);
uint8_t SD_get_sector_count(void * buff);
uint8_t SD_get_block_size(void * buff);
uint8_t SD_get_cardType(void);
uint8_t SD_read_csd(uint8_t * ptr);
uint8_t SD_read_cid(uint8_t * ptr);
uint8_t SD_read_ocr(uint8_t * ptr);
uint8_t SD_get_status(uint8_t * ptr);

#endif
