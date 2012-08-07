/****************************************************************************/
/*

FILE
	
	56X_Chip_init.c
	
DESCRIPTION

	This is the chip initialization file for the MPC56X series
*/
/****************************************************************************/

#pragma section code_type ".init"

#define SIUMCR				0x2fc000
#define SYPCR				0x2fc004
#define SIPEND				0x2fc010
#define SIMASK				0x2fc014
#define SIEL				0x2fc018
#define SIVEC				0x2fc01c
#define PLPRCR				0x2fc284

//SPR defines
#define SPR_SRR1			27
#define SPR_ICTRL           158
#define SPR_IMMR			638
#define SPR_BBCMCR			560			

#ifdef __cplusplus
extern "C" {
#endif

asm void __reset(void);
asm void usr_init();
extern void __start();

#ifdef __cplusplus
}
#endif

asm void __reset(void)
{
	//
	//   Enable machine check exceptions
	//
		lis		r3, 0
		ori		r3, r3, 0x1002
		mtmsr	r3
		
	//
	//	ROM is not relocated, so nothing left to do here.
	//

		b		__start
}

asm void usr_init()
{
		nofralloc
		
// Adr 0x002fc384 = 0x55ccaa33  PLPRCRK: open key
		lis			r5, 0x55cc
		ori			r5, r5, 0xaa33
		lis			r4, 0x002fc384@ha
		stw			r5, 0x002fc384@l(r4)

// Adr 0x002fc284 = 0x00900000  PLPRCR
		lis			r5, 0x0090
		ori			r5, r5, 0x0000
		lis			r4, 0x002fc284@ha
		stw			r5, 0x002fc284@l(r4)

// SPR MSR = 0x00003002  MSR
		lis			r5, 0x0000
		ori			r5, r5, 0x3002
		mtmsr			r5

// SPR 27 = 0x00003002  SSR1
		lis			r5, 0x0000
		ori			r5, r5, 0x3002
		mtspr		27, r5

// SPR 638 = 0xfff00800  IMMR
		lis			r5, 0xfff0
		ori			r5, r5, 0x0800
		mtspr		638, r5

// Adr 0x002fc000 = 0x00000000  SIUMCR
		lis			r5, 0x0000
		ori			r5, r5, 0x0000
		lis			r4, 0x002fc000@ha
		stw			r5, 0x002fc000@l(r4)

// Adr 0x002fc004 = 0x0000ff88  SYPCR
		lis			r5, 0x0000
		ori			r5, r5, 0xff88
		lis			r4, 0x002fc004@ha
		stw			r5, 0x002fc004@l(r4)

// SPR 158 = (SPR 158 & 0xfffffff0) | 0x00000007  ICTRL: switch serialised mode off
		lis			r5, 0xffff
		ori			r5, r5, 0xfff0
		mfspr		r4, 158
		and			r4, r4, r5
		lis			r5, 0x0000
		ori		 	r5, r5, 0x0007
		or			r4, r4, r5
		mtspr		158, r4

// Adr 0x002fc380 = 0x55ccaa33  SCCRK: open key
		lis			r5, 0x55cc
		ori			r5, r5, 0xaa33
		lis			r4, 0x002fc380@ha
		stw			r5, 0x002fc380@l(r4)

// Adr 0x002fc280 = 0x00010000  SCCR
		lis			r5, 0x0001
		ori			r5, r5, 0x0000
		lis			r4, 0x002fc280@ha
		stw			r5, 0x002fc280@l(r4)

// Adr 0x00305014 = 0x0000  PORTQS
		li			r5, 0x0000
		lis			r4, 0x00305014@ha
		sth			r5, 0x00305014@l(r4)

// Adr 0x00305016 = 0x0000  PQSPAR/DDRQS
		li			r5, 0x0000
		lis			r4, 0x00305016@ha
		sth			r5, 0x00305016@l(r4)

// Adr 0x00307f80 = 0x00000000  UMCR
		lis			r5, 0x0000
		ori			r5, r5, 0x0000
		lis			r4, 0x00307f80@ha
		stw			r5, 0x00307f80@l(r4)


// Power PC Clock 
// Oscillator Clock OSCCLK: 4000000
// SCCR: 00010000
// PLPRCR: 00900000

// TBS: 0
// PRQEN: 0
// DFNL: 0
// DFNH: 0
// MF: 9
// CSRC: 0
// LPM: 0
// DIVF: 0

// shift right: 0
// SPLL input: 4000000
// SPLL output: 80000000
// General System Clock GLCK2 frequency: 40000000
// Time Base Clock divisor: 4
// Time Base Clock TBMCLK frequency: 1000000

		blr
}
