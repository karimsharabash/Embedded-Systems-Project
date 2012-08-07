#include "mpc555.h"
#include "UltraSensor.h"
#include "serial.h"

char isPulsing = 0;
/*
*  This function sets up the General-Purpose Input Output banks 
*/

void init_GPIO()
{
	//Operate under single chip mode
	//Allows us to hijak Data and address busses for i/o
	USIU.SIUMCR.B.SC = 0x3;
	
	
	//GDDR0: Data[0-7] are output
	
	USIU.SGPIOCR.B.GDDR0 = 1;
	USIU.SGPIOCR.B.SDDRD = 0xFF;  

	USIU.SGPIODT1.R = 0xFF000000; 
	//USIU.SGPIODT2.R = 0xFFFFFFFF; 						 

	//GDDR2: Data[16-23] are output

	
	
	//Send out an UltraSonic pulse right away
//	isPulsing =  0;
}
