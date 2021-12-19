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

// PCF8523 Real Time Clock

// 1 MHz I2C Capable

#define PCF8523_I2C_ADDR 0x68

// PCF8523 Registers
#define PCF8523_REG_CONTROL_1 0x00

#define PCF8523_CIE_BIT 0x01 // 1 = correction cycle interrupts, 0 = disabled (default)
#define PCF8523_AIE_BIT 0x02 // 1 = alarm interrupt enabled, 0 = disabled (default)
#define PCF8523_SIE_BIT 0x04 // 1 = seconds interrupt enabled, 0 = disabled (default)
#define PCF8523_12_BIT 0x08 // 1 = 12 hour mode, 0 = 24 hour mode (default)
#define PCF8523_SR_BIT 0x10 // 1 = software reset, 0 = no reset (default)
#define PCF8523_STOP_BIT 0x20 // 1 = RTC frozen, 0 = RTC running (default)
#define PCF8523_CAP_SEL_BIT 0x80 // 1 = 12.5 pF, 0 = 7pF (default)

// For a software reset, write 0x58 to CONTROL_1

#define PCF8523_REG_CONTROL_2 0x01

#define PCF8523_CTBIE_BIT 0x01 // 1 = Countdown timer B interrupt enable, 0 = disabled (default)
#define PCF8523_CTAIE_BIT 0x02 // 1 = Countdown timer A interrupt enabled, 0 = disable (default)
#define PCF8523_WTAIE_BIT 0x04 // 1 = Watchdog timer interrupt enabled, 0 = disabled (default)
#define PCF8523_AF_BIT 0x08 // 1 indicates alarm tiggered, software must clear
#define PCF8523_SF_BIT 0x10 // 1 indicates seconds int triggered, software clears
#define PCF8523_CTBF_BIT 0x20 // 1 indicates countdown timer B triggered, software clears
#define PCF8523_CTAF_BIT 0x40 // 1 indicates countdown timer A triggered, software clears
#define PCF8523_WTAF_BIT 0x80 // 1 indicates watchdog timer triggered, software clears

#define PCF8523_REG_CONTROL_3 0x02

#define PCF8523_BLIE_BIT 0x01 // 1 enables interrupt from BLF (0 is default)
#define PCF8523_BSIE_BIT 0x02 // 1 enables interrupt from BSF (0 is default)
#define PCF8523_BLF_BIT 0x04 // 1 = battery low
#define PCF8523_BSF_BIT 0x08 // 1 = battery switchover, software clears

#define PCF8523_PM_SHIFT 5 // battery switch-over and low detection control see below
#define PCF8523_PM_MASK 0x07
// 000 standard switchover mode, low detection enabled
// 001 direct switchover mode, low detection enabled
// 010, 011 switchover disabled, low detection enabled
// 100 standard switchover mode, low detection disabled
// 101 direct switchover mode, low detection disabled
// 110 not allowed

// 111 switchover disabled, low detection disabled (default)
//     battery will not power clock, only VCDD

// Standard mode: VDD < VBAT and VDD < 2.5V
// Direct mode: VDD < VBAT

#define PCF8523_REG_SECONDS 0x03 // BCD Seconds [00..59], bit 7 is OS bit

#define PCF8523_OS_BIT 0x80 // Oscillator stopped bit

#define PCF8523_REG_MINUTES 0x04 // BCD Minutes [00..59]
#define PCF8523_REG_HOURS 0x05 // BCD Hours [00..23]
#define PCF8523_REG_DATE 0x06 // BCD Day [1..31]
#define PCF8523_REG_WEEKDAY 0x07 // Day of week [0..6]
#define PCF8523_REG_MONTH 0x08 // BCD Month [1..12], Bit 7 is century
#define PCF8523_REG_YEAR 0x09 // BCD Year [00.99]

#define PCF8523_REG_AL_MINUTES 0x0A // BCD Minutes [00..59], Bit 7 = AEN_M

#define PCF8523_AEN_M_BIT 0x80

#define PCF8523_REG_AL_HOURS 0x0B // BCD Hours [00..23], Bit 7 = AEN_H

#define PCF8523_AEN_H_BIT 0x80

#define PCF8523_REG_AL_DATE 0x0C // BCD Date [01..31], Bit 7 = AEN_D

#define PCF8523_AEN_D_BIT 0x80

#define PCF8523_REG_AL_WEEKDAY 0x0D // Day of week [0..6], Bit 7 = AEN_W

#define PCF8523_AEN_W_BIT 0x80

#define PCF8523_REG_OFFSET 0x0E // Bit 7 = Mode, Bit 6:0 = Offset

#define PCF8523_MODE_BIT 0x80 // 1 = offset corrections applied once per second, 0 = per minute

#define PCF8523_REG_CLKOUT 0x0F

#define PCF8523_TBC_BIT 0x01 // 1 = Timer B enabled, 0 = disabled

#define PCF8523_TAC_SHIFT 1
#define PCF8523_TAC_MASK 0x03
// 00 disabled (default)
// 01 timer A is countdown
// 10 timer A is watchdog
// 11 disabled


#define PCF8523_COF_SHIFT 3 // CLKOUT frequency selection
#define PCF8523_COF_MASK 0x07
// 0 = 32768 (default)
// 1 = 16384
// 2 = 8192
// 3 = 4096
// 4 = 1024
// 5 = 32
// 6 = 1
// 7 = CLKOUT disabled


#define PCF8523_TBM_BIT 0x40 // 1 = Pulsed interrupt timer B, 0 = non-pulsed (default)
#define PCF8523_TAM_BIT 0x80 // 1 = Pulsed interrupt timer A and second, 0 = non-pulsed (default)

#define PCF8523_REG_TMRA_FREQ 0x10

#define PCF8523_TAQ_SHIFT 0
#define PCF8523_TAQ_MASK 0x07
// Timer A source frequency
// 0 = 4096
// 1 = 64 Hz
// 2 = 1 Hz
// 3 1/60 Hz
// 4, 5, 7: 1/3600 Hz (7 is default)

#define PCF8523_REG_TMRA 0x11

#define PCF8523_REG_TMRB_FREQ 0x12

#define PCF8523_TBQ_SHIFT 0
#define PCF8523_TBQ_MASK 0x07
// 0 4096 Hz
// 1 64 Hz
// 2 1 Hz
// 3 1/60 Hz
// 4, 6, 7 (default): 1/3600 Hz

#define PCF8523_TBW_SHIFT 4
#define PCF8523_TBW_MASK 0x07
// 0 46.875 ms
// 1 62.5 ms
// 2 78.125 ms
// 3 93.75 ms
// 4 125 ms
// 5 156.24 ms
// 6 187.5 ms
// 7 218.75 ms

#define PCF8523_REG_TMRB 0x13



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
    uint8_t buf[21];

    memset(buf, 0, sizeof(buf));

    buf[0] = 0; // Starting write address
    buf[1 + PCF8523_REG_CONTROL_1] = 0x00;
    buf[1 + PCF8523_REG_CONTROL_2] = 0x00;
    buf[1 + PCF8523_REG_CONTROL_3] = 0x00;
    buf[1 + PCF8523_REG_SECONDS] = ((datetime->sec / 10) << 4) + (datetime->sec % 10);
    buf[1 + PCF8523_REG_MINUTES] = ((datetime->min / 10) << 4) + (datetime->min % 10);
    buf[1 + PCF8523_REG_HOURS] = ((datetime->hour / 10) << 4) + (datetime->hour % 10);
    buf[1 + PCF8523_REG_DATE] = (((datetime->day + 1) / 10) << 4) + ((datetime->day + 1) % 10);
    buf[1 + PCF8523_REG_WEEKDAY] = datetime->weekday ; // Day of week
    buf[1 + PCF8523_REG_MONTH] = (((datetime->month + 1) / 10) << 4) + ((datetime->month + 1) % 10);
    buf[1 + PCF8523_REG_YEAR] = (((datetime->year - 2000) / 10) << 4) + ((datetime->year - 2000) % 10);
    buf[1 + PCF8523_REG_AL_MINUTES] = 0x00;
    buf[1 + PCF8523_REG_AL_HOURS] = 0x00;
    buf[1 + PCF8523_REG_AL_DATE] = 0x00;
    buf[1 + PCF8523_REG_AL_WEEKDAY] = 0x00;
    buf[1 + PCF8523_REG_OFFSET] = 0x00;
    buf[1 + PCF8523_REG_CLKOUT] = 0x00; // CLKOUT is 32 KHz
    buf[1 + PCF8523_REG_TMRA_FREQ] = 0x00;
    buf[1 + PCF8523_REG_TMRA] = 0x00;
    buf[1 + PCF8523_REG_TMRB_FREQ] = 0x00;
    buf[1 + PCF8523_REG_TMRB] = 0x00;
    

    return nk_i2c_write(port, PCF8523_I2C_ADDR, sizeof(buf), buf);
}

int nk_ext_rtc_get_datetime(void *port, nkdatetime_t *datetime)
{
    uint8_t buf[20];
    int rtn;
    

    buf[0] = 0; // Set starting address
    rtn = nk_i2c_write(port, PCF8523_I2C_ADDR, 1, buf);

    if (rtn)
        return rtn;

    memset(buf, 0, sizeof(buf));

    // Read everything
    rtn = nk_i2c_read(port, PCF8523_I2C_ADDR, 20, buf);

    if (rtn)
        return rtn;

    datetime->sec = (0x0F & buf[PCF8523_REG_SECONDS]) + ((0x7 & (buf[PCF8523_REG_SECONDS] >> 4)) * 10);
    datetime->min = (0x0F & buf[PCF8523_REG_MINUTES]) + ((0x07 & (buf[PCF8523_REG_MINUTES] >> 4)) * 10);
    datetime->hour = (0x0F & buf[PCF8523_REG_HOURS]) + ((0x3 & (buf[PCF8523_REG_HOURS] >> 4)) * 10);
    datetime->weekday = (0x07 & buf[PCF8523_REG_WEEKDAY]);
    datetime->day = (0x0F & buf[PCF8523_REG_DATE]) + ((0x03 & (buf[PCF8523_REG_DATE] >> 4)) * 10) - 1;
    datetime->month = (0x0F & buf[PCF8523_REG_MONTH]) + ((0x1 & (buf[PCF8523_REG_MONTH] >> 4)) * 10) - 1;
    datetime->year = (0x0F & buf[PCF8523_REG_YEAR]) + ((buf[PCF8523_REG_YEAR] >> 4) * 10) + 2000;

    // Sanity check the date- so we don't crash
    return nk_datetime_sanity(datetime);
}

int nk_ext_rtc_init(void *port)
{
    return 0;
}
