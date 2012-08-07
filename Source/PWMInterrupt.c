#include "mpc555.h"
#include "Insect.h"
#include "serial.h"
#define NUM_WAIT 0
#define NUM_AVG 4
#define NUM_WAIT_END 0
#define POLL_STATE 11
#define MOVE_STATE 8
#define SAFETY 1000
void wheelTimer();
void PWMHandler3();
void twitchTimer();
void PWMHandler17();
void sensorStart1();
void sensorEnd1();
void sensorStart2();
void sensorEnd2();
int TurnLen;
unsigned int direction = 0; // 0 - clockwise, 1 - counterclockwise
volatile unsigned int pulse1_high_time;
volatile unsigned int pulse1_low_time;
volatile unsigned int pulse2_high_time;
volatile unsigned int pulse2_low_time;
char position = 0; // 0 = 0, 1 = 45, 2 = 90, 3 = 135

unsigned int dist0[Q_SIZE];
unsigned int dist1[Q_SIZE];

int delta0[AVG_NUM-1];
int delta1[AVG_NUM-1];

unsigned int dist0_avg[AVG_NUM];
unsigned int dist1_avg[AVG_NUM];

char high1done = 0;
char high2done = 0;

char song_num = 0;

char turnTime = 0;

short notes [NUM_SONGS][SONG_LEN] = 
{
{hG,hA,hF,mF,hC,0},
{mA,mB,mG,lA,hD,mC,mA,hB},
{hG,hA,hF,mF,hC,0},
{hG, hG, hG, hG, hG}
};
short note_len [NUM_SONGS][SONG_LEN] =
{
{8,8,8,8,16,8},
{1,1,1,1,1,1,1,1},
{1,1,1,1,2,1},
{1,1,1,1,1,1}
};


//Run left wheel
void wheelTimer() //PWM 1
{

	if(TurnLen > 0)
	{
		MIOS1.MPWMSM19SCR.B.EN = 1;
		MIOS1.MPWMSM2SCR.B.EN = 1;
		TurnLen--;
	}
	else
	{
		TurnLen = 0;
		MIOS1.MPWMSM19PULR.R = 0x96;  // right wheel
		MIOS1.MPWMSM19SCR.B.EN = 0;
		MIOS1.MPWMSM2PULR.R = 0x96;  // left wheel
		MIOS1.MPWMSM2SCR.B.EN = 0;
		
		MIOS1.MPWMSM1SCR.B.EN = 0;
		
	}
	USIU.SIPEND.B.IRQ6 = 1;
}

void twitchTimer() //PWM 16
{	
	static int flag = 1;
	static char cnt = 0;
	static char cnt2 = 0;
	static char cnt3 = 0;
	static char song_cnt = 0;
	static char song_local = 0xFF;
	static char curr_note = 0;
	
	if (move == 0)
	{
		cnt2 = 0;
		cnt3 = 0;
		if (cnt == 0)
		{
			USIU.SGPIODT1.R &= 0x00FFFFFF;
		}
		else if (cnt == 1)
		{
			MIOS1.MPWMSM0PULR.R = (unsigned short)(MIOS1.MPWMSM0PULR.R + flag*TURN_AMOUNT);
			if(MIOS1.MPWMSM0PULR.R <= 0x3C)
			{
				flag = 1;
			}
			else if (MIOS1.MPWMSM0PULR.R >= 0xE6)
			{
				flag = -1;
			}
		}
		else if(cnt == 2+MOVE_STATE)
		{
			init_data_set();
			USIU.SGPIODT1.R |= 0xFF000000;
		
		}
		cnt = (char)((cnt+1)%(3+POLL_STATE+MOVE_STATE));
	}
	else if(move <= 3)
	{
		cnt = 0;
		cnt3 = 0;
		if(TurnLen!=0/*cnt2 == 0*/)
		{
			MIOS1.MPWMSM0PULR.R = 0x3C;
		}
		else if (TurnLen<=0/*cnt2 == turnTime 45*/)
		{
			USIU.SGPIODT1.R |= 0xFF000000;
			move = 3;	
		}
		/*else if(cnt2 >= turnTime +10)
		{
			move = 3;
		}*/
		
		
		cnt2 = (char)(cnt2+1);
	}
	else if(move == 4)
	{
		USIU.SGPIODT1.R |= 0xFF000000;
		cnt = 0;
		cnt2 = 0;
		if (cnt3 == 60)
		{
			cnt3 = 0;
			move = 0;
		}
		cnt3++;
		
	}
	if (song_local != song_num)
	{
		song_local = song_num;
		curr_note = 0;
		song_cnt = 0;
		
	}
	if((curr_note==-1) || (curr_note == SONG_LEN))
	{
		curr_note = 0;
	}
	
	if(song_cnt < 2*note_len[song_num][curr_note])
	{
		MIOS1.MPWMSM17PULR.R = notes[song_num][curr_note];
		MIOS1.MPWMSM17PERR.R = 2*notes[song_num][curr_note];
	}
	else if (song_cnt < 2*note_len[song_num][curr_note]+1)
	{
		MIOS1.MPWMSM17PULR.R = 0;
	}
	else
	{
		curr_note++;
		song_cnt = 0;	
	}
	
	song_cnt++;
	
	USIU.SIPEND.B.IRQ5 = 1;
}


