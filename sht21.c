#include "sht21.h"



//**********************************************************************

WORD readSHT21(BYTE cmd) {

    BYTE msb = 0;
    BYTE lsb = 0;
    WORD temp_num = 0;
   
    
  

        /*
        //start
        I2C1CONbits.SEN = 1;
        while (I2C1CONbits.SEN);*/

    i2c_start();

    /*

    //sensor address
    I2C1TRN = 0x80;
    while (I2C1STATbits.TBF);
    while(I2C1STATbits.TRSTAT);
    while (I2C1STATbits.ACKSTAT);*/

    i2c_wr(0x80);
    i2c_idle();

    /*
    //sensor command
    I2C1TRN = cmd;
    while (I2C1STATbits.TBF);
    while(I2C1STATbits.TRSTAT);
    while (I2C1STATbits.ACKSTAT);*/

    i2c_wr(cmd);
    i2c_idle();
    /*
   //repeated start
   I2C1CONbits.RSEN = 1;
   while (I2C1CONbits.RSEN);*/

    i2c_restart();


    /*
    //address for reading
    I2C1TRN = 0x81;
    while (I2C1STATbits.TBF);
    while(I2C1STATbits.TRSTAT); 
    while (I2C1STATbits.ACKSTAT);*/

    i2c_wr(0x81);
    i2c_idle();
    /*
    //receiver enabled
    I2C1CONbits.RCEN = 1;
    while (I2C1CONbits.RCEN);



    //read first byte data
    //while(!I2C1STATbits.RBF);   -----con questo si blocca
    I2C1STATbits.I2COV = 0;
    msb = I2C1RCV;
    while (I2C1STATbits.RBF);*/

    delay_ms(1000);

    msb = i2c_rd();

    /*
    //send ack
    I2C1CONbits.ACKDT = 0;
    I2C1CONbits.ACKEN = 1;*/
    i2c_ack();

    /*
   //receiver enabled
   I2C1CONbits.RCEN = 1;
   while (I2C1CONbits.RCEN);

   //read second byte data
   I2C1STATbits.I2COV = 0;
   lsb = I2C1RCV;
   while(I2C1STATbits.RBF);*/

    lsb = i2c_rd();

    /*
    //configure i2c to send nack
    I2C1CONbits.ACKDT = 1;
    //send nack
    I2C1CONbits.ACKEN = 1;*/

    i2c_nack();

    /*
    //send stop
    I2C1CONbits.PEN = 1;
    while (I2C1CONbits.PEN);
    I2C1CONbits.I2CEN = 0;*/

    i2c_stop();
    delay_10us(4);
    temp_num = ((unsigned int) msb << 8) | lsb;
    return temp_num;
}

//**************************************************************************

float readTemperatura() {

    WORD temp;
    float tmp;

    temp = readSHT21(TEMPERATURA);
    tmp = (175.72 * ((float) temp / 0xFFFF)) - 46.85;

    return tmp;
}
//*************************************************************************

float readHumedad() {

    WORD temp;
    float tmp;

    temp = readSHT21(HUMEDAD);
    tmp = (-6 + (125 * ((float) temp / 0xFFFF)));

    return tmp;
}

//***************************************************************************

void read_SHT21(WORD *Temperatura, BYTE *Humedad) {

    BYTE buf[4];
    DWORD val;

    i2c_start();

    i2c_wr(WRITE);
    i2c_idle();

    // Temperatura *****************************************
    i2c_wr(TEMPERATURA);
    i2c_idle();
    i2c_restart();
    i2c_wr(READ);
    i2c_idle();
    delay_ms(200);

    buf[0] = i2c_rd();
    i2c_ack();
    buf[1] = i2c_rd();
    i2c_nack();

    val = buf[0];
    val <<= 8;
    val += buf[1];
    val &= 0xFFFC;

    *Temperatura = ((val * 512) / 9548);
    *Temperatura = ((*Temperatura) - 937) / 2;

    //Humedad ***************************************************
    i2c_wr(HUMEDAD);
    i2c_idle();
    i2c_restart();
    i2c_wr(READ);
    i2c_idle();
    delay_ms(60);

    buf[0] = i2c_rd();
    i2c_ack();
    buf[1] = i2c_rd();
    i2c_nack();

    val = buf[0];
    val <<= 8;
    val += buf[1];
    val &= 0xFFFC;

    //   T = -6 + 125* Srh/65535
    //	 T = -6 + Srh / 524,28
    //   T = -6 + (Srh * 256) / 134215      |  *256	 wegen Numerik erweitern

    val = ((val * 256) / 134215) - 6;
    *Humedad = val;




    i2c_stop();
    delay_10us(4);


}
