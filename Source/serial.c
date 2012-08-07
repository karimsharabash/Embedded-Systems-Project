/*################################################################
#	file: serial_func.c
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


#include "serial.h"
#include "mpc555.h"
#include "m_usiu.h"

unsigned char synch[6] = {0xAA,0x0D,0x00,0x00,0x00,0x00};
//unsigned char synch[6] = {'T','I','M','M','M','\n'};
unsigned char ack[6] = {0xAA,0x0E,0xFF,0x00,0x00,0x00};  //Set Third bit
unsigned char initial[6] = {0xAA,0x01,0x00,0x05,0x01,0x00};
unsigned char getPic[6] = {0xAA,0x04,0x01,0x00,0x00,0x00};  // Set for snapshot
unsigned char snapshot[6] = {0xAA,0x05,0x01,0x00,0x00,0x00};  // no dropped frames
unsigned char packSize[6] = {0xAA,0x06,0x08,0x00,0x00,0x00};
unsigned char sync[6] = {0xAA,0x0D,0x00,0x00,0x00,0x00};

unsigned char resp[6] = {'T','I','M','M','\r','\n'};

char noAck = 1;

void cam_send_command_block(volatile unsigned char* c);
char cam_send_command_nonblock(volatile unsigned char* c);
char cam_get_response_nonblock();
char getACK();

void init_cam();
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
##############################################################*/

void init_serial(int mode)
{

	/*set baud rate to 9600*/
	/*Baud = SYSTEM_FREQUENCY / (32 * SC1BR);*/
	QSMCM.SCC1R0.B.SC1BR = 0x82;
	
	/*set interrupt level to 4*/
	QSMCM.QDSCI_IL.B.ILDSCI = 0x7;
	
	/*enable interrupts if mode ==1 */
	QSMCM.SCC1R1.B.RIE = mode;
	
	/*enable transmit, recieve*/
	QSMCM.SCC1R1.B.TE = 0x1;
	QSMCM.SCC1R1.B.RE = 0x1;
	
}

//Call after interrupts enabled
void init_cam()
{
	char noACK = 0xFF;
	char two;
	/*set baud rate to 9600*/
	/*Baud = SYSTEM_FREQUENCY / (32 * SC1BR);*/
	QSMCM.SCC1R0.B.SC1BR = 0x41;//0x82;
	
	/*set interrupt level to 4*/
	QSMCM.QDSCI_IL.B.ILDSCI = 0x7;
	
	/*enable interrupts if mode ==1 */
	QSMCM.SCC1R1.B.RIE = 0;
	
	/*enable transmit, recieve*/
	QSMCM.SCC1R1.B.TE = 0x1;
	QSMCM.SCC1R1.B.RE = 0x1;

/*	while(1)
	{
		serial_putchar('A');	
	}*/
	
	while(noACK == 0xFF)
	{
		cam_send_command_block(synch);
		noACK = getACK();
		two = resp[1];
	}
	cam_send_command_block(ack);
	
		
}
void cam_send_command_block(volatile unsigned char* c)
{
	char i;
	for (i = 0;i < 6;i++)
	{
			
		while(!((volatile)QSMCM.SC1SR.B.TC))
		{
			
		}
		QSMCM.SC1DR.R = *(c+i);	
	}
		
}
char cam_send_command_nonblock(volatile unsigned char* c)
{
	static char i = 0;
	
	if((volatile)QSMCM.SC1SR.B.TC)
	{
		QSMCM.SC1DR.R = *(c+i);
		
		if (i == 5)
		{
			i = 0;
			return 1;			
		}
		i++;
		
	}
	return 0;
			

		
}
char cam_get_response_nonblock()
{

	static int i = 0;
	char c = 0xFF;
	/* wait for new incoming data */

	c = (volatile)QSMCM.SC1SR.B.RDRF;
	if(c == 1)
	{
		resp[i] = (unsigned char)QSMCM.SC1DR.R & 0x00ff;
		if(i == 5)
		{
			i = 0;
			return 1;
		}
		i++;
	}
	return 0;
}
char getACK()
{
	char i;
	//char ret = 1;
	ack[2] = 0x0D;
	if(cam_get_response_nonblock())
	{
		for(i = 0;i < 6; i++)
		{
			if (ack[i] != resp[i])
			{
				return 0;
			}
		}
		return 1;
	}
	return 0xFF;

}

