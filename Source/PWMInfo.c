#include "mpc555.h"
#include "Insect.h"

void init_pwm_wheels();
void init_pwm();
void init_pwm_timers();
void init_pwm_sensor();
void init_pwm_turntable();
/*
This starts up the PWM
*/
void init_pwm()
{
	//Must unlock register to write to it.
	USIU.PLPRCRK.R = 0x55CCAA33; // Unlock the PLPRCR register

	USIU.SCCR.B.MFPDL = 0; // Unlock MF and DIVF fields (Writable)
	USIU.SCCR.B.RTDIV = 0; // Clock for PIT/ RT devided by 4
	USIU.SCCR.B.RTSEL = 0; // OSCM (NOT extclock) chosen
	
	//Sets some system values to set up.
	//Low power mode disables fast clock so keep it off.
	USIU.PLPRCR.B.CSRC = 0; 	// Normal clock source
	USIU.PLPRCR.B.LPM = 0; 		// Normal power mode (not low power)
	USIU.PLPRCR.B.DIVF = 0x0; 	// Divide system clock by 2 (Minimum divid)

	
	//Divides the system clock by 2 (Smallest possible)
	//And enables the clock into the PWM module.   
	MIOS1.MCPSMSCR.B.PSL = 0x2;
	MIOS1.MCPSMSCR.B.PREN = 1;

	init_pwm_turntable();
	init_pwm_wheels();
	//init_pwm_sensor();
	init_pwm_timers();
}

/*
This sets up the wheel portion of the pwm.for PIN 19 and 2
*/
void init_pwm_wheels()
{
	
	//This sets up the PWM prescaler
	//Remember:  This register requires the 2's complement
	//	Of the prescaler you want.  "200" to Hundreths of a millisec.
	MIOS1.MPWMSM19SCR.B.CP = 0x38; 
	MIOS1.MPWMSM2SCR.B.CP = 0x38; 
	
	//The following sets the period
	//Here the clock is divided to hundredths of a millisecond
	//Thus a period of "2000" clicks means a 20 mili second period 
	MIOS1.MPWMSM19PERR.R = 0x7D0; 
	MIOS1.MPWMSM2PERR.R  = 0x7D0;
	
	
	// This enables the clock to the PWM thus it starts
	MIOS1.MPWMSM19SCR.B.EN = 0;
	MIOS1.MPWMSM2SCR.B.EN = 0; 
	
	MIOS1.MPWMSM19PULR.R = 0x96; // right wheel
	MIOS1.MPWMSM2PULR.R = 0x96;  // left wheel
			
}
/*This generates the pulse for the ultrasonic sensor*/
void init_pwm_sensor()
{
	//This sets up the PWM prescaler
	//Remember:  This register requires the 2's complement
	//	Of the prescaler you want.  Divide by "200" to get 
	//  Hundreths of a millisec.
	MIOS1.MPWMSM18SCR.B.CP = 0x38; 
 
	
	//The following sets the period
	//Here the clock is divided to hundredths of a millisecond
	//Thus a period of "5000" clicks means a 50 ms period 
	MIOS1.MPWMSM18PERR.R = 0x1388; 
	//MIOS1.MPWMSM18PERR.R = 0x2710;
	
	//This sets the time high.  It is the number of "clicks" or
	//Parts of the period we set above. Stay low for 4998 clicks
	//Go high for 2 thus 20 micro seconds
	MIOS1.MPWMSM18PULR.R = 0x0; 

	
	// This enables the clock to the PWM thus it starts
	MIOS1.MPWMSM18SCR.B.EN = 1;

	
}


/*
This code sets up some pwm modules to behave as timers.  
**Remember the PWM output MUST be feed into the externalized IRQ lines (Active low)
*/
void init_pwm_timers()
{
	/*
	Sets SIEL to allow IRQ1,2,3,4 as edge level.  Does not effect
	other bits.  Then unmask the interrupt
	*/

	
	
	//This sets up the PWM prescaler
	//Remember:  This register requires the 2's complement
	//	Of the prescaler you want.  Divide by "200" to get 
	//  Hundreths of a millisec.
	MIOS1.MPWMSM1SCR.B.CP = 0x38; 
//	MIOS1.MPWMSM3SCR.B.CP = 0x38; 
	MIOS1.MPWMSM16SCR.B.CP = 0x38; 
	MIOS1.MPWMSM17SCR.B.CP = 0x38; 
	
	//The following sets the period
	//Here the clock is divided to hundredths of a millisecond
	//Thus a period of "20001" clicks means a 5 times a second 
	MIOS1.MPWMSM1PERR.R = 0x3E9;//0x1389;//0x4E21; 
//	MIOS1.MPWMSM3PERR.R = 0x4E21;
	MIOS1.MPWMSM16PERR.R = 0x1388;//EA61; 
	MIOS1.MPWMSM17PERR.R = 0x4E21;
	
	//This sets the time high.  It is the number of "clicks" or
	//Parts of the period we set above. Stay high for 20000 clicks
	//Go down for 1 o trigger interrupt
	MIOS1.MPWMSM1PULR.R = 0x85;//0x1388;//0x4E20; 
//	MIOS1.MPWMSM3PULR.R = 0x4E20; 
	MIOS1.MPWMSM16PULR.R = 0x9C4;//EA60;
	MIOS1.MPWMSM17PULR.R = 0x0;
	
	// This enables the clock to the PWM thus it starts
	MIOS1.MPWMSM1SCR.B.EN = 0;
//	MIOS1.MPWMSM3SCR.B.EN = 0; 
	MIOS1.MPWMSM16SCR.B.EN = 1;
	MIOS1.MPWMSM17SCR.B.EN = 1; 

	
}

void init_pwm_turntable()
{
	int i;
	//This sets up the PWM prescaler
	//Remember:  This register requires the 2's complement
	//	Of the prescaler you want.  Divide by "200" to get 
	//  Hundreths of a millisec.
	MIOS1.MPWMSM0SCR.B.CP = 0x38; 

	//The following sets the period
	//Here the clock is divided to hundredths of a millisecond
	//Thus a period of "2000" clicks 
	MIOS1.MPWMSM0PERR.R = 0x7D0; 

	//This sets the time high.  It is the number of "clicks" or
	//Parts of the period we set above. Stay high for 20000 clicks
	//Go down for 1 o trigger interrupt
	MIOS1.MPWMSM0PULR.R = 0x3C; 
	
	// This enables the clock to the PWM thus it starts
	MIOS1.MPWMSM0SCR.B.EN = 1;
	for(i=0; i<5000000; i++)
	{
		
	}
	//position = 0;
//	MIOS1.MPWMSM16SCR.B.EN = 1;
}
