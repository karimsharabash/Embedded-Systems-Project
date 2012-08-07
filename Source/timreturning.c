#include "mpc555.h"
#include "Insect.h"

void init_data_set();
void moving();
void stationary();
void turning();
void confirmation();

volatile unsigned short pos;
char whichSensor;

void init_data_set()
{
	int i;
	for(i=0; i<Q_SIZE; i++)
	{
		dist0[i]=0;
		dist1[i]=0;

	}

	for(i=0; i<AVG_NUM-1; i++)
	{
		delta0[i]=0x80000000;
		delta1[i]=0x80000000;
	}
	
	for(i=0; i<AVG_NUM; i++)
	{
		dist0_avg[i] = 0;
		dist1_avg[i] = 0;
	}

}
void moving()
{
	char i;

	char motion1 = 1;
	char balloon = 0;
	static int count = 0;
	USIU.SIMASK.R &= 0xD7F70000;
	
	

	for(i=1; i<Q_SIZE; i++)
	{
		if(dist1[i]==0)
			continue;
		if(dist1[i] >= dist1[i-1]+100)
		{
			motion1 = 0;
			break;
		}
	}	
	

	if((dist1[0] + D_SAFETY) <= dist1[Q_SIZE-1])
	{
		motion1 = 0;
	}



	USIU.SIMASK.R |= 0x28080000;
	
	if((motion1== 0) && (count > MOVE_MIN))
	{
	    count = 0;
		stop();
		move = 4;
	}
	count++;
	
}
void stationary()
{
	char i;
	char motion0 = 1;
	char motion1 = 1;
	char balloon = 0;
	
	
	USIU.SIMASK.R &= 0xD7F70000;
	
	for(i=1; i<Q_SIZE; i++)
	{
		if(dist0[i]+50 >= dist0[i-1])
		{
			motion0 = 0;
			break;
		}
	}

	for(i=1; i<Q_SIZE; i++)
	{
		if(dist1[i]+50 >= dist1[i-1])
		{
			motion1 = 0;
			break;
		}
	}	
	
	if((dist0[Q_SIZE-1] + D_SAFETY) >= dist0[0])
	{
		motion0 = 0;
	}

	if((dist1[Q_SIZE-1] + D_SAFETY) >= dist1[0])
	{
		motion1 = 0;
	}

	pos = MIOS1.MPWMSM0PULR.R;
	
	USIU.SIMASK.R |= 0x28080000;
	
	if((motion0==1) || (motion1==1))
	{
		//cForward = 1;
		whichSensor = motion1;
		move = 1;
	}
}
void confirmation()
{
	char i;
	char motion1 = 1;
	
	
	USIU.SIMASK.R &= 0xD7F70000;
	
	for(i=1; i<Q_SIZE; i++)
	{
		if(dist1[i]+50 >= dist1[i-1])
		{
			motion1 = 0;
			break;
		}
	}	
	

	if((dist1[Q_SIZE-1] + D_SAFETY) >= dist1[0])
	{
		motion1 = 0;
	}



	USIU.SIMASK.R |= 0x28080000;
	
	if((motion1==1))
	{
		//cForward = 1;
		move = 3;
	}
}

// 45. 3C. E6

void turning()
{
	if(whichSensor == 0)
	{
		
		turnLeft((0xE6-pos)/TURN_AMOUNT * (TURN_SUB_LEN+10));
		turnTime = (0xE6-pos)/TURN_AMOUNT * (TURN_SUB_LEN+10);
	}
	else
	{
		turnRight((pos-0x3C)/TURN_AMOUNT * (TURN_SUB_LEN+15));
		turnTime = (pos-0x3C)/TURN_AMOUNT * (TURN_SUB_LEN+15);
	}
	move = 2;
}