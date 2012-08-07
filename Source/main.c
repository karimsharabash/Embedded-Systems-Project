//////////////////////////
//	Project Stationery  //
//////////////////////////

#include <stdio.h>
#include "m_mios.h"
#include "mpc555.h"
#include "serial.h"
#include "UltraSensor.h"
#include "Insect.h"

asm void system_call();

//extern init_GPIO();
void init_timer();
void init_pwm();
void init_QADC();
void bug_move(int, int);
void interruptC();
void mainWhile();
void turnLeft(int);
void turnRight(int);
void stop();
void forward();
void evadeLeft();
void evadeRight();
#define LEFT 0
#define FORWARD 1
#define RIGHT 2
#define CLOSE 180
#define TO_CLOSE 200
#define MAX_COUNT 50


char cForward = 0;
char cTurn = 0;
char* printBuffer[5];
char needPrint[5]; 
char move;
unsigned int needNumber[5];	
void InteruptC();



asm void system_call()
{
	nofralloc
	sc
	blr
}

void main()
{
	char c;
	
	char cBuff[10];
	int a;
	int i;
	int itoa1;
	volatile unsigned short result_right_IR;
	volatile unsigned short result_left_IR;
	
	//Allow interrupts

	
	init_QADC();
	//init_serial(0);
//	serial_putstring("\r            \r\0");
//	serial_putstring("Go Go Go\r\n");
	init_GPIO();

	//ultraSense();	
//	init_timer();
	
	// Pulse width modulator:
	asm
	{
		lis r11, 0x7FFF
		mtspr 22, r11
	}
	USIU.TBSCR.B.TBE = 1;
	init_pwm();

	//init_delta();
	USIU.SIPEND.B.IRQ1 = 1;
	USIU.SIPEND.B.IRQ2 = 1;
	USIU.SIPEND.B.IRQ3 = 1;
	USIU.SIPEND.B.IRQ4 = 1;
	USIU.SIPEND.B.LVL4 = 1;
	USIU.SIPEND.B.IRQ5 = 1;
	USIU.SIPEND.B.IRQ6 = 1;
	//for(i=0; i<5000000; i++)
	//{
			
	//}
	//USIU.SIEL.R   |= 0x02000000;
	USIU.SIEL.R   |= 0x2AA80000;
	//USIU.SIMASK.R |= 0x02000000;
	USIU.SIMASK.R |= 0x2AA80000;
	init_data_set();
	asm
	{
		mfmsr r12
		ori r12,r12,0x00008002@l
		mtmsr r12
		
	}
//	init_cam();
	//twitching = 1;
//	forward();
//	cForward = 1;
	move =0;
/*	
	while(1)
	{	
		
		switch (move)
		{
			case 0:
				stop();
				stationary();
				break;
			case 1:
				turning();
				break;
			case 3:
				forward();
				moving();
				break;
			case 4:
				stop();
				confirmation();
				break;
		}
		
	}*/
	mainWhile();
}

void mainWhile()
{
	volatile unsigned int IRres1,IRres2;
	
	static char count0 = 0;
	static char count1 = 0;
	static char count2 = 0;
	
	while(1)
	{

		IRres1 = QADC_A.RJURR[0].B.RESULT;	
		IRres2 = QADC_A.RJURR[1].B.RESULT;	
		
		switch (move)
		{
			case 0:
				stop();
				song_num = 0;
				stationary();
				break;
			case 1:
				song_num = 1;
				turning();
				break;
			case 3:
				song_num = 2;
				/*if ((IRres1 > TO_CLOSE || IRres2 > TO_CLOSE)||(IRres1 > CLOSE && IRres2 > CLOSE))
				{
					
					count0++;
					if (count0 > MAX_COUNT)
					{
						turnLeft(5);
						count0 = 0;
						count1 = 0;
						count2 = 0;
					}
				}
				else */if((IRres1> CLOSE) && (IRres1 < TO_CLOSE) && (IRres2 < TO_CLOSE))
				{
					count1++;
					if (count1 > MAX_COUNT)
					{	
						evadeLeft();
						count0 = 0;
						count1 = 0;
						count2 = 0;
					}
				}
				else if ((IRres2 > CLOSE)  && (IRres1 < TO_CLOSE) && (IRres2 < TO_CLOSE))
				{
					count2++;
					if (count2 > MAX_COUNT)
					{
						evadeRight();
						count0 = 0;
						count1 = 0;
						count2 = 0;
					}
					
				}
				else
				{
					count0 = 0;
					count1 = 0;
					count2 = 0;
					forward();
					moving();
				}
				break;
			case 4:
				stop();
				song_num = 3;
				confirmation();
				break;
		}
	}
}
void evadeLeft()
{
	move=2;
	MIOS1.MPWMSM19PULR.R = 0x5A;//0x96; // right wheel
	MIOS1.MPWMSM2PULR.R = 0x5A;  // left wheel		
	//MIOS1.MPWMSM19SCR.B.EN = 1; // Enable prescaler
	//MIOS1.MPWMSM2SCR.B.EN = 1; // Enable prescaler
	TurnLen = EVADE_TIME;
	MIOS1.MPWMSM1SCR.B.EN = 1;
}
void evadeRight()
{
	move=2;
	MIOS1.MPWMSM19PULR.R = 0xD2; // right wheel
	MIOS1.MPWMSM2PULR.R = 0xD2;//0x96;  // left wheel		
	//MIOS1.MPWMSM19SCR.B.EN = 1; // Enable prescaler
	//MIOS1.MPWMSM2SCR.B.EN = 1; // Enable prescaler
	TurnLen = EVADE_TIME;
	MIOS1.MPWMSM1SCR.B.EN = 1;
}

