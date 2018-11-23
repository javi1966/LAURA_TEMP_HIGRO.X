/* 
 * File:   sht21.h
 * Author: t133643
 *
 * Created on 3 de septiembre de 2013, 19:04
 */

#ifndef SHT21_H
#define	SHT21_H

#include <p33FJ128GP802.h>
#include "GenericTypeDefs.h"
#include "i2c_HAL.h"
#include "delay.h"


#define WRITE       0x80
#define READ        0x81
#define TEMPERATURA 0xE3
#define HUMEDAD     0xE5
#define SOFTRESET   0xFE


WORD readSHT21(BYTE);
float readTemperatura();
float readHumedad();
void read_SHT21(WORD *Temperatura, BYTE *humedad);

#endif	/* SHT21_H */