void sensorStart1() //Now IRQ 3
{
	asm
	{
		mftb r12
		lis r11, pulse1_high_time@h
		ori r11, r11, pulse1_high_time@l
		stw r12, 0(r11)
		lis r11, 0x7FFF
		mtspr 22, r11
	}
	high1done = 1;
	USIU.SIPEND.B.IRQ3 = 1;
	
}

void sensorEnd1() //Now IRQ 1
{
	unsigned int temp_sub;
	unsigned int dist0_avg_temp;
	unsigned int dist_sum = 0;
	
	char i;
	if(high1done != 1)
	{
		USIU.SIPEND.B.IRQ1 = 1;
		return;
	}
	asm
	{
		mftb r12
		lis r11, pulse1_low_time@h
		ori r11, r11, pulse1_low_time@l
		stw r12, 0(r11)
		lis r11, 0x7FFF
		mtspr 22, r11
	}
	
	temp_sub = (pulse1_low_time - pulse1_high_time);
	
	for(i=0; i<Q_SIZE-1; i++)
	{
		dist0[i] = dist0[i+1];
	}
	
	// Conditional insert here
	if(temp_sub < 30000)
	{
		dist0[i] = temp_sub;
	}
	else
	{
		dist0[i] = dist0[i-1];
	}
/*
	for(i=0; i<Q_SIZE; i++)
	{
		if(dist0[i]=0)
			dist0[i] = temp_sub;
	}
*/	
	for(i=0; i<Q_SIZE; i++)
	{
		dist_sum = dist_sum + dist0[i];
	}
	dist0_avg_temp = dist_sum/Q_SIZE;
	
	for(i=0; i<AVG_NUM-1; i++)
	{
		dist0_avg[i] = dist0_avg[i+1];
	}
	
	dist0_avg[i] = dist0_avg_temp;
	
	for(i=0; i<AVG_NUM-2; i++)
	{
		delta0[i] = delta0[i+1];
	}

	delta0[i] = (int)dist0_avg[i] - (int)dist0_avg[i+1];
	
	high1done = 0;
	USIU.SIPEND.B.IRQ1 = 1;
}
void sensorStart2() //IRQ 4
{
	asm
	{
		mftb r12
		lis r11, pulse2_high_time@h
		ori r11, r11, pulse2_high_time@l
		stw r12, 0(r11)
		lis r11, 0x7FFF
		mtspr 22, r11
	}
	high2done = 1;
	//USIU.TBSCR.B.TBE = 1;
	USIU.SIPEND.B.IRQ4 = 1;
	
}

void sensorEnd2() //IRQ 2
{
	unsigned int temp_sub;
	unsigned int dist1_avg_temp;
	unsigned int dist_sum = 0;

	char i;
	if(high2done != 1)
	{
		USIU.SIPEND.B.IRQ2 = 1;
		return;
	}
	asm
	{
		mftb r12
		lis r11, pulse2_low_time@h
		ori r11, r11, pulse2_low_time@l
		stw r12, 0(r11)
		lis r11, 0x7FFF
		mtspr 22, r11
	}

	temp_sub = (pulse2_low_time - pulse2_high_time);
	
	for(i=0; i<Q_SIZE-1; i++)
	{
		dist1[i] = dist1[i+1];
	}
	
	// Conditional insert here
	if(temp_sub < 30000)
	{
		dist1[i] = temp_sub;
	}
	else
	{
		dist1[i] = dist0[i-1];
	}
/*
	for(i=0; i<Q_SIZE; i++)
	{
		if(dist1[i]=0)
			dist1[i] = temp_sub;
	}
*/
	for(i=0; i<Q_SIZE; i++)
	{
		dist_sum = dist_sum + dist1[i];
	}
	dist1_avg_temp = dist_sum/Q_SIZE;
	
	for(i=0; i<AVG_NUM-1; i++)
	{
		dist1_avg[i] = dist1_avg[i+1];
	}
	
	dist1_avg[i] = dist1_avg_temp;
	
	for(i=0; i<AVG_NUM-2; i++)
	{
		delta1[i] = delta1[i+1];
	}

	delta1[i] = (int)dist1_avg[i] - (int)dist1_avg[i+1];
	
	high2done = 0;
	USIU.SIPEND.B.IRQ2 = 1;
}

void speaker()
{
	static volatile short sound = 0;
	//sound = (USIU.SGPIODT1.R & 0x00FF0000)>>16;
	sound += 7;
	USIU.SGPIODT1.R &= 0xFFFFFF00;
	USIU.SGPIODT1.R |= sound;
	USIU.PISCR.B.PTE = 1;
}
void snk()
{
	
}
void InterruptHandler(long int loser)
{
	
	
}