void turnLeft(int len)
{
	MIOS1.MPWMSM19PULR.R = 0xD2; // right wheel
	MIOS1.MPWMSM2PULR.R = 0xD2;  // left wheel
	TurnLen = (unsigned int)len;
	MIOS1.MPWMSM1SCR.B.EN = 1;
}
void turnRight(int len)
{
	MIOS1.MPWMSM19PULR.R = 0x5A; // right wheel
	MIOS1.MPWMSM2PULR.R = 0x5A;  // left wheel
	TurnLen = (unsigned int)len;
	MIOS1.MPWMSM1SCR.B.EN = 1;
}

void forward()
{
	cForward = 1;
	MIOS1.MPWMSM19PULR.R = 0xD2; // right wheel
	MIOS1.MPWMSM2PULR.R = 0x5A;  // left wheel		
	MIOS1.MPWMSM19SCR.B.EN = 1; // Enable prescaler
	MIOS1.MPWMSM2SCR.B.EN = 1; // Enable prescaler
}
void stop()
{
	cForward = 0;
	MIOS1.MPWMSM19PULR.R = 0x96; // right wheel
	MIOS1.MPWMSM2PULR.R = 0x96;  // left wheel		
	MIOS1.MPWMSM19SCR.B.EN = 0; // Enable prescaler
	MIOS1.MPWMSM2SCR.B.EN = 0; // Enable prescaler
}

void bug_move(int start, int direction)
{
	int i;
	if(start)
	{

		if(direction == LEFT)
		{
			MIOS1.MPWMSM19SCR.B.EN = 1; // Enable prescaler
			MIOS1.MPWMSM2SCR.B.EN = 1; // Enable prescaler
			MIOS1.MPWMSM19PULR.R = 0x5A; // right wheel
			MIOS1.MPWMSM2PULR.R = 0x5A;  // left wheel
			for(i=0; i<5000000; i++) {}   // wait for turning
			MIOS1.MPWMSM19SCR.B.EN = 0; // Disable prescaler (turn wheel off)
			MIOS1.MPWMSM2SCR.B.EN = 0; // Disable prescaler (turn wheel off)
		}
		if(direction == RIGHT)
		{
			MIOS1.MPWMSM19SCR.B.EN = 1; // Enable prescaler
			MIOS1.MPWMSM2SCR.B.EN = 1; // Enable prescaler
			MIOS1.MPWMSM19PULR.R = 0xD2; // right wheel
			MIOS1.MPWMSM2PULR.R = 0xD2;  // left wheel
			for(i=0; i<5000000; i++) {}   // wait for turning
			MIOS1.MPWMSM19SCR.B.EN = 0; // Disable prescaler (turn wheel off)
			MIOS1.MPWMSM2SCR.B.EN = 0; // Disable prescaler (turn wheel off)
		}
		if(direction == FORWARD)
		{
			MIOS1.MPWMSM19SCR.B.EN = 1; // Enable prescaler
			MIOS1.MPWMSM2SCR.B.EN = 1; // Enable prescaler
			MIOS1.MPWMSM19PULR.R = 0x5A; // right wheel
			MIOS1.MPWMSM2PULR.R = 0xD2;  // left wheel
			//for(i=0; i<50000000; i++) {}   // wait for turning
			// Here, we let the motors keep spinning until something turns it off.
		}
	}
	else
	{
		MIOS1.MPWMSM19SCR.B.EN = 0; // Disable prescaler (turn wheel off)
		MIOS1.MPWMSM2SCR.B.EN = 0; // Disable prescaler (turn wheel off)
	}
	
}


