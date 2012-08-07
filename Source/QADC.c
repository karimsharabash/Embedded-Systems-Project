#include "mpc555.h"

void init_QADC();
void init_QADC()
{
	QADC_A.QADC64MCR.B.SUPV = 0; //Turn off supervisor mode
	USIU.PDMCR.B.PRDS = 1; //disable internal pullup resister
	
	QADC_A.QACR0.B.MUX = 0x0; //ADC is internally multiplexed
	QADC_A.QACR0.B.PSH = 0xB; //Div system clock by 20
	QADC_A.QACR0.B.PSL = 0x7;
	
	QADC_A.CCW[0].B.P = 0; //does not pause between CCW
	QADC_A.CCW[0].B.BYP = 0; //does not bypass amplifier
	QADC_A.CCW[0].B.IST = 0x11; //set to highest sampling time for
							// accuracy
							
	QADC_A.CCW[1].B.P = 0; //does not pause between CCW
	QADC_A.CCW[1].B.BYP = 0; //does not bypass amplifier
	QADC_A.CCW[1].B.IST = 0x11; //set to highest sampling time for
							// accuracy
							
	QADC_A.CCW[0].B.CHAN = 0; //Set channel 0 to pin AN0
	QADC_A.CCW[1].B.CHAN = 1; //Set channel 1 to pin AN1
	QADC_A.CCW[2].B.CHAN = 63; //End of queue code
	QADC_A.QACR1.B.MQ1 = 0x11;
}
