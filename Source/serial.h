/*################################################################
#	file: serial_func.h
#	Description:Sets up and uses 555 serial port
#
#
#	Includes functions for serial communication
#	and conversion of data
#
#	Notes: Some functions based on EECS 461 documentation and 
#	source code
#	Initial Version:		Matthew Fornero		11/24/2005
################################################################*/
#ifndef __SERIAL_FUNC__
#define __SERIAL_FUNC__

#define LF 10			/* line feed */
#define CR 13			/* carriage return */

#include "mpc555.h"

/*################################################################
#	name: init_serial
#	description: initialize serial port SCC1
#
#	SCC1R0 		SCI1 Control Register 0
#		SC1BR	Baud Rate
#
#	SCC1R1		SCI1 Control Register 1
#		TE		Transmit Enable
#		RE		Recieve Enable
#		RIE		Recieve interrupt enable
################################################################*/
void init_serial(int mode);

/*################################################################
#	name: serial_putchar
#	description: blocking function to send 8 bits over serial port
#
#	SC1DR 		SCI1 Data Register
#
#	SC1SR		SCI1 Status Register
#		TC		Transmit complete (transmitter idle)
################################################################*/
void serial_putchar(unsigned char sbyte);

/*################################################################
#	name: serial_putstring
#	description: send null terminated string over the serial port
################################################################*/
void serial_putstring(char *sptr);

/*################################################################
#	name: serial_putchar
#	description: blocking function to get 8 bits from serial port
#
#	SC1DR 		SCI1 Data Register
#
#	SC1SR		SCI1 Status Register
#		RDRF	Recieve data register full
################################################################*/
char serial_getchar();

/*################################################################
#	name: chartohex
#	description: takes a character 0-9, a-f, or A-F and returns
#	its hex value
#
#	input: 8 bit ASCII character
#	returns: hex value of character
################################################################*/
INT8 chartohex(UINT8 byte);

/*################################################################
#	name: strtohex
#	description: converts a string of characters into a single 
#	integer value until it encounters white space
#
#	input: string of hex characters terminated by whitespace
#	returns: hex value of characters
################################################################*/
int strtohex(char * ptr);



void itoa(int num, char* str);


void prompt(char*);
void serial_getstring(char*);

#endif