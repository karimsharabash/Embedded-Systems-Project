/*
#######################################
#FILE
#	$RCSfile: interrupt.c,v $
#	$Date: 2005/03/21 08:22:04 $
#	$Revision: 1.2 $
#DESCRIPTION
#	interrupt handler
#COPYRIGHT	
#	(c) 2003 Metrowerks Corporation
#	All rights reserved.
#HISTORY
#   $Log: interrupt.c,v $
#   Revision 1.2  2005/03/21 08:22:04  milies
#   save/restore MSR bits
#
#   Revision 1.4  2004/05/27 14:42:41  milies
#   added sync after setting MSR;
#   enable block address translation in MSR if debugging on Cache Debug Version
#
#   Revision 1.3  2003/10/21 12:27:37  milies
#   remove warning message
#   
########################################
*/
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void InterruptHandler(long cause);

#ifdef __cplusplus
}
#endif

void InterruptHandler(long cause)
{
	register long aMSR;
	#pragma unused (aMSR) /* Do not warn - aMSR used in asm code */

   /* Added by Kobler 2003.05.02:
	* Because we will call printf, which is part of the MSL_C library, we have to
 	* enable the FP bit in the MSR, when the MSL_C library has been built
 	* with Hardware  floating point enabled in the compiler
 	* otherwise this would lead to some exceptions
 	*/	
	asm{	
   /*
	* If we are using Hardware  floating point we need to make sure
 	* to enable the FP bit in the  MSR
 	*/	

#if __option(floatingpoint)==1 &&  __option(sfp_emulation)==0
 	mfmsr aMSR
 	ori  aMSR,aMSR,0x00002000
 	mtmsr aMSR
 	sync
#endif


#ifdef CACHE_VERSION
 	mfmsr aMSR
 	ori  aMSR,aMSR,0x00000030
 	mtmsr aMSR
 	sync
#endif
	}
	
	printf("InterruptHandler: 0x%x exception.\r\n", cause);
}