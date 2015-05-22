//**************************************************************
// ****** FUNCTIONS FOR SD RAW DATA TRANSFER *******
//**************************************************************
//Controller: ATmega32M1 (Clock: 16 Mhz-external)
//Compiler	: AVR-GCC (winAVR with AVRStudio)
//Project V.: Version - 2.4.1
//Author	: CC Dharmani, Chennai (India)
//			  www.dharmanitech.com
//Date		: 24 Apr 2011
//**************************************************************

//Link to the Post: http://www.dharmanitech.com/2009/01/sd-card-interfacing-with-atmega8-fat32.html

#ifndef _SD_ROUTINES_H_
#define _SD_ROUTINES_H_


//SD commands, many of these are not used here
#define GO_IDLE_STATE            0
#define SEND_OP_COND             1
#define SEND_IF_COND			 8
#define SEND_CSD                 9
#define STOP_TRANSMISSION        12
#define SEND_STATUS              13
#define SET_BLOCK_LEN            16
#define READ_SINGLE_BLOCK        17
#define READ_MULTIPLE_BLOCKS     18
#define WRITE_SINGLE_BLOCK       24
#define WRITE_MULTIPLE_BLOCKS    25
#define ERASE_BLOCK_START_ADDR   32
#define ERASE_BLOCK_END_ADDR     33
#define ERASE_SELECTED_BLOCKS    38
#define SD_SEND_OP_COND			 41   //ACMD
#define APP_CMD					 55
#define READ_OCR				 58
#define CRC_ON_OFF               59


#define ON     1
#define OFF    0

volatile unsigned long startBlock, totalBlocks; 
volatile unsigned char SDHC_flag, cardType, buffer[512];

unsigned char SD_init(void);
unsigned char SD_sendCommand(unsigned char, unsigned long);
unsigned char SD_readSingleBlock(unsigned long );
unsigned char SD_writeSingleBlock(unsigned long);
unsigned char SD_readMultipleBlock (unsigned long, unsigned long);
unsigned char SD_writeMultipleBlock(unsigned long, unsigned long);
unsigned char SD_erase (unsigned long, unsigned long);

#endif
