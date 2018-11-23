/* 
 * File:   displayPWM.h
 * Author: t133643
 *
 * Created on 1 de septiembre de 2013, 11:39
 */

#ifndef DISPLAYPWM_H
#define	DISPLAYPWM_H

//Pines Display
/*
Top Row
1,a,f,2,3,b
Bottom Row
e,d,dp,c,g,4
 */

#include <p33FJ128GP802.h>
#include "GenericTypeDefs.h"


#define DPOFF     4
#define DPCENTRO  2
#define _BV(bit) (1 << (bit))
#define PWM_PERIODO   50
#define PWM_OFF       PWM_PERIODO+1
#define PWM_ON        ! PWM_PERIODO


#define CIFRA0(valor)     OC4RS = (valor); OC3RS=PWM_OFF; OC2RS=PWM_OFF; OC1RS=PWM_OFF;
#define CIFRA1(valor)     OC4RS = PWM_OFF; OC3RS=(valor); OC2RS=PWM_OFF; OC1RS=PWM_OFF;
#define CIFRA2(valor)     OC4RS = PWM_OFF; OC3RS=PWM_OFF; OC2RS=(valor); OC1RS=PWM_OFF;
#define CIFRA3(valor)     OC4RS = PWM_OFF; OC3RS=PWM_OFF; OC2RS=PWM_OFF; OC1RS=(valor);
#define CIFRA0_OFF()      OC4RS =PWM_OFF;
#define CIFRA1_OFF()      OC3RS =PWM_OFF;
#define CIFRA2_OFF()      OC2RS =PWM_OFF;
#define CIFRA3_OFF()      OC1RS =PWM_OFF;

#define setValorPWM(npwm,valor) {            \
if ((valor) > PWM_PERIODO)                   \
        iValorPWM##npwm=PWM_PERIODO;          \
 iValorPWM##npwm=(valor);                     \
}



void initDisplayPWM();
void disNum(int number, BYTE DecPlace);
void creaArray(char *,BYTE DecPlace);
void printDisplayPWMnum(int number, BYTE DecPlace);
void disAlpha(char *str);
void printDisplayPWMtxt(char *txt,int posDP);

#endif	/* DISPLAYPWM_H */

