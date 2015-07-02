/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include <stdint.h>
#include "diskio.h"		/* FatFs lower layer API */
#include "SD_routines.h"

/* Definitions of physical drive number for each drive */
#define SD		0

static volatile uint8_t disc_status = STA_NOINIT;

uint8_t disk_status (uint8_t pdrv) {
	if (pdrv) {
		return STA_NOINIT;
	}
	return disc_status;
}

uint8_t disk_initialize (uint8_t pdrv) {
	if (pdrv) {
		return STA_NOINIT;
	}
	return SD_init();
}

DRESULT disk_read (
	uint8_t pdrv,		/* Physical drive nmuber to identify the drive */
	uint8_t *buff,		/* Data buffer to store read data */
	uint32_t sector,	/* Sector address in LBA */
	uint16_t count		/* Number of sectors to read */
) {
	uint8_t res = 0;
	
	if (pdrv || !count) {
		return RES_PARERR;
	}
	if (disc_status & STA_NOINIT) {
		return RES_NOTRDY;
	}
	
	if (count == 1) {	/* Single block read */
		res = SD_readSingleBlock(buff, sector);
	} else {			/* Multiple block read */
		res = SD_readMultipleBlock(buff, sector, count);
	}
	
	return res ? RES_ERROR : RES_OK;
}

#if _USE_WRITE
DRESULT disk_write (
	uint8_t pdrv,			/* Physical drive nmuber to identify the drive */
	const uint8_t *buff,	/* Data to be written */
	uint32_t sector,		/* Sector address in LBA */
	uint16_t count			/* Number of sectors to write */
) {
	uint8_t res = 0;
	uint8_t * buffer = (uint8_t *) buff;
	
	if (pdrv || !count) {
		return RES_PARERR;
	}
	if (disc_status & STA_NOINIT) {
		return RES_NOTRDY;
	}
	
	if (count == 1) {	/* Single block read */
		res = SD_writeSingleBlock(buffer, sector);
	} else {			/* Multiple block read */
		res = SD_writeMultipleBlock(buffer, sector, count);
	}
	
	return res ? RES_ERROR : RES_OK;
}
#endif

#if _USE_IOCTL
DRESULT disk_ioctl (
	uint8_t pdrv,		/* Physical drive nmuber (0..) */
	uint8_t cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
) {
	DRESULT res = RES_ERROR;
	uint8_t *ptr = buff;
	
	if (pdrv) {
		return RES_ERROR;
	}
	
	if (cmd == CTRL_POWER) {
		switch (ptr[0]) {
			case 0:		/* Sub control code (POWER_OFF) */
				res = RES_OK;
				break;
			case 1:		/* Sub control code (POWER_GET) */
				ptr[1] = 1;
				res = RES_OK;
				break;
			default :
				res = RES_PARERR;
		}
	} else {
		if (disc_status & STA_NOINIT) {
			return RES_NOTRDY;
		}
		
		switch (cmd) {
			case CTRL_SYNC :		/* Make sure that no pending write process. */
				if (SD_sync()) {
					res = RES_OK;
				}
				break;
				
			case GET_SECTOR_COUNT : /* Get number of sectors on the disk (DWORD) */
				if (SD_get_sector_count(buff)) {
					res = RES_OK;
				}
				break;
				
			case GET_SECTOR_SIZE :	/* Get R/W sector size (uint16_t) */
				*(uint16_t*)buff = 512;
				res = RES_OK;
				break;
				
			case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
				if (SD_get_block_size(buff)) {
					res = RES_OK;
				}
				break;
				
			case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
				*ptr = SD_get_cardType();
				res = RES_OK;
				break;
				
			case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
				if (SD_read_csd(ptr)) {
					res = RES_OK;
				}
				break;
				
			case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
				if (SD_read_cid(ptr)) {
					res = RES_OK;
				}
				break;
				
			case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
				if (SD_read_ocr(ptr)) {
					res = RES_OK;
				}
				break;
				
			case MMC_GET_SDSTAT :	/* Receive SD status as a data block (64 bytes) */
				if (SD_get_status(ptr)) {	/* SD_STATUS */
					res = RES_OK;
				}
				break;
				
			default:
				res = RES_PARERR;
		}
		
		SD_deselect();
	}
	
	return res;
}
#endif
