#include <p33FJ128GP802.h>
#include "GenericTypeDefs.h"
#include "i2c_HAL.h"

//******************************************************************************

void i2c_init(void) {

    I2C1BRG = 0x4F; //aprox. 100khz@32M
    I2C1CON = 0x1200; //0x0020;
    I2C1RCV = 0x0000;
    I2C1TRN = 0x0000;
    I2C1STAT = 0x0000;


    I2C1CON = 0x9200; //0x8020;


}

//***************************************************************************

void i2c_start() {

    //  I2C1CONbits.I2CEN = 0;
    //  I2C1CONbits.ACKDT = 0;
    //  I2C1CONbits.I2CEN = 1;

    I2C1CONbits.SEN = 1;
    while (I2C1CONbits.SEN);


}
//***************************************************************************

void i2c_restart(void) {

    I2C1CONbits.RSEN = 1;
    while (I2C1CONbits.RSEN);

}
//*************************************************************************

void i2c_stop() {

    I2C1CONbits.PEN = 1;
    while (I2C1CONbits.PEN);
    

}

//*************************************************************************

void i2c_wr(unsigned char i2c_data) {


    while (I2C1STATbits.TRSTAT);
    I2C1TRN = i2c_data; //0x81
    while (I2C1STATbits.TBF);

    // while (I2C1STATbits.ACKSTAT);
}

//************************************************************************

BYTE i2c_rd() {



    //receiver enabled
    I2C1CONbits.RCEN = 1;
    //  while (I2C1CONbits.RCEN);

    // I2C1STATbits.I2COV = 0;
    Nop();
    while (!I2C1STATbits.RBF);

    return I2C1RCV;
}
//****************************************************************************

void i2c_ack() {

    /*send ack*/
    I2C1CONbits.ACKDT = 0;
    I2C1CONbits.ACKEN = 1;
    while (I2C1CONbits.ACKEN);
}
//***************************************************************************

BYTE i2c_ackStatus() {

    return (!I2C1STATbits.ACKSTAT);
}
//******************************************************************************

void i2c_nack() {
    /*configure i2c to send nack*/
    I2C1CONbits.ACKDT = 1;
    /*send nack*/
    I2C1CONbits.ACKEN = 1;
    while (I2C1CONbits.ACKEN);
    I2C1CONbits.ACKDT = 0;
}

//*****************************************************************************

void i2c_idle() {

    while (I2C1STATbits.TRSTAT);

}

