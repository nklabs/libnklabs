// Copyright 2020 NK Labs, LLC

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <string.h>
#include "nki2c.h"
#include "nkextrtc.h"

// DS3231 Real Time Clock with TCXO

// 32 KHz output is open drain
// INT_L / SQW output is open drain
// RST_L input has built-in pull-up

// 400 KHz I2C Capable

#define DS3231_I2C_ADDR 0x68

// DS3231 Registers
#define DS3231_REG_SECONDS 0x00 // BCD Seconds [00..59]
#define DS3231_REG_MINUTES 0x01 // BCD Minutes [00..59]
#define DS3231_REG_HOURS 0x02 // BCD Hours [00..23], Bit set for 12-hour mode
#define DS3231_REG_DAY 0x03 // Day of week [1..7]
#define DS3231_REG_DATE 0x04 // BCD  Day of month [1..31]
#define DS3231_REG_MONTH 0x05 // BCD Month [1..12], Bit 7 is century
#define DS3231_REG_YEAR 0x06 // BCD Year [00.99]

#define DS3231_REG_AL1_SECONDS 0x07 // BCD Seconds, Bit 7 = A1M1
#define DS3231_REG_AL1_MINUTES 0x08 // BCD Minutes, Bit 7 = A1M2
#define DS3231_REG_AL1_HOUR 0x09 // BCD Hours, Bit 6 = 12 hour, Bit 7 = A1M3
#define DS3231_REG_AL1_DAY 0x0A // BCD Day or Date, Bit 6 = Day, Bit 7 = A1M4

// A1M bits:
//  1111 Once per second
//  1110 When seconds match
//  1100 When minutes and seconds match
//  1000 When hours, minutes and seconds match
//  0000 When day/date, hours, minutes and seconds match

#define DS3231_REG_AL2_MINUTES 0x0B // BCD Mnutes, Bit 7 = A2M2
#define DS3231_REG_AL2_HOUR 0x0C // BCD Hours, Bit 6 = 12 hour, Bit 7 = A2M3
#define DS3231_REG_AL2_DAY 0x0D // BCD Day or DAte, Bit 6 = Day, Bit 7 = A2M4

// A2M bits:
//  111 Once per minute (seconds == 00)
//  110 When minutes match and seconds == 00
//  100 When hours, minutes match and seconds == 00
//  000 When day/date, hours, minutes match and seconds == 00

#define DS3231_REG_CONTROL 0x0E

#define DS3231_REG_CONTROL_OSC_EN_L_BIT 0x80 // Oscillator enabled on battery power when low (default = 0)
#define DS3231_REG_CONTROL_BBSQW_BIT 0x40 // Disable square wave on battery power when low (default = 0)
#define DS3231_REG_CONTROL_CONV_BIT 0x20 // Start temp conversion (otherwise they happen once every 64 seconds)
#define DS3231_REG_CONTROL_RS2_BIT 0x10 // 11 = 8 KHz, 10 = 4 KHz, 01 = 1 KHz, 00 = 1 Hz (default = 11)
#define DS3231_REG_CONTROL_RS1_BIT 0x08
#define DS3231_REG_CONTROL_INTCN_BIT 0x04 // 1 = Enable interrupt output, 0 = square wave (default = 1)
#define DS3231_REG_CONTROL_A2IE_BIT 0x02 // AL2 int enable on square wave pin, default = 0
#define DS3231_REG_CONTROL_A1IE_BIT 0x01 // AL1 int enable on square wave pin, default = 0

#define DS3231_REG_STATUS 0x0F

#define DS3231_REG_STATUS_OSF_BIT 0x80 // Clock stopped status (default = 1)
#define DS3231_REG_STATUS_EN32KHZ_BIT 0x08 // 1 means 32 KHz output is on, 0 = High-Z (default = 1)
#define DS3231_REG_STATUS_BSY_BIT 0x04 // 1 means busy with TCXO functions
#define DS3231_REG_STATUS_A2F_BIT 0x02 // 1 means alarm2 pending (write 0 to clear)
#define DS3231_REG_STATUS_A1F_BIT 0x01 // 1 means alarm1 pending (write 0 to clear)

#define DS3231_REG_AGING_OFFSET 0x10 // Higher is slower (default = 0) .1 PPM at 25C
#define DS3231_REG_TEMP_HIGH 0x11 // Upper 8 bits.  10 bit signed temperature, LSB = .25 C
#define DS3231_REG_TEMP_LOW 0x12 // Lower 2 bits in MSBs (Lower 6 bits unused)


// RTC Notes

// DS1307: I2C ADDR = 0x68, BBRAM = 0x08 - 0x3F, 0x00 - 0x07:
//   0x00   BCD Seconds    Bit 7 = CH (Clock Halt)
//   0x01   BCD Mins
//   0x02   BCD Hours      Bit 6 = 12 hour mode
//   0x03   Day of week [1..7]
//   0x04   BCD Date [1..31]
//   0x05   BCD Month [1..12]
//   0x06   BCD Year [00..99]
//   0x07   Control:
//            Bit 7 = Out pin level (when SQWE == 0), Bit 4 = SQWE (square wave enable), Bit 1:0 = RS: Out frequency 0=1Hz, 1=4KHz, 2=8KHz, 3=32KHz
// On first power up, datetime is 01/01/00 1 00:00:00 with CH bit set
// Reads are buffered for consistency
// During write, divider chain is cleared when you write to seconds and you must write remaining registers within one second.

// DS1338: same as DS1307 except:
//    More control bits:
//      Bit 5 = OSF, 1 if clock had ever stopped since this bit was last cleared
// 400 KHz capable

// DS1308: same as DS1307 except:
//    More control bits:
//      Bit 2 = BBCLK, 1 means clock output is enabled even when under battery power
//      Bit 3 = LOS, 1 means loss of external clock input
//      Bit 5 = OSF, 1 if clock had ever stopped since this bit was last cleared
//      Bit 6 = ECLK, 1 = use clock input

// DS1337: same as DS1307 except: (address is 0x68)
//      Bit 7 of 0x00 is always zero, not CH bit
//      Bit 7 of 0x05 is century bit
//     0x07 - 0x0A is Alarm 1
//     0x0B - 0x0D is Alarm 2
//     0x0E is control
//        Bit 0 A1 interrupt enable
//            1 A2 interrupt enable
//            2 INTCN  1 enable interrupt output pin, 0 enable square wave pin.  Is low when power first applied
//          4:3 RS
//            7 EOSC_L  Enable oscillator when low, is low when power first applied
//     0x0F is control/status
//        Bit 0 A1F  alarm 1 status, write 0 to clear
//            1 A2F  alarm 2 status, write 0 to clear
//            7 OSF, 1 if oscillator had ever stopped
// no BBRAM
// 400 KHz capable
//
// DS1339: same as DS1337 but with trickle charger


// PCF8523 is different:
//  0x00 Control_1
//  0x01 Control_2
//  0x02 Control_3
//  0x03 BCD Seconds, OS in bit 7
//  0x04 BCD Minutes
//  0x05 BCD Hours  (12 hour mode bit is not here)
//  0x06 BCD Day of month [1..31]
//  0x07 Day of week [0..6]
//  0x08 Month [1..12]
//  0x09 Year [0..99]
// 0x0A - 0x0D  alarm
//  0x0E Offset register
//  0x0F Clkout
//  0x10-0x11: timer A
//  0x12-0x13: timer B



int nk_ext_rtc_set_datetime(void *port, const nkdatetime_t *datetime)
{
    uint8_t buf[8];

    buf[0] = 0; // Starting write address
    buf[1 + DS3231_REG_SECONDS] = ((datetime->sec / 10) << 4) + (datetime->sec % 10);
    buf[1 + DS3231_REG_MINUTES] = ((datetime->min / 10) << 4) + (datetime->min % 10);
    buf[1 + DS3231_REG_HOURS] = ((datetime->hour / 10) << 4) + (datetime->hour % 10); // 24 hour mode: bit 6 is 0
    buf[1 + DS3231_REG_DAY] = datetime->weekday + 1; // Day of week
    buf[1 + DS3231_REG_DATE] = (((datetime->day + 1) / 10) << 4) + ((datetime->day + 1) % 10);
    buf[1 + DS3231_REG_MONTH] = (((datetime->month + 1) / 10) << 4) + ((datetime->month + 1) % 10); // Bit 7 is century on some of them
    buf[1 + DS3231_REG_YEAR] = (((datetime->year - 2000) / 10) << 4) + ((datetime->year - 2000) % 10);

    return nk_i2c_write(port, DS3231_I2C_ADDR, sizeof(buf), buf);
}

int nk_ext_rtc_get_datetime(void *port, nkdatetime_t *datetime)
{
    uint8_t buf[8];
    int rtn;
    
    buf[0] = 0; // Set starting address
    rtn = nk_i2c_write(port, DS3231_I2C_ADDR, 1, buf);

    if (rtn)
        return rtn;

    memset(buf, 0, sizeof(buf));

    // Read date/time
    rtn = nk_i2c_read(port, DS3231_I2C_ADDR, 7, buf);

    if (rtn)
        return rtn;

    datetime->sec = (0x0F & buf[DS3231_REG_SECONDS]) + ((0x7 & (buf[DS3231_REG_SECONDS] >> 4)) * 10);
    datetime->min = (0x0F & buf[DS3231_REG_MINUTES]) + ((0x07 & (buf[DS3231_REG_MINUTES] >> 4)) * 10);
    datetime->hour = (0x0F & buf[DS3231_REG_HOURS]) + ((0x3 & (buf[DS3231_REG_HOURS] >> 4)) * 10);
    datetime->weekday = (0x07 & buf[DS3231_REG_DAY]) - 1;
    datetime->day = (0x0F & buf[DS3231_REG_DATE]) + ((0x03 & (buf[DS3231_REG_DATE] >> 4)) * 10) - 1;
    datetime->month = (0x0F & buf[DS3231_REG_MONTH]) + ((0x1 & (buf[DS3231_REG_MONTH] >> 4)) * 10) - 1;
    datetime->year = (0x0F & buf[DS3231_REG_YEAR]) + ((buf[DS3231_REG_YEAR] >> 4) * 10) + 2000;

    // Sanity check the date- so we don't crash
    return nk_datetime_sanity(datetime);
}

int nk_ext_rtc_init(void *port)
{
    return 0;
}
