/* 
 * File:   delay.h
 * Author: t133643
 *
 * Created on 30 de agosto de 2012, 19:20
 */

#ifndef DELAY_H
#define	DELAY_H

#include "GenericTypeDefs.h"

#define GetSystemClock()        (32000000)		// Hz
#define GetInstructionClock()	(GetSystemClock()/2)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32
#define GetPeripheralClock()	(GetSystemClock()/2)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32

void delay_10us(DWORD dwCount);
void delay_ms(WORD ms);



#endif	/* DELAY_H */

