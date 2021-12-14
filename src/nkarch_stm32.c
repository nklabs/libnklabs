#include <string.h>
#include "nkprintf.h"
#include "nkarch_stm32.h"

static uint32_t timeout;
static nk_time_t start_time;

// Scheduler timer- using low power timer

uint32_t nk_get_sched_time(void)
{
	nk_time_t current = nk_get_time();
	nk_time_t change = current - start_time;
	if (change > timeout)
		timeout = 0;
	else
		timeout -= change;
	return current;
}

uint32_t nk_get_sched_timeout(void)
{
	return timeout;
}

// Convert delay in milliseconds to number of scheduler timer clock ticks
uint32_t nk_convert_delay(uint32_t delay)
{
	return (delay * NK_TIME_COUNTS_PER_SECOND) / 1000;
}

void nk_init_sched_timer()
{
}

// delay in timer ticks
void nk_start_sched_timer(uint32_t delay)
{
	timeout = delay;
	start_time = nk_get_time();
}

// Get current time

nk_time_t nk_get_time()
{
	return HAL_GetTick();
}

// Busy loop delay

void nk_udelay(unsigned long usec)
{
	uint32_t msec = (usec / 1000);
	if (!msec)
		msec = 1;

	// Just use the HAL one...
	HAL_Delay(msec);
#if 0
	// Generic implementation
	nk_time_t old = nk_get_time();
	nk_time_t clocks = msec * NK_TIME_COUNTS_PER_SECOND / 1000;
	// nk_time_t clocks = usec * NK_TIME_COUNTS_PER_SECOND / 1000000;
	while ((nk_get_time() - old) < clocks);
#endif
}

//void _exit()
//{
//}

void reboot()
{
	NVIC_SystemReset();
}



int nk_init_mcuflash()
{
	return 0;
}

// Erase one page
// Page size == FLASH_PAGE_SIZE

int flash_erase(uint32_t address)
{
	int rtn = 0;

	address -= NK_FLASH_BASE_ADDRESS;

#ifdef FLASH_PAGE_SIZE
	FLASH_EraseInitTypeDef erase;

	erase.TypeErase = FLASH_TYPEERASE_PAGES;
#ifdef FLASH_FLAG_PGAERR // Maybe?
	erase.Banks = 0;
	erase.Page = (address / FLASH_PAGE_SIZE);
#else
	erase.PageAddress = address;
#endif
	erase.NbPages = 1;

	HAL_FLASH_Unlock();

	__HAL_FLASH_CLEAR_FLAG(
		FLASH_FLAG_EOP
		| FLASH_FLAG_WRPERR
#ifdef FLASH_FLAG_OPERR
		| FLASH_FLAG_OPERR
#endif
#ifdef FLASH_FLAG_PGAERR
		| FLASH_FLAG_PGAERR
#endif
#ifdef FLASH_FLAG_PGERR
		| FLASH_FLAG_PGERR
#endif
#ifdef FLASH_FLAG_PGSERR
		| FLASH_FLAG_PGSERR
#endif
	);

	uint32_t err = 0;
	rtn = (HAL_FLASHEx_Erase(&erase, &err) != HAL_OK);

	HAL_FLASH_Lock();

#else

// Sectors on not uniformly sized

// But FLASH_SECTOR_TOTAL indicates how many..

//On STM32F756xx and STM32F74xx devices, a main memory block divided into 4 
//sectors of 32 Kbytes, 1 sector of 128 Kbytes and 3 sectors of 256 Kbytes   

#if 0
	FLASH_EraseInitTypeDef erase;
	memset(&erase, 0, sizeof(erase));
	erase.TypeErase = FLASH_TYPEERASE_SECTORS;
	erase.Sector = address / sector-size;
	erase.NbSectors = 1;
	erase.VoltageRange = FLASH_VOLTAGE_RANGE_4; // Ugh.. this needs to be a config option

	HAL_FLASH_Unlock();
	uint32_t err = 0;
	rtn = (HAL_FLASHEx_Erase(&erase, &err) != HAL_OK);
	HAL_FLASH_Lock();
#endif

        rtn = -1;
#endif

        return rtn;
}

