
DATE=$(shell date -u -Iminute)
YEAR=$(shell expr $(shell echo $(DATE) | cut -b 1-4) + 0)
MONTH=$(shell expr $(shell echo $(DATE) | cut -b 6-7) + 0)
DAY=$(shell expr $(shell echo $(DATE) | cut -b 9-10) + 0)
HOUR=$(shell expr $(shell echo $(DATE) | cut -b 12-13) + 0)
MINUTE=$(shell expr $(shell echo $(DATE) | cut -b 15-16) + 0)

################################################################################
# Automatically-generated file. Do not edit!
################################################################################

ifdef SystemRoot
	SHELL = cmd.exe
	MK_DIR = mkdir
else
	ifeq ($(shell uname), Linux)
		MK_DIR = mkdir -p
	endif

	ifeq ($(shell uname | cut -d _ -f 1), CYGWIN)
		MK_DIR = mkdir -p
	endif

	ifeq ($(shell uname | cut -d _ -f 1), MINGW32)
		MK_DIR = mkdir -p
	endif

	ifeq ($(shell uname | cut -d _ -f 1), MINGW64)
		MK_DIR = mkdir -p
	endif

	ifeq ($(shell uname | cut -d _ -f 1), DARWIN)
		MK_DIR = mkdir -p
	endif
endif

# List the subdirectories for creating object files
SUB_DIRS +=  \
 \
hpl/tc \
hpl/systick \
hpl/dmac \
hal/src \
samd21a/gcc \
hpl/pm \
hpl/sysctrl \
hal/utils/src \
hpl/sercom \
examples \
hpl/gclk \
samd21a/gcc/gcc \
hpl/rtc \
hpl/nvmctrl \
hpl/core

# List the object files
OBJS +=  \
basic_cmds_atsam.o \
database.o \
nkymodem_cmd.o \
main_atsam.o \
nkprintf_fp.o \
nkrtc_atsam.o \
nkdbase.o \
nkserialize.o \
nkmcuflash.o \
nkspiflash.o \
nkcrclib.o \
nkscan.o \
nksched.o \
nkcli.o \
nkreadline.o \
nkuart_atsam.o \
nkarch_atsam.o \
nkymodem.o \
nkprintf.o \
nkinfile.o \
nkoutfile.o \
nkdectab.o \
nkstring.o \
nkstrtod.o \
hal/src/hal_io.o \
hpl/systick/hpl_systick.o \
hal/src/hal_calendar.o \
samd21a/gcc/gcc/startup_samd21.o \
hal/utils/src/utils_syscalls.o \
hpl/nvmctrl/hpl_nvmctrl.o \
hal/src/hal_spi_m_sync.o \
hal/src/hal_timer.o \
hal/src/hal_delay.o \
hpl/sysctrl/hpl_sysctrl.o \
hpl/core/hpl_init.o \
hal/utils/src/utils_list.o \
hpl/core/hpl_core_m0plus_base.o \
hal/utils/src/utils_assert.o \
hpl/dmac/hpl_dmac.o \
hpl/pm/hpl_pm.o \
hpl/gclk/hpl_gclk.o \
hal/src/hal_flash.o \
hal/src/hal_init.o \
samd21a/gcc/system_samd21.o \
hpl/rtc/hpl_rtc.o \
examples/driver_examples.o \
driver_init.o \
hal/src/hal_usart_async.o \
hpl/sercom/hpl_sercom.o \
hal/utils/src/utils_ringbuffer.o \
hal/src/hal_gpio.o \
hal/utils/src/utils_event.o \
hal/src/hal_sleep.o \
atmel_start.o \
hpl/tc/hpl_tc.o \
hal/src/hal_atomic.o

