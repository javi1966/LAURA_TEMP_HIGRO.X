#include "displayPWM.h"

//Pines Display
/*
Top Row
1,a,f,2,3,b
Bottom Row
e,d,dp,c,g,4
 */
//**** Variables Globales
BYTE numDigits[4];
BYTE numSeg[8];
BOOL leds[4][8];
BYTE nums[4];
int numero;
int iValorPWM0;
int iValorPWM1;
int iValorPWM2;
int iValorPWM3;


//***********************************************************************

void initDisplayPWM() {

    TRISB = 0x0000;

    numDigits[0] = 12; //MSB
    numDigits[1] = 13;
    numDigits[2] = 14;
    numDigits[3] = 15; //LSB
    numSeg[0] = 2; //a...f,DP
    numSeg[1] = 3;
    numSeg[2] = 4;
    numSeg[3] = 5;
    numSeg[4] = 6;
    numSeg[5] = 7;
    numSeg[6] = 10;
    numSeg[7] = 11; //DP


    LATB |= _BV(4);
    LATB |= _BV(3);
    LATB |= _BV(2);
    LATB |= _BV(7);
    LATB |= _BV(8);
    LATB |= _BV(9);
    LATB |= _BV(10);
    LATB |= _BV(11);




    __builtin_write_OSCCONL(OSCCON & ~(1 << 6));

    RPOR7bits.RP15R = 0b10010; //OC1...OC4
    RPOR7bits.RP14R = 0b10011;
    RPOR6bits.RP13R = 0b10100;
    RPOR6bits.RP12R = 0b10101;

    __builtin_write_OSCCONL(OSCCON | (1 << 6));



    //PWM1 -> RB15
    OC1CONbits.OCM = 0x0000;
    OC1R = 100; // Write the duty cycle for the first PWM pulse
    OC1RS = 200; // Write the duty cycle for the second PWM pulse
    OC1CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
    OC1R = 100; // Load the Compare Register Value
    OC1CONbits.OCM = 0b110; // Select the Output Compare mode

    //PWM2 -> RB14
    OC2CONbits.OCM = 0x0000;
    OC2R = 100; // Write the duty cycle for the first PWM pulse
    OC2RS = 200; // Write the duty cycle for the second PWM pulse
    OC2CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
    OC2R = 100; // Load the Compare Register Value
    OC2CONbits.OCM = 0b110; // Select the Output Compare mode

    //PWM3 -> RB13
    OC3CONbits.OCM = 0x0000;
    OC3R = 100; // Write the duty cycle for the first PWM pulse
    OC3RS = 200; // Write the duty cycle for the second PWM pulse
    OC3CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
    OC3R = 100; // Load the Compare Register Value
    OC3CONbits.OCM = 0b110; // Select the Output Compare mode

    //PWM4 -> RB12
    OC4CONbits.OCM = 0x0000;
    OC4R = 100; // Write the duty cycle for the first PWM pulse
    OC4RS = 200; // Write the duty cycle for the second PWM pulse
    OC4CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
    OC4R = 100; // Load the Compare Register Value
    OC4CONbits.OCM = 0b110; // Select the Output Compare mode

    //Configura TIMER2
    // Initialize and enable Timer2
    T2CONbits.TON = 0; // Disable Timer
    T2CONbits.TCS = 0; // Select internal instruction cycle clock
    T2CONbits.TGATE = 0; // Disable Gated Timer mode
    T2CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
    TMR2 = 0x00; // Clear timer register
    PR2 = PWM_PERIODO; // Load the period value

    IPC1bits.T2IP = 0x01; // Set Timer 2 Interrupt Priority Level
    IFS0bits.T2IF = 0; // Clear Timer 2 Interrupt Flag
    IEC0bits.T2IE = 0; // Enable Timer 2 interrupt
    T2CONbits.TON = 1; // Start Time

    setValorPWM(0, 0);
    setValorPWM(1, 0);
    setValorPWM(2, 0);
    setValorPWM(3, 0);
}

//****************************************************************

void disNum(int number, BYTE DecPlace) {

    BOOL negative;

    if (number < 0) {
        negative = 1;
        number = number*-1;
    } else {
        negative = 0;
    }

    //If the number is out of range, just display '----'
    if ((negative == 0 && number > 9999) || (negative == 1 && number > 999)) {
        nums[0] = 21;
        nums[1] = 21;
        nums[2] = 21;
        nums[3] = 21;
    } else {
        //Find the four digits
        int total = number;
        if (negative == 0) {
            nums[0] = number / 1000;
            total = total - nums[0]*1000;
        } else {
            nums[0] = 21;
        }
        nums[1] = total / 100;
        total = total - nums[1]*100;
        nums[2] = total / 10;
        nums[3] = total - nums[2]*10;


        //If there are zeros, set them to 20 which means a blank
        //However, don't cut out significant zeros
        if (negative == 0) {
            if (nums[0] == 0 && DecPlace < 3) {
                nums[0] = 20;
                if (nums[1] == 0 && DecPlace < 2) {
                    nums[1] = 20;
                    if (nums[2] == 0 && DecPlace == 0) {
                        nums[2] = 20;
                    }
                }
            }
        } else {
            if (nums[1] == 0 && DecPlace < 2) {
                nums[1] = 20;
                if (nums[2] == 0 && DecPlace == 0) {
                    nums[2] = 20;
                }
            }
        }
    }


}
//***********************************************************************

void disAlpha(char *str) {


    nums[0] = str[0] - 0x30;
    nums[1] = str[1] - 0x30;
    nums[2] = str[2] - 0x30;
    nums[3] = str[3] - 0x30;

}

//***********************************************************************

void creaArray(char *buf,BYTE DecPlace) {

    BYTE digit;
    BYTE aux;

    for (digit = 0; digit < 4; digit++) {
        aux=buf[digit + 4 - 4];
        switch (aux) {
            case 0x30:
                leds[digit][0] = 1;
                leds[digit][1] = 1;
                leds[digit][2] = 1;
                leds[digit][3] = 1;
                leds[digit][4] = 1;
                leds[digit][5] = 1;
                leds[digit][6] = 0;
                break;

            case 0x31:
                leds[digit][0] = 0;
                leds[digit][1] = 1;
                leds[digit][2] = 1;
                leds[digit][3] = 0;
                leds[digit][4] = 0;
                leds[digit][5] = 0;
                leds[digit][6] = 0;
                break;

            case 0x32:
                leds[digit][0] = 1;
                leds[digit][1] = 1;
                leds[digit][2] = 0;
                leds[digit][3] = 1;
                leds[digit][4] = 1;
                leds[digit][5] = 0;
                leds[digit][6] = 1;
                break;

            case 0x33:
                leds[digit][0] = 1;
                leds[digit][1] = 1;
                leds[digit][2] = 1;
                leds[digit][3] = 1;
                leds[digit][4] = 0;
                leds[digit][5] = 0;
                leds[digit][6] = 1;
                break;

            case 0x34:
                leds[digit][0] = 0;
                leds[digit][1] = 1;
                leds[digit][2] = 1;
                leds[digit][3] = 0;
                leds[digit][4] = 0;
                leds[digit][5] = 1;
                leds[digit][6] = 1;
                break;

            case 0x35:
                leds[digit][0] = 1;
                leds[digit][1] = 0;
                leds[digit][2] = 1;
                leds[digit][3] = 1;
                leds[digit][4] = 0;
                leds[digit][5] = 1;
                leds[digit][6] = 1;
                break;

            case 0x36:
                leds[digit][0] = 1;
                leds[digit][1] = 0;
                leds[digit][2] = 1;
                leds[digit][3] = 1;
                leds[digit][4] = 1;
                leds[digit][5] = 1;
                leds[digit][6] = 1;
                break;

            case 0x37:
                leds[digit][0] = 1;
                leds[digit][1] = 1;
                leds[digit][2] = 1;
                leds[digit][3] = 0;
                leds[digit][4] = 0;
                leds[digit][5] = 0;
                leds[digit][6] = 0;
                break;

            case 0x38:
                leds[digit][0] = 1;
                leds[digit][1] = 1;
                leds[digit][2] = 1;
                leds[digit][3] = 1;
                leds[digit][4] = 1;
                leds[digit][5] = 1;
                leds[digit][6] = 1;
                break;

            case 0x39:
                leds[digit][0] = 1;
                leds[digit][1] = 1;
                leds[digit][2] = 1;
                leds[digit][3] = 1;
                leds[digit][4] = 0;
                leds[digit][5] = 1;
                leds[digit][6] = 1;
                break;

            case 0x20:                  //' '
                leds[digit][0] = 0;
                leds[digit][1] = 0;
                leds[digit][2] = 0;
                leds[digit][3] = 0;
                leds[digit][4] = 0;
                leds[digit][5] = 0;
                leds[digit][6] = 0;
                break;
            case 0x21:
                leds[digit][0] = 0;
                leds[digit][1] = 0;
                leds[digit][2] = 0;
                leds[digit][3] = 0;
                leds[digit][4] = 0;
                leds[digit][5] = 0;
                leds[digit][6] = 1;
                break;
            case 0x48:                    //"H"
                leds[digit][0] = 0;
                leds[digit][1] = 1;
                leds[digit][2] = 1;
                leds[digit][3] = 0;
                leds[digit][4] = 1;
                leds[digit][5] = 1;
                leds[digit][6] = 1;
                break;
            case 0x13:
                leds[digit][0] = 1;
                leds[digit][1] = 0;
                leds[digit][2] = 0;
                leds[digit][3] = 1;
                leds[digit][4] = 1;
                leds[digit][5] = 1;
                leds[digit][6] = 0;
                break;
            case 0x1C:
                leds[digit][0] = 0;
                leds[digit][1] = 0;
                leds[digit][2] = 0;
                leds[digit][3] = 1;
                leds[digit][4] = 1;
                leds[digit][5] = 1;
                leds[digit][6] = 0;
                break;

            case 0xBA:                 //"º"
                leds[digit][0] = 1;
                leds[digit][1] = 1;
                leds[digit][2] = 0;
                leds[digit][3] = 0;
                leds[digit][4] = 0;
                leds[digit][5] = 1;
                leds[digit][6] = 1;
                break;

            case 0x2D:                  //"-"
                leds[digit][0] = 0;
                leds[digit][1] = 0;
                leds[digit][2] = 0;
                leds[digit][3] = 0;
                leds[digit][4] = 0;
                leds[digit][5] = 0;
                leds[digit][6] = 1;
                break;


            default:
                leds[digit][0] = 0;
                leds[digit][1] = 0;
                leds[digit][2] = 1;
                leds[digit][3] = 1;
                leds[digit][4] = 1;
                leds[digit][5] = 0;
                leds[digit][6] = 1;
                break;
        }

        //Set the decimal place leds
        if (-1 - digit + 4 == DecPlace) {
            leds[digit][7] = 1;
        } else {
            leds[digit][7] = 0;
        }
    }
}




//************************************************************************

void printDisplayPWMnum(int number, BYTE DecPlace) {


    BYTE digit;
    BYTE seg;

    disNum(number, DecPlace);
   // creaArray(DecPlace);



    //  int WaitTime = map(brightness,0,100,1,2000); //Calculate the delay once
    for (digit = 0; digit < 4; digit++) {
        //Turn the relevant segment on
        // digitalWrite(numSeg[digit], 1);
        //PORTB |= _BV(numDigits[digit]);
        switch (digit) {
            case 0:
                CIFRA0(iValorPWM0);
                break;
            case 1:
                CIFRA1(iValorPWM1);
                break;
            case 2:
                CIFRA2(iValorPWM2);
                break;
            case 3:
                CIFRA3(iValorPWM3);
                break;
            default:
                break;

        }

        // LATB &= ~_BV(numDigits[digit]);


        //For each digit, turn relevant digits on
        for (seg = 0; seg < 8; seg++) {
            if (leds[digit][seg] == 1) {
                LATB &= ~_BV(numSeg[seg]);
            }
            //delay(200); //Uncomment this to see it in slow motion

        }
        // delay_10us(10);
        //Turn all digits OFF
        for (seg = 0; seg < 8; seg++) {
            LATB |= _BV(numSeg[seg]);

        }

        switch (digit) {
            case 0:
                CIFRA0_OFF();
                break;
            case 1:
                CIFRA1_OFF();
                break;
            case 2:
                CIFRA2_OFF();
                break;
            case 3:
                CIFRA3_OFF();
                break;
            default:
                break;

        }

        //Turn the relevant segment OFF
        // LATB |= _BV(numDigits[digit]);
    }
}

//************************************************************************

void printDisplayPWMtxt(char *txt,int posDP) {


    BYTE digit;
    BYTE seg;

  
    creaArray(txt,posDP);   //DP en 25.4ªc,hora HH.MM



    //  int WaitTime = map(brightness,0,100,1,2000); //Calculate the delay once
    for (digit = 0; digit < 4; digit++) {
        //Turn the relevant segment on
        // digitalWrite(numSeg[digit], 1);
        //PORTB |= _BV(numDigits[digit]);
        switch (digit) {
            case 0:
                CIFRA0(iValorPWM0);
                break;
            case 1:
                CIFRA1(iValorPWM1);
                break;
            case 2:
                CIFRA2(iValorPWM2);
                break;
            case 3:
                CIFRA3(iValorPWM3);
                break;
            default:
                break;

        }

        // LATB &= ~_BV(numDigits[digit]);


        //For each digit, turn relevant digits on
        for (seg = 0; seg < 8; seg++) {
            if (leds[digit][seg] == 1) {
                LATB &= ~_BV(numSeg[seg]);
            }
            //delay(200); //Uncomment this to see it in slow motion

        }
        // delay_10us(10);
        //Turn all digits OFF
        for (seg = 0; seg < 8; seg++) {
            LATB |= _BV(numSeg[seg]);

        }

        switch (digit) {
            case 0:
                CIFRA0_OFF();
                break;
            case 1:
                CIFRA1_OFF();
                break;
            case 2:
                CIFRA2_OFF();
                break;
            case 3:
                CIFRA3_OFF();
                break;
            default:
                break;

        }

        //Turn the relevant segment OFF
        // LATB |= _BV(numDigits[digit]);
    }
}