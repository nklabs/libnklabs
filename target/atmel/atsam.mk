# libnklabs paths

NK_SRC := ../../../../src
NK_APP := ../../../../app
NK_INC := ../../../../inc
NK_APP_INC := ../../../../app_inc
NK_CONFIG := ../../../../config
NK_ARCHCONFIG := ../../../../config_atsam

# A define for the platform
NK_PLATFORM := NK_PLATFORM_ATSAM

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

# Add out source files
OBJS += \
ads1115.o \
app_main.o \
at25df081.o \
basic_cmds.o \
button.o \
database.o \
ds1307.o \
ds3231.o \
ht16k33.o \
i2c.o \
ina219.o \
info_cmd.o \
lcd8574.o \
led.o \
lm75.o \
m24256.o \
m24c02.o \
m24m01.o \
m95040.o \
m95256.o \
m95m04.o \
max6675.o \
max7219.o \
mcp23017.o \
mcp4725.o \
mcp9808.o \
nkymodem_cmd.o \
parlcd.o \
pca9633.o \
pca9685.o \
pcf8523.o \
pcf8574.o \
pcf8575.o \
pins.o \
rtc.o \
rv8803.o \
tca9548a.o \
test_cmds.o \
tm1637.o \
tm1638.o \
w25q64.o \
wdt.o \
nkarch_atsam.o \
nkchecked.o \
nkcli.o \
nkcrclib.o \
nkdatetime.o \
nkdbase.o \
nkdectab.o \
nkdriver_ads1115.o \
nkdriver_aip31068.o \
nkdriver_ds1307.o \
nkdriver_ds3231.o \
nkdriver_hd44780.o \
nkdriver_ht16k33.o \
nkdriver_ina219.o \
nkdriver_lcd8574.o \
nkdriver_lm75.o \
nkdriver_max6675.o \
nkdriver_max7219.o \
nkdriver_mcp23017.o \
nkdriver_mcp4725.o \
nkdriver_mcp9808.o \
nkdriver_parlcd.o \
nkdriver_pca9633.o \
nkdriver_pca9685.o \
nkdriver_pcf8523.o \
nkdriver_pcf8574.o \
nkdriver_pcf8575.o \
nkdriver_rtc_atsam.o \
nkdriver_rv8803.o \
nkdriver_tca9548a.o \
nkdriver_tm1637.o \
nkdriver_tm1638.o \
nkfont_7seg.o \
nkfont_14seg.o \
nki2c.o \
nki2c_atsam.o \
nki2c_eeprom.o \
nkinfile.o \
nkmcuflash.o \
nkoutfile.o \
nkpin.o \
nkprintf_fp.o \
nkprintf.o \
nkreadline.o \
nkrtc.o \
nkscan.o \
nksched.o \
nkserialize.o \
nkspiflash.o \
nkspi.o \
nkstring.o \
nkstrtod.o \
nkuart_atsam.o \
nkymodem.o \


# Add our include files
DIR_INCLUDES +=  \
-I$(NK_INC) \
-I$(NK_APP_INC) \
-I$(NK_CONFIG) \
-I$(NK_ARCHCONFIG) \

OBJS_AS_ARGS := ${OBJS}

# List the dependency files
DEPS := $(OBJS:%.o=%.d)

DEPS_AS_ARGS = $(DEPS)

# Rebuild version.o if anything changed

version.o: $(OBJS) $(NK_APP)/VERSION_MAJOR $(NK_APP)/VERSION_MINOR $(NK_APP)/version.c

# Program using AtmelICE using and OpenOCD

flash: $(OUTPUT_FILE_PATH)
	(openocd --file atmelice.cfg)

# Program using JLINK using and OpenOCD

jlink: $(OUTPUT_FILE_PATH)
	(cd build; openocd --file jlink.cfg)

# Update files from atmel start

$(ATMEL_START_DIR)/gcc/Makefile: $(ATZIP_DIR)/$(ATZIP)
	@echo Updating atmel_start...
	rm -rf $(ATMEL_START_DIR)
	mkdir $(ATMEL_START_DIR)
	(cd $(ATMEL_START_DIR); unzip $(ATZIP_DIR)/$(ATZIP))
	(cd $(ATMEL_START_DIR); find . -type f -exec touch {} +)
	@echo
	@echo Apply patches
	@echo
	PATCHES=../../patches TARGET=`pwd`/$(ATMEL_START_DIR) ../../patches/apply
	@echo
	@echo Updated $(ATMEL_START_DIR) from $(ATZIP)
	@echo
	@echo Please type \'make\' again
	@echo
	exit 1
