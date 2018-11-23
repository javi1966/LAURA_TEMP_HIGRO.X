#include "rtcc_MCP79410.h"


 // time variables, BCD format
  BYTE day;


BYTE rtcc_wr(unsigned char time_var, unsigned char rtcc_reg)
// writes a constant/variable in a RTCC register
{
    BYTE ErrorCode;

     i2c_idle();
     i2c_start(); // start I2C communication
 
     i2c_wr(ADDR_RTCC_WRITE); // write DEVICE ADDR for RTCC WRITES
     i2c_idle();
     ErrorCode|=i2c_ackStatus();

     i2c_wr(rtcc_reg); // write the register's ADDRESS
     i2c_idle();
     ErrorCode|=i2c_ackStatus();

   
     i2c_wr(time_var); // write byte variable in the register
     i2c_idle();
     i2c_stop(); // stop I2C communication
     delay_10us(4);  //aprox 39us

    return ErrorCode;
} // free time STOP - START
//.....................................................................................

BYTE rtcc_rd(BYTE rtcc_reg) // reads a register of the RTCC
{
    BYTE buf;

    i2c_idle();
    i2c_start(); // start I2C communication
   
    i2c_wr(ADDR_RTCC_WRITE); // write DEVICE ADDR for RTCC WRITES
    i2c_idle();
    i2c_wr(rtcc_reg); // write the register ADDRESS
    i2c_idle();
    i2c_restart(); // RESTART for READS
   
    i2c_wr(ADDR_RTCC_READ); // send the DEVICE ADDRESS for RTCC READS.
    i2c_idle();
    
    buf= i2c_rd(); // read register (stored in 'rtcc_buf')
    i2c_nack(); // NOACK from MASTER (last read byte)
    i2c_stop(); // stop I2C communication
    delay_10us(4); // free time STOP - START
    return buf;
} // store the read byte in 'time_var'
//...................................................................................

void ini_rtcc(void) // initialization of the RTCC(MCP79410)
{ // START bit is located in the sec register
    day = rtcc_rd(ADDR_DAY); // read day + OSCON bit
    // keep the previous value of 'seconds'
    rtcc_wr(SQWE + ALM_NO + MFP_01H, ADDR_CTRL); // square vawe on MFP, no alarms, MFP = 1Hz
    // write this init constant in CONTROL REG
    // final settings are: // crystal = started,
    // 24H format,square vawe on MFP,no alarms, MFP=1Hz
    rtcc_wr(day | VBATEN, ADDR_DAY);
} // enable the battery back-up
//.........................................................................................

void ini_time(void) // initialization of time/date:2010_03_01 , 9am.
{ // it initializes also :24H format, and START OSC.
    if ((day & OSCON) == OSCON) {
        ;
    }// if oscillator = already running, do nothing.
    else { // if oscillator = not running, set time/date(arbitrary)
        // and SART oscillator/ crystal
        rtcc_wr(0x10, ADDR_YEAR); // initialize YEAR  register
        rtcc_wr(0x03, ADDR_MNTH); // initialize MONTH register
        rtcc_wr(0x01, ADDR_DATE); // initialize DATE  register
        rtcc_wr(0x00, ADDR_HOUR); // initialize HOUR  register
        rtcc_wr(0x00, ADDR_MIN); // initialize MIN   register
        rtcc_wr(0x30 | START_32KHZ, ADDR_SEC);
    } //init SEC   register, set START bit
}



