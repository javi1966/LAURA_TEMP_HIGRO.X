/* 
 * File:   i2c_HAL.h
 * Author: Javier
 *
 * Created on 14 de septiembre de 2013, 19:36
 */

#ifndef I2C_HAL_H
#define	I2C_HAL_H

void i2c_init(void);
void i2c_start(void); // I2C   START command
void i2c_restart(void); // I2C RESTART command (for reads)
void i2c_stop(void); // I2C    STOP command
void i2c_wr(unsigned char i2c_data); // I2C WRITE BYTE
unsigned char i2c_rd(void); // I2C READ BYTE (returns 'rtcc_buf')
void i2c_ack(void); // MASTER ACK
void i2c_nack(void); // MASTER NOACK
unsigned char i2c_ackStatus(void);
void i2c_idle(void);
#endif	/* I2C_HAL_H */