/*################################################################
#	name: serial_putchar
#	description: blocking function to send 8 bits over serial port
#
#	SC1DR 		SCI1 Data Register
#
#	SC1SR		SCI1 Status Register
#		TC		Transmit complete (transmitter idle)
################################################################*/
void serial_putchar(VUINT8 byte)
{
	/* wait for ready to send */
	while(!QSMCM.SC1SR.B.TC)
	{
		
	}
	/*put both LR and CR char if newline*/
	if(byte == '\n')
	{
		QSMCM.SC1DR.R = CR;
		while(!QSMCM.SC1SR.B.TC)
		{
			
		}
		QSMCM.SC1DR.R = LF;	
	}
	/*transmit data*/
	else
		QSMCM.SC1DR.R = byte;
}

/*################################################################
#	name: serial_putstring
#	description: send null terminated string over the serial port
################################################################*/
void serial_putstring(char *sptr)
{
	while(*sptr)
	{
		serial_putchar(*sptr);
		++sptr;
	}
}

/*################################################################
#	name: serial_getchar
#	description: blocking function to get 8 bits from serial port
#
#	SC1DR 		SCI1 Data Register
#
#	SC1SR		SCI1 Status Register
#		RDRF	Recieve data register full
################################################################*/
char serial_getchar()
{

	/* wait for new incoming data */
	while(!QSMCM.SC1SR.B.RDRF)
	{
		
	}

	/*return data*/
	return (unsigned char)(QSMCM.SC1DR.R & 0x00ff);
}
/*################################################################
#	name: serial_getstring
#	description: blocking function to get 8 bits from serial port
#
#	SC1DR 		SCI1 Data Register
#
#	SC1SR		SCI1 Status Register
#		RDRF	Recieve data register full
################################################################*/
void serial_getstring(char* string)
{
	char j = 'j';
	int i = 0;
	//char string[50];
			j = serial_getchar();
		serial_putchar(j);
		string[i] = j;
		i++;
	
	while(i<20 && j != '\n' && j != '\r')
	{
		j = serial_getchar();
		serial_putchar(j);
		string[i] = j;
		i++;
	}
	string[i] = '\0';
	serial_putchar('\n');
	return;
}


/*################################################################
#	name: chartohex
#	description: takes a character 0-9, a-f, or A-F and returns
#	its hex value
#
#	input: 8 bit ASCII character
#	returns: hex value of character
################################################################*/
INT8 chartohex(UINT8 byte)
{
	/* byte = 0-9*/
	if (byte > 47 && byte < 58)
		return (INT8)(byte - 48);
	/*byte = A-F*/
	else if (byte > 64 && byte < 71)
		return (INT8)(byte-65 + 10);
	/*byte = a-f*/
	else
		return (INT8)(byte-97+10);
}

/*################################################################
#	name: strtohex
#	description: converts a string of characters into a single 
#	integer value until it encounters white space
#
#	input: string of hex characters terminated by whitespace or
#	newline or null character
#	returns: hex value of characters
################################################################*/
int strtohex(char * ptr)
{
	UINT32 value;
	/*convert until you reach a space*/
	value = chartohex(*ptr);
	ptr++;
	while(*ptr != ' ' && *ptr!='\r' && *ptr)
	{
		/*move current value one hex digit over*/
		value <<=4;
		/*add next hex digit*/
		value |=chartohex(*ptr);
		/*increment pointer*/
		ptr++;	
	}
	return (int)value;
}


// Takes an int and char* as inputs.  Modifies passed in char* to be
// a string representation of the int that was passed in.
void itoa(int num, char* str) {
	unsigned char counter=0, tcounter=0;
	char temp[10] = {'\0'};

	// Check to see if number is negative
	if(num<0) {
		str[counter++] = '-';
		num = (~num) + 1;
	}

	
	// Strip off ones place of the int and put into string
	// Then make 10s place of int the new 1s place.  Keep doing
  	// till entire number has been put backwards into string
	do{
		temp[tcounter++] = num % 10 + '0';		
		num = num / 10;
	}while(num);

	// Reverse string so it is in the correct order
	while(tcounter>0) {
		str[counter++] = temp[--tcounter];
	}		
	str[counter] = '\0';
}