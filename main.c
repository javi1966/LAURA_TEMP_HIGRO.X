/* 
 * File:   main.c
 * Author: t133643
 *
 * Created on 27 de agosto de 2013, 18:58
 */

#include <p33FJ128GP802.h>
#include <stdio.h>
#include "GenericTypeDefs.h"
#include "delay.h"
#include "displayPWM.h"
#include "i2c_HAL.h"
#include "ds3231.h"
#include "sht21.h"

_FOSCSEL(FNOSC_FRCPLL) //Oscilador Fast RC 8Mhz
_FOSC(IOL1WAY_OFF & OSCIOFNC_OFF & POSCMD_HS)
_FWDT(FWDTEN_OFF)
_FICD(ICS_PGD1 & JTAGEN_OFF)


#define GetSystemClock()        (32000000)		// Hz
#define GetInstructionClock()	(GetSystemClock()/2)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32
#define GetPeripheralClock()	(GetSystemClock()/2)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32

#define  DISABLE_INT   IEC0bits.T1IE = 0;
#define  ENABLE_INT    IEC0bits.T1IE = 1;


#define  TRIS_KEYB1   TRISAbits.TRISA0
#define  TRIS_KEYB2   TRISAbits.TRISA1
#define  KEYB1        PORTAbits.RA0        // switch1 on RB0
#define  KEYB2        PORTAbits.RA1        // switch2 on RA5

#define  KEYB_MENU 0x01          // code for the MENU KEY (enter,exit,change cursor)
#define  KEYB_INCR 0x02          // code for the INCR KEY (increments values)

extern int iValorPWM0;
extern int iValorPWM1;
extern int iValorPWM2;
extern int iValorPWM3;


int i = 0;
BOOL bFlag1seg = FALSE;
BOOL bFlag5min = FALSE;
BOOL bFlag500ms = FALSE;
int numHumTemHor = 0;
BOOL bUpDown = TRUE;
BOOL bEstado = FALSE;
BOOL bDP = FALSE;
unsigned char hr, min, sec;
unsigned char buf_hora[5] = "8888";
char bufferTemperatura[5];
char bufferHumedad[5];

//*****************************************************************
void lee_SHT21(void);
void displayHora(void);
BYTE keyb_press(void);
void incr_min(void);
void incr_hr(void);

//*****************************************************************

int main(void) {

    int j = 0;

    TRIS_KEYB1 = 1;
    TRIS_KEYB2 = 1;
    _PCFG0 = 1;
    _PCFG1 = 1;

    // ********* Use Timer 1 for 16-bit and 32-bit processors
    // 1:256 prescale
    T1CONbits.TCKPS = 3;
    PR1 = 625; //aprox 10ms
    TMR1 = 0;

    // Enable timer interrupt

    IPC0bits.T1IP = 7; // Interrupt priority 2 (low)
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    T1CONbits.TON = 1;


    initDisplayPWM();
    i2c_init();
    
    delay_ms(1000);

    initDS3231();

    
    // ini_rtcc(); //  initialize MCP79410(enable VBAT).
    // ini_time();

    lee_SHT21();
    
    
   
    delay_ms(1000);
    
    while (TRUE) {

        //******************************************************

        if (!KEYB1) {
            //DISABLE_INT
            while (!KEYB1) {
                DS3231_AddData(HOURS);
                delay_ms(300);
            }
            //rtcc_wr(hr,ADDR_HOUR);
            //DS3231_SetInfo(HOURS, hr);
            
            numHumTemHor = 0;

            // ENABLE_INT
        }

        if (!KEYB2) {
            
           
            while (!KEYB2) {
                 DS3231_AddData(MINUTES);
                delay_ms(300);
            }

            //rtcc_wr(min,ADDR_MIN);
           
            numHumTemHor = 0;

            //ENABLE_INT
        }

        //******************************************************
        if (bFlag500ms) {

            if (!bUpDown) {
                setValorPWM(0, i + 6);
                setValorPWM(1, i + 4);
                setValorPWM(2, i + 2);
                setValorPWM(3, i);
            } else {
                setValorPWM(0, i);
                setValorPWM(1, i + 2);
                setValorPWM(2, i + 4);
                setValorPWM(3, i + 6);
            }

            bFlag500ms = FALSE;
        }

        if (bFlag1seg) {


            bDP = !bDP;
            displayHora();
            bFlag1seg = FALSE;
        }

        if (bFlag5min) {
            lee_SHT21();
            bFlag5min = FALSE;
        }



        switch (numHumTemHor) {

            case 0:if (bDP)
                    printDisplayPWMtxt(buf_hora, DPOFF);
                else
                    printDisplayPWMtxt(buf_hora, DPCENTRO);

                break;

            case 1:printDisplayPWMtxt(bufferTemperatura, DPCENTRO);
                break;

            case 2:printDisplayPWMtxt(bufferHumedad, DPOFF);
                break;
            default:
                break;
        }

    } //while

    return (1);
}

//******************************************************************

void _ISR __attribute__((__no_auto_psv__)) _T1Interrupt(void) {

    static BYTE count = 0;
    static WORD count1 = 0;
    static BYTE count2 = 0;

    if (count2++ > 50) {

        if (bUpDown) {

            i += 2;
            if (i > PWM_OFF) {
                i = PWM_OFF;
                bEstado ^= TRUE;
                if (++numHumTemHor > 2) //cambia visu. hora,temp,hu
                    numHumTemHor = 0;
                bUpDown = FALSE;
            }

        } else {
            i -= 2;

            if (i <= PWM_ON) {
                i = PWM_ON;
                bUpDown = TRUE;
            }

        }


        bFlag500ms = TRUE;

        count2 = 0;
    }

    if (count++ > 100) { //aprox 1 seg
        bFlag1seg = TRUE;

        count = 0;

    }

    if (count1++ > 32768) { //aprox 5 min.

        bFlag5min = TRUE;
        count1 = 0;
    }

    // Reset interrupt flag
    IFS0bits.T1IF = 0;
}
//*****************************************************************

void lee_SHT21() {

    WORD Temperatura;
    WORD Humedad;

    read_SHT21(&Temperatura, &Humedad);

    sprintf(bufferTemperatura, "%.1f", ((float) Temperatura) / 10);
    bufferTemperatura[2] = bufferTemperatura[3];
    bufferTemperatura[3] = 'º';
    bufferTemperatura[4] = '\0';

    sprintf(bufferHumedad, "%u", 100 -Humedad );
    bufferHumedad[2] = 'º';
    bufferHumedad[3] = 'H';
    bufferHumedad[4] = '\0';


}

//****************************************************************

void displayHora() {

    //hr = rtcc_rd(ADDR_HOUR); // read HOUR
    hr = DS3231_GetInfo(HOURS);
    //min = rtcc_rd(ADDR_MIN); // read MIN
    min = DS3231_GetInfo(MINUTES);
    //sec = rtcc_rd(ADDR_SEC); // read MIN
    sec = DS3231_GetInfo(SECONDS);
    
    
    //sprintf(buf_hora, "%02d%02d", hr,min);

    // leeDS3231();
    // buf_hora[2] = ((sec & (~START_32KHZ)) >> 4) + 0x30;
    // mask the START bit, extract MS digit by shifting
    // right 4 times,  add the ASCII digit offset
    // buf_hora[3] = (sec & 0x0f) + 0x30; // extract LS digit, add offset

    buf_hora[2] = (min >> 4) + 0x30;
    buf_hora[3] = (min & 0x0f) + 0x30;

    buf_hora[0] = (hr >> 4) + 0x30;
    if (buf_hora[0] == '0') buf_hora[0] = ' ';
    buf_hora[1] = (hr & 0x0F) + 0x30;
    buf_hora[4] = '\0';

   
}
//********************************************************************

BYTE keyb_press(void) {

    BYTE keyb_code = 0x00; // init keyboard code returned by the function
    while (!KEYB1 && !KEYB2); // wait a keyboard action(if not, loop)
    if (!KEYB1) {
        keyb_code = KEYB_MENU;
    }// if MENU KEY = pressed, update keyb code = 01
    else {
        keyb_code = KEYB_INCR;
    } // if INCR KEY : code = 02
    delay_ms(80);
    delay_ms(80); // override keyboard hazard
    while ((!KEYB1) || (!KEYB2)); // wait end of action
    return keyb_code; // returns the code only if a key was pressed

}
//********************************************************************

void incr_hr(void) // increments HOURS value
{
    if(hr++ == 0x60)
        hr=0;
    /*
    switch (hr) { // none auxiliary control/status bits
        case 0x23:
        {
            hr = 0x00;
            break;
        } // '24h' format, overflow after '23'
        case 0x19:
        {
            hr = 0x20;
            break;
        } // 19 --> 20
        case 0x09:
        {
            hr = 0x10;
            break;
        } // 09 --> 10
        default:
        {
            hr++;
            break;
        }
    } // in any other case, increment value
  */
} // display date value
//........................................................................................

void incr_min(void) // increments MINUTES ('minutes' have no aux flags)
{
    
    
    
 
    if (min > 0x59) {
        min = 0x00;
    }// overflow after 59 minutes
    else if ((min & 0x0f) == 0x09) // m9 --> (m+1)0
    {
        min = (min + 0x0f) & 0xf0;
    } else {
        min++;
    } // mm --> m(m+1)*/
    
 
    
     
} // display new MINUTES value
//********************************************************************