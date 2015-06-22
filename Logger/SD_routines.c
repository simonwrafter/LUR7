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

#include <avr/io.h>
#include <avr/stdint.h>
#include <avr/pgmspace.h>
#include "SPI_routines.h"
#include "SD_routines.h"

volatile unsigned long startBlock, totalBlocks; 
volatile unsigned char SDHC_flag, cardType, buffer[512];

//******************************************************************
//Function	: to initialize the SD/SDHC card in SPI mode
//Arguments	: none
//return	: unsigned char; will be 0 if no error,
// 			  otherwise the response uint8_t will be sent
//******************************************************************
uint8_t SD_init(void) {
	uint8_t i, response, SD_version;
	uint16_t retry=0 ;

	for (i=0;i<10;i++) {
		SPI_transmit(0xff);   //80 clock pulses spent before sending the first command
	}
	SPI_select();
	do {
		response = SD_sendCommand(GO_IDLE_STATE, 0); //send 'reset & go idle' command
		retry++;
		if(retry>0x20) {
			return 1;   //time out, card not detected
		}
	} while (response != 0x01);

	SPI_deselect();
	SPI_transmit (0xff);
	SPI_transmit (0xff);

	retry = 0;

	SD_version = 2; //default set to SD compliance with ver2.x; 
				//this may change after checking the next command
	do {
		response = SD_sendCommand(SEND_IF_COND,0x000001AA); //Check power supply status, mendatory for SDHC card
		retry++;
		if(retry>0xfe) {
			SD_version = 1;
			cardType = 1;
			break;
		} //time out
	} while (response != 0x01);

	retry = 0;

	do {
		response = SD_sendCommand(APP_CMD,0); //CMD55, must be sent before sending any ACMD command
		response = SD_sendCommand(SD_SEND_OP_COND,0x40000000); //ACMD41

		retry++;
		if (retry>0xfe) {
			return 2;  //time out, card initialization failed
		}

	} while (response != 0x00);

	retry = 0;
	SDHC_flag = 0;

	if (SD_version == 2) {
		do {
			response = SD_sendCommand(READ_OCR,0);
			retry++;
			if(retry>0xfe) {
				cardType = 0;
				break;
			} //time out
		} while (response != 0x00);
		if (SDHC_flag == 1) {
			cardType = 2;
		} else {
			cardType = 3;
		}
	}
	//SD_sendCommand(CRC_ON_OFF, OFF); //disable CRC; deafault - CRC disabled in SPI mode
	//SD_sendCommand(SET_BLOCK_LEN, 512); //set block size to 512; default size is 512
	return 0; //successful return
}

//******************************************************************
//Function	: to send a command to SD card
//Arguments	: unsigned char (8-bit command value)
// 			  & unsigned long (32-bit command argument)
//return	: unsigned char; response uint8_t
//******************************************************************
uint8_t SD_sendCommand(uint8_t cmd, uint32_t arg) {
	uint8_t response, retry=0, status;

//SD card accepts uint8_t address while SDHC accepts block address in multiples of 512
//so, if it's SD card we need to convert block address into corresponding uint8_t address by 
//multipying it with 512. which is equivalent to shifting it left 9 times
//following 'if' loop does that

	if(SDHC_flag == 0) {
		if(cmd == READ_SINGLE_BLOCK     ||
				cmd == READ_MULTIPLE_BLOCKS  ||
				cmd == WRITE_SINGLE_BLOCK    ||
				cmd == WRITE_MULTIPLE_BLOCKS ||
				cmd == ERASE_BLOCK_START_ADDR||
				cmd == ERASE_BLOCK_END_ADDR ) {
			arg = arg << 9;
		}
	}

	SPI_select();

	SPI_transmit(cmd | 0x40); //send command, first two bits always '01'
	SPI_transmit(arg>>24);
	SPI_transmit(arg>>16);
	SPI_transmit(arg>>8);
	SPI_transmit(arg);

	if(cmd == SEND_IF_COND) { //it is compulsory to send correct CRC for CMD8 (CRC=0x87) & CMD0 (CRC=0x95)
		SPI_transmit(0x87);    //for remaining commands, CRC is ignored in SPI mode
	} else {
		SPI_transmit(0x95);
	}

	while ((response = SPI_receive()) == 0xff) { //wait response
		if(retry++ > 0xfe) {
			break; //time out error
		}
	}

	if (response == 0x00 && cmd == 58) { //checking response of CMD58
		status = SPI_receive() & 0x40;     //first uint8_t of the OCR register (bit 31:24)
		if (status == 0x40) {
			SDHC_flag = 1;  //we need it to verify SDHC card
		} else {
			SDHC_flag = 0;
		}
		SPI_receive(); //remaining 3 uint8_ts of the OCR register are ignored here
		SPI_receive(); //one can use these uint8_ts to check power supply limits of SD
		SPI_receive(); 
	}

	SPI_receive(); //extra 8 CLK
	SPI_deselect();

	return response; //return state
}

//*****************************************************************
//Function	: to erase specified no. of blocks of SD card
//Arguments	: none
//return	: unsigned char; will be 0 if no error,
// 			  otherwise the response uint8_t will be sent
//*****************************************************************
uint8_t SD_erase (uint32_t startBlock, uint32_t totalBlocks) {
	uint8_t response;

	response = SD_sendCommand(ERASE_BLOCK_START_ADDR, startBlock); //send starting block address
	if (response != 0x00) { //check for SD status: 0x00 - OK (No flags set)
		return response;
	}
	response = SD_sendCommand(ERASE_BLOCK_END_ADDR,(startBlock + totalBlocks - 1)); //send end block address
	if (response != 0x00) {
		return response;
	}
	response = SD_sendCommand(ERASE_SELECTED_BLOCKS, 0); //erase all selected blocks
	if (response != 0x00) {
		return response;
	}
	return 0; //normal return
}

//******************************************************************
//Function	: to read a single block from SD card
//Arguments	: none
//return	: unsigned char; will be 0 if no error,
// 			  otherwise the response uint8_t will be sent
//******************************************************************
uint8_t SD_readSingleBlock(uint32_t startBlock) {
	uint8_t response;
	uint16_t i, retry=0;

	response = SD_sendCommand(READ_SINGLE_BLOCK, startBlock); //read a Block command
	if (response != 0x00) {
		return response; //check for SD status: 0x00 - OK (No flags set)
	}

	SPI_select();

	retry = 0;
	while (SPI_receive() != 0xfe) { //wait for start block token 0xfe (0x11111110)
		if (retry++ > 0xfffe) {
			SPI_deselect();
			return 1;
		} //return if time-out
	}

	for (i=0; i<512; i++) { //read 512 uint8_ts
		buffer[i] = SPI_receive();
	}

	SPI_receive(); //receive incoming CRC (16-bit), CRC is ignored here
	SPI_receive();
	SPI_receive(); //extra 8 clock pulses
	SPI_deselect();
	return 0;
}

//******************************************************************
//Function	: to write to a single block of SD card
//Arguments	: none
//return	: unsigned char; will be 0 if no error,
// 			  otherwise the response uint8_t will be sent
//******************************************************************
uint8_t SD_writeSingleBlock(uint32_t startBlock) {
	uint8_t response;
	uint16_t i, retry=0;

	response = SD_sendCommand(WRITE_SINGLE_BLOCK, startBlock); //write a Block command
  
	if (response != 0x00) {
		return response; //check for SD status: 0x00 - OK (No flags set)
	}

	SPI_select();
	SPI_transmit(0xfe);     //Send start block token 0xfe (0x11111110)

	for (i=0; i<512; i++) {    //send 512 uint8_ts data
		SPI_transmit(buffer[i]);
	}

	SPI_transmit(0xff);     //transmit dummy CRC (16-bit), CRC is ignored here
	SPI_transmit(0xff);
	response = SPI_receive();

	if ((response & 0x1f) != 0x05) { //response= 0xXXX0AAA1 ; AAA='010' - data accepted
		SPI_deselect();              //AAA='101'-data rejected due to CRC error
		return response;             //AAA='110'-data rejected due to write error
	}

	while (!SPI_receive()) { //wait for SD card to complete writing and get idle
		if (retry++ > 0xfffe) {
			SPI_deselect();
			return 1;
		}
	}

	SPI_deselect();
	SPI_transmit(0xff);   //just spend 8 clock cycle delay before reasserting the CS line
	SPI_select();         //re-asserting the CS line to verify if card is still busy

	while (!SPI_receive()) { //wait for SD card to complete writing and get idle
		if (retry++ > 0xfffe) {
			SPI_deselect();
			return 1;
		}
	}
	SPI_deselect();
	return 0;
}
