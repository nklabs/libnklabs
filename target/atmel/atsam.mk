# libnklabs paths

NK_SRC := ../../../../src
NK_APP := ../../../../app
NK_INC := ../../../../inc
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
button.o \
led.o \
wdt.o \
nki2c_atsam.o \
nki2c.o \
nki2c_eeprom.o \
nkspi.o \
i2c.o \
at25df081.o \
m95040.o \
m95256.o \
m95m04.o \
basic_cmds.o \
test_cmds.o \
info_cmd.o \
database.o \
nkymodem_cmd.o \
app_main.o \
nkdatetime.o \
nki2crtc_ds3231.o \
extrtc.o \
nkextrtc.o \
nkprintf_fp.o \
nkmcurtc.o \
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
nkstrtod.o

# Add our include files
DIR_INCLUDES +=  \
-I$(NK_INC) \
-I$(NK_CONFIG) \
-I$(NK_ARCHCONFIG)

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
