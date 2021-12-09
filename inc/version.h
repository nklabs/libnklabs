#ifndef _Iversion
#define _Iversion

#include <stdint.h>

extern const uint16_t firmware_major;
extern const uint16_t firmware_minor;
extern const uint16_t build_year;
extern const uint8_t build_month;
extern const uint8_t build_day;
extern const uint8_t build_hour;
extern const uint8_t build_minute;
extern const char git_hash[];

#endif
