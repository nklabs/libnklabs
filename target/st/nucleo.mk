# Paths

NK_SRC = ../../../src
NK_APP = ../../../app
NK_INC = ../../../inc
NK_APP_INC = ../../../app_inc
NK_CONFIG = ../../../config
NK_ARCHCONFIG = ../../../config_stm32

# A define for the platform
NK_PLATFORM := NK_PLATFORM_STM32

# Get application version number

NK_VERSION_MAJOR := $(shell cat $(NK_APP)/VERSION_MAJOR)
NK_VERSION_MINOR := $(shell cat $(NK_APP)/VERSION_MINOR)

# Get date/time

NK_DATE := $(shell date -u -Iminute)
NK_YEAR := $(shell expr $(shell echo $(NK_DATE) | cut -b 1-4) + 0)
NK_MONTH := $(shell expr $(shell echo $(NK_DATE) | cut -b 6-7) + 0)
NK_DAY := $(shell expr $(shell echo $(NK_DATE) | cut -b 9-10) + 0)
NK_HOUR := $(shell expr $(shell echo $(NK_DATE) | cut -b 12-13) + 0)
NK_MINUTE := $(shell expr $(shell echo $(NK_DATE) | cut -b 15-16) + 0)

# Get git hash as a string
# It is postfixed with -dirty if there are uncommitted changed; otherwise, it is postfixed with -clean.
NK_GIT_REV := \"$(shell git rev-parse HEAD)-$(shell if git diff-index --quiet HEAD --; then echo clean; else echo dirty; fi)\"

# Our soruce files..
C_SOURCES += \
$(NK_APP)/app_main.c \
$(NK_APP)/basic_cmds.c \
$(NK_APP)/button.c \
$(NK_APP)/database.c \
$(NK_APP)/ds3231.c \
$(NK_APP)/i2c.c \
$(NK_APP)/ina219.c \
$(NK_APP)/info_cmd.c \
$(NK_APP)/led.c \
$(NK_APP)/lm75.c \
$(NK_APP)/m24256.c \
$(NK_APP)/m24c02.c \
$(NK_APP)/m24m01.c \
$(NK_APP)/m95040.c \
$(NK_APP)/m95256.c \
$(NK_APP)/m95m04.c \
$(NK_APP)/max6675.c \
$(NK_APP)/mcp23017.c \
$(NK_APP)/mcp9808.c \
$(NK_APP)/nkymodem_cmd.c \
$(NK_APP)/pca9685.c \
$(NK_APP)/pcf8523.c \
$(NK_APP)/pcf8574.c \
$(NK_APP)/pcf8575.c \
$(NK_APP)/pins.c \
$(NK_APP)/rtc.c \
$(NK_APP)/test_cmds.c \
$(NK_APP)/w25q64.c \
$(NK_APP)/wdt.c \
$(NK_SRC)/nkarch_stm32.c \
$(NK_SRC)/nkchecked.c \
$(NK_SRC)/nkcli.c \
$(NK_SRC)/nkcrclib.c \
$(NK_SRC)/nkdatetime.c \
$(NK_SRC)/nkdbase.c \
$(NK_SRC)/nkdectab.c \
$(NK_SRC)/nkdriver_ds3231.c \
$(NK_SRC)/nkdriver_ina219.c \
$(NK_SRC)/nkdriver_lm75.c \
$(NK_SRC)/nkdriver_max6675.c \
$(NK_SRC)/nkdriver_mcp23017.c \
$(NK_SRC)/nkdriver_mcp9808.c \
$(NK_SRC)/nkdriver_pca9685.c \
$(NK_SRC)/nkdriver_pcf8523.c \
$(NK_SRC)/nkdriver_pcf8574.c \
$(NK_SRC)/nkdriver_pcf8575.c \
$(NK_SRC)/nkdriver_rtc_stm32.c \
$(NK_SRC)/nki2c.c \
$(NK_SRC)/nki2c_eeprom.c \
$(NK_SRC)/nki2c_stm.c \
$(NK_SRC)/nkinfile.c \
$(NK_SRC)/nkmcuflash.c \
$(NK_SRC)/nkoutfile.c \
$(NK_SRC)/nkpin.c \
$(NK_SRC)/nkprintf.c \
$(NK_SRC)/nkprintf_fp.c \
$(NK_SRC)/nkreadline.c \
$(NK_SRC)/nkrtc.c \
$(NK_SRC)/nkscan.c \
$(NK_SRC)/nksched.c \
$(NK_SRC)/nkserialize.c \
$(NK_SRC)/nkspi.c \
$(NK_SRC)/nkspiflash.c \
$(NK_SRC)/nkstring.c \
$(NK_SRC)/nkstrtod.c \
$(NK_SRC)/nkuart_stm32.c \
$(NK_SRC)/nkymodem.c \

# Our include directories...
C_INCLUDES += \
-I$(NK_ARCHCONFIG) \
-I$(NK_CONFIG) \
-I$(NK_INC) \
-I$(NK_APP_INC) \

CFLAGS += -D$(NK_PLATFORM) -DNK_PLATFORM=\"$(NK_PLATFORM)\" -DNK_VERSION_MAJOR=$(NK_VERSION_MAJOR)  -DNK_VERSION_MINOR=$(NK_VERSION_MINOR) -DNK_YEAR=$(NK_YEAR) -DNK_MONTH=$(NK_MONTH) -DNK_DAY=$(NK_DAY) -DNK_HOUR=$(NK_HOUR) -DNK_MINUTE=$(NK_MINUTE) -DNK_GIT_REV=$(NK_GIT_REV)

# Rebuild version.o if anything changed

$(BUILD_DIR)/version.o: $(OBJECTS) $(NK_APP)/VERSION_MAJOR $(NK_APP)/VERSION_MINOR $(NK_APP)/version.c

# Program Nucleo board

flash:
	/opt/STM32CubeProgrammer/bin/STM32_Programmer_CLI -c port=swd -e all -d build/${TARGET}.elf -v -rst
  
# Bump version numbers...

bump_minor:
	@expr `cat $(NK_APP)/VERSION_MINOR` + 1 >$(NK_APP)/VERSION_MINOR
	@echo
	@echo New version V`cat $(NK_APP)/VERSION_MAJOR`R`cat $(NK_APP)/VERSION_MINOR`
	@echo

bump_major:
	@expr `cat $(NK_APP)/VERSION_MAJOR` + 1 >$(NK_APP)/VERSION_MAJOR
	@echo 0 > $(NK_APP)/VERSION_MINOR
	@echo
	@echo New version V`cat $(NK_APP)/VERSION_MAJOR`R`cat $(NK_APP)/VERSION_MINOR`
	@echo