// Erase a range of space, all pages that touch the space are erased

int nk_mcuflash_erase(uint32_t address, uint32_t byte_count)
{
	address += NK_FLASH_BASE_ADDRESS;

#ifdef FLASH_PAGE_SIZE
	int rtn = 0;
	// As long as we're not done..
	while (byte_count) {
		// Try to use largest erase that works..
		if ((address & (FLASH_PAGE_SIZE-1)) == 0 && byte_count >= FLASH_PAGE_SIZE) {
			rtn = flash_erase(address);
			if (rtn) break;
			address += FLASH_PAGE_SIZE;
			byte_count -= FLASH_PAGE_SIZE;
		} else {
			// None work? Fail..
			break;
		}
	}

	if (rtn) {
		nk_printf("ERROR: flash_erase returned %d\n", rtn);
	} else if (byte_count) {
		nk_printf("ERROR: Invalid erase size\n");
		return -1;
	}
	return 0;
#else
	return -1;
#endif
}

// Program 8 bytes

int flash_write(uint32_t address, uint64_t data)
{
	int rtn;

	HAL_FLASH_Unlock();

	rtn = (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data));

	HAL_FLASH_Lock();

	return rtn;
}

int nk_mcuflash_write(uint32_t address, uint8_t *data, uint32_t byte_count)
{
	int rtn = 0; // Assume success

	address += NK_FLASH_BASE_ADDRESS;

	uint32_t page_size = 8; // Write size

	// Write a page at a time
	while (byte_count) {
		uint32_t page_offset = (address & (page_size - 1)); // Starting offset within page
		uint32_t page_len = page_size - page_offset; // Up to one page

		if (byte_count < page_len)
			page_len = byte_count;

		if (page_len != page_size)
			rtn = 1; // We can only write multiples of pages
		else
			rtn = flash_write(address, *(uint64_t *)data);

		if (rtn)
			break;

		byte_count -= page_len;
		address += page_len;
		data += page_len;
	}

	if (rtn) {
		nk_printf("ERROR: Flash_write returned %d\n", rtn);
	}

	return rtn;
}

int nk_mcuflash_read(uint32_t address, uint8_t *data, uint32_t byte_count)
{
	// Flash is memory mapped
	memcpy((void *)data, (void *)(address + NK_FLASH_BASE_ADDRESS), byte_count);
	return 0;
}

// Obtain the STM32 system reset cause
reset_cause_t reset_cause_get(void)
{
    reset_cause_t reset_cause = RESET_CAUSE_UNKNOWN;

#ifdef RCC_FLAG_PORRST
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
    {
        reset_cause = RESET_CAUSE_POWER_ON_POWER_DOWN_RESET;
    }
#endif
#ifdef RCC_FLAG_PWRRST
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PWRRST))
    {
        reset_cause = RESET_CAUSE_POWER_ON_POWER_DOWN_RESET;
    }
#endif
#ifdef RCC_FLAG_PINRST
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
    {
        reset_cause = RESET_CAUSE_EXTERNAL_RESET_PIN_RESET;
    }
#endif
#ifdef RCC_FLAG_SFTRST
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
    {
        reset_cause = RESET_CAUSE_SOFTWARE_RESET; // This reset is induced by calling the ARM CMSIS `NVIC_SystemReset()` function!
    }
#endif
#ifdef RCC_FLAG_IWDGRST
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
    {
        reset_cause = RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET;
    }
#endif
#ifdef RCC_FLAG_WWDGRST
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
    {
        reset_cause = RESET_CAUSE_WINDOW_WATCHDOG_RESET;
    }
#endif
#ifdef RCC_FLAG_LPWRRST
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
    {
        reset_cause = RESET_CAUSE_LOW_POWER_RESET;
    }
#endif

    // Clear all the reset flags or else they will remain set during future resets until system power is fully removed.
    __HAL_RCC_CLEAR_RESET_FLAGS();

    return reset_cause; 
}

// Note: any of the STM32 Hardware Abstraction Layer (HAL) Reset and Clock Controller (RCC) header
// files, such as "STM32Cube_FW_F7_V1.12.0/Drivers/STM32F7xx_HAL_Driver/Inc/stm32f7xx_hal_rcc.h",
// "STM32Cube_FW_F2_V1.7.0/Drivers/STM32F2xx_HAL_Driver/Inc/stm32f2xx_hal_rcc.h", etc., indicate that the 
// brownout flag, `RCC_FLAG_BORRST`, will be set in the event of a "POR/PDR or BOR reset". This means that a 
// Power-On Reset (POR), Power-Down Reset (PDR), OR Brownout Reset (BOR) will trip this flag. See the 
// doxygen just above their definition for the `__HAL_RCC_GET_FLAG()` macro to see this:
// "@arg RCC_FLAG_BORRST: POR/PDR or BOR reset." <== indicates the Brownout Reset flag will *also* be set in 
// the event of a POR/PDR. 
// Therefore, you must check the Brownout Reset flag, `RCC_FLAG_BORRST`, *after* first checking the 
// `RCC_FLAG_PORRST` flag in order to ensure first that the reset cause is NOT a POR/PDR reset.


/// @brief      Obtain the system reset cause as an ASCII-printable name string from a reset cause type
/// @param[in]  reset_cause     The previously-obtained system reset cause
/// @return     A null-terminated ASCII name string describing the system reset cause
const char *reset_cause_get_name(reset_cause_t reset_cause)
{
    const char * reset_cause_name = "TBD";

    switch (reset_cause)
    {
        case RESET_CAUSE_UNKNOWN:
            reset_cause_name = "UNKNOWN";
            break;
        case RESET_CAUSE_LOW_POWER_RESET:
            reset_cause_name = "LOW_POWER_RESET";
            break;
        case RESET_CAUSE_WINDOW_WATCHDOG_RESET:
            reset_cause_name = "WINDOW_WATCHDOG_RESET";
            break;
        case RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET:
            reset_cause_name = "INDEPENDENT_WATCHDOG_RESET";
            break;
        case RESET_CAUSE_SOFTWARE_RESET:
            reset_cause_name = "SOFTWARE_RESET"; // We get this on reboot()
            break;
        case RESET_CAUSE_POWER_ON_POWER_DOWN_RESET:
            reset_cause_name = "BROWNOUT_RESET (BOR) / POWER-ON_RESET (POR) / POWER-DOWN_RESET (PDR)";
            break;
        case RESET_CAUSE_EXTERNAL_RESET_PIN_RESET:
            reset_cause_name = "EXTERNAL_RESET_PIN_RESET"; // We get this on power up because we have the reset pin connected
            break;
    }

    return reset_cause_name;
}

// STM32 built-in RTC

extern RTC_HandleTypeDef hrtc;

int nk_mcu_rtc_get_datetime(int *year, int *month, int *day, int *hour, int *min, int *sec)
{
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    *year = sDate.Year + 2000;
    *month = sDate.Month;
    *day = sDate.Date;
    // *weekday = sDate.WeekDay;

    *hour = sTime.Hours;
    *min = sTime.Minutes;
    *sec = sTime.Seconds;

    return 0;
}

int nk_mcu_rtc_set_datetime(int year, int month, int day, int hour, int min, int sec)
{
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    memset(&sTime, 0, sizeof(sTime));
    memset(&sDate, 0, sizeof(sDate));

    if (year < 2000 || year > 2099)
        return -1;

    if (day < 1 || day > 31)
        return -1;

    if (month < 1 || month > 12)	
        return -1;

    if (hour < 0 || hour > 23)
        return -1;
    
    if (min < 0 || min > 59)
        return -1;
    
    if (sec < 0 || sec > 59)
        return -1;

    sDate.Year = year - 2000; // 0..99
    sDate.Month = month; // BCD?
    sDate.Date = day; // 1..31
    sDate.WeekDay = 1; // Invalid for now

    sTime.Hours = hour;
    sTime.Minutes = min;
    sTime.Seconds = sec;

    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    return 0;
}

void nk_mcu_rtc_init()
{
    // Nothing to do for STM32
}