OBJS_AS_ARGS +=  \
"basic_cmds_atsam.o" \
"database.o" \
"nkymodem_cmd.o" \
"main_atsam.o" \
"nkprintf_fp.o" \
"nkrtc_atsam.o" \
"nkdbase.o" \
"nkserialize.o" \
"nkmcuflash.o" \
"nkspiflash.o" \
"nkcrclib.o" \
"nkscan.o" \
"nksched.o" \
"nkcli.o" \
"nkreadline.o" \
"nkuart_atsam.o" \
"nkarch_atsam.o" \
"nkymodem.o" \
"nkprintf.o" \
"nkinfile.o" \
"nkoutfile.o" \
"nkdectab.o" \
"nkstring.o" \
"nkstrtod.o" \
"hal/src/hal_io.o" \
"hpl/systick/hpl_systick.o" \
"hal/src/hal_calendar.o" \
"samd21a/gcc/gcc/startup_samd21.o" \
"hal/utils/src/utils_syscalls.o" \
"hpl/nvmctrl/hpl_nvmctrl.o" \
"hal/src/hal_spi_m_sync.o" \
"hal/src/hal_timer.o" \
"hal/src/hal_delay.o" \
"hpl/sysctrl/hpl_sysctrl.o" \
"hpl/core/hpl_init.o" \
"hal/utils/src/utils_list.o" \
"hpl/core/hpl_core_m0plus_base.o" \
"hal/utils/src/utils_assert.o" \
"hpl/dmac/hpl_dmac.o" \
"hpl/pm/hpl_pm.o" \
"hpl/gclk/hpl_gclk.o" \
"hal/src/hal_flash.o" \
"hal/src/hal_init.o" \
"samd21a/gcc/system_samd21.o" \
"hpl/rtc/hpl_rtc.o" \
"examples/driver_examples.o" \
"driver_init.o" \
"hal/src/hal_usart_async.o" \
"hpl/sercom/hpl_sercom.o" \
"hal/utils/src/utils_ringbuffer.o" \
"hal/src/hal_gpio.o" \
"hal/utils/src/utils_event.o" \
"hal/src/hal_sleep.o" \
"atmel_start.o" \
"hpl/tc/hpl_tc.o" \
"hal/src/hal_atomic.o"

# List the directories containing header files
DIR_INCLUDES +=  \
-I"../../../inc" \
-I"../../../config" \
-I"../../../config_atsam" \
-I"../atmel_start/" \
-I"../atmel_start/config" \
-I"../atmel_start/examples" \
-I"../atmel_start/hal/include" \
-I"../atmel_start/hal/utils/include" \
-I"../atmel_start/hpl/core" \
-I"../atmel_start/hpl/dmac" \
-I"../atmel_start/hpl/gclk" \
-I"../atmel_start/hpl/nvmctrl" \
-I"../atmel_start/hpl/pm" \
-I"../atmel_start/hpl/port" \
-I"../atmel_start/hpl/rtc" \
-I"../atmel_start/hpl/sercom" \
-I"../atmel_start/hpl/sysctrl" \
-I"../atmel_start/hpl/systick" \
-I"../atmel_start/hpl/tc" \
-I"../atmel_start/hri" \
-I"../atmel_start/" \
-I"../atmel_start/CMSIS/Core/Include" \
-I"../atmel_start/samd21a/include"

# List the dependency files
DEPS := $(OBJS:%.o=%.d)

DEPS_AS_ARGS +=  \
"basic_cmds_atsam.d" \
"database.d" \
"nkymodem_cmd.d" \
"main_atsam.d" \
"nkprintf_fp.d" \
"nkrtc_atsam.d" \
"nkdbase.d" \
"nkserialize.d" \
"nkmcuflash.d" \
"nkspiflash.d" \
"nkcrclib.d" \
"nkscan.d" \
"nksched.d" \
"nkcli.d" \
"nkreadline.d" \
"nkuart_atsam.d" \
"nkarch_atsam.d" \
"nkymodem.d" \
"nkprintf.d" \
"nkinfile.d" \
"nkoutfile.d" \
"nkdectab.d" \
"nkstring.d" \
"nkstrtod.d" \
"samd21a/gcc/gcc/startup_samd21.d" \
"hal/src/hal_calendar.d" \
"hal/utils/src/utils_event.d" \
"hal/src/hal_io.d" \
"hpl/systick/hpl_systick.d" \
"hal/utils/src/utils_syscalls.d" \
"hpl/nvmctrl/hpl_nvmctrl.d" \
"hal/src/hal_timer.d" \
"hal/src/hal_spi_m_sync.d" \
"hpl/core/hpl_core_m0plus_base.d" \
"hal/utils/src/utils_list.d" \
"hpl/dmac/hpl_dmac.d" \
"hal/utils/src/utils_assert.d" \
"hal/src/hal_delay.d" \
"hpl/core/hpl_init.d" \
"hpl/sysctrl/hpl_sysctrl.d" \
"hal/src/hal_flash.d" \
"hpl/gclk/hpl_gclk.d" \
"hal/src/hal_init.d" \
"driver_init.d" \
"hal/src/hal_usart_async.d" \
"samd21a/gcc/system_samd21.d" \
"examples/driver_examples.d" \
"hpl/rtc/hpl_rtc.d" \
"hal/src/hal_sleep.d" \
"hal/utils/src/utils_ringbuffer.d" \
"hpl/sercom/hpl_sercom.d" \
"hal/src/hal_gpio.d" \
"hal/src/hal_atomic.d" \
"hpl/tc/hpl_tc.d" \
"hpl/pm/hpl_pm.d" \
"atmel_start.d"

OUTPUT_FILE_NAME :=AtmelStart
QUOTE := "
OUTPUT_FILE_PATH +=$(OUTPUT_FILE_NAME).elf
OUTPUT_FILE_PATH_AS_ARGS +=$(OUTPUT_FILE_NAME).elf

vpath %.c ../../../src/ ../../../app/ ../atmel_start/
vpath %.s ../../../src/ ../../../app/ ../atmel_start/
vpath %.S ../../../src/ ../../../app/ ../atmel_start/

# All Target
all: $(SUB_DIRS) $(OUTPUT_FILE_PATH)

# Rebuild version.o if anything changed
version.o: $(OBJS) ../../../app/VERSION_MAJOR ../../../app/VERSION_MINOR ../../../app/version.c

# Linker target

$(OUTPUT_FILE_PATH): $(OBJS) version.o
	@echo Building target: $@
	@echo Invoking: ARM/GNU Linker
	$(QUOTE)arm-none-eabi-gcc$(QUOTE) -o $(OUTPUT_FILE_NAME).elf $(OBJS_AS_ARGS) version.o -Wl,--start-group -lm -Wl,--end-group -mthumb \
-Wl,-Map="$(OUTPUT_FILE_NAME).map" --specs=nano.specs -Wl,--gc-sections -mcpu=cortex-m0plus \
 \
-T"samd21j18a_flash.ld" \
-L"../atmel_start/samd21a/gcc/gcc"
	@echo Finished building target: $@

	"arm-none-eabi-objcopy" -O binary "$(OUTPUT_FILE_NAME).elf" "$(OUTPUT_FILE_NAME).bin"
	"arm-none-eabi-objcopy" -O ihex -R .eeprom -R .fuse -R .lock -R .signature  \
        "$(OUTPUT_FILE_NAME).elf" "$(OUTPUT_FILE_NAME).hex"
	"arm-none-eabi-objcopy" -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma \
        .eeprom=0 --no-change-warnings -O binary "$(OUTPUT_FILE_NAME).elf" \
        "$(OUTPUT_FILE_NAME).eep" || exit 0
	"arm-none-eabi-objdump" -h -S "$(OUTPUT_FILE_NAME).elf" > "$(OUTPUT_FILE_NAME).lss"
	"arm-none-eabi-size" "$(OUTPUT_FILE_NAME).elf"

	

# Compiler targets




%.o: %.c
	@echo Building file: $<
	@echo ARM/GNU C Compiler
	$(QUOTE)arm-none-eabi-gcc$(QUOTE) -x c -mthumb -DDEBUG -Os -ffunction-sections -mlong-calls -g3 -Wall -c -std=gnu99 \
-D__SAMD21J18A__ -mcpu=cortex-m0plus  \
$(DIR_INCLUDES) \
-DNKVERSION_MAJOR=`cat ../../../app/VERSION_MAJOR`  -DNKVERSION_MINOR=`cat ../../../app/VERSION_MINOR` -DNKYEAR=$(YEAR) -DNKMONTH=$(MONTH) -DNKDAY=$(DAY) -DNKHOUR=$(HOUR) -DNKMINUTE=$(MINUTE) \
-MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"  -o "$@" "$<"
	@echo Finished building: $<

%.o: %.s
	@echo Building file: $<
	@echo ARM/GNU Assembler
	$(QUOTE)arm-none-eabi-as$(QUOTE) -x c -mthumb -DDEBUG -Os -ffunction-sections -mlong-calls -g3 -Wall -c -std=gnu99 \
-D__SAMD21J18A__ -mcpu=cortex-m0plus  \
$(DIR_INCLUDES) \
-MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"  -o "$@" "$<"
	@echo Finished building: $<

%.o: %.S
	@echo Building file: $<
	@echo ARM/GNU Preprocessing Assembler
	$(QUOTE)arm-none-eabi-gcc$(QUOTE) -x c -mthumb -DDEBUG -Os -ffunction-sections -mlong-calls -g3 -Wall -c -std=gnu99 \
-D__SAMD21J18A__ -mcpu=cortex-m0plus  \
$(DIR_INCLUDES) \
-MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"  -o "$@" "$<"
	@echo Finished building: $<

# Detect changes in the dependent files and recompile the respective object files.
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(DEPS)),)
-include $(DEPS)
endif
endif

$(SUB_DIRS):
	$(MK_DIR) "$@"

clean:
	rm -f $(OBJS_AS_ARGS)
	rm -f $(OUTPUT_FILE_PATH)
	rm -f $(DEPS_AS_ARGS)
	rm -f $(OUTPUT_FILE_NAME).a $(OUTPUT_FILE_NAME).hex $(OUTPUT_FILE_NAME).bin \
        $(OUTPUT_FILE_NAME).lss $(OUTPUT_FILE_NAME).eep $(OUTPUT_FILE_NAME).map \
        $(OUTPUT_FILE_NAME).srec

flash:
	(openocd --file atmelice.cfg)
