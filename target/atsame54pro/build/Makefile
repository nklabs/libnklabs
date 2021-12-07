# libnklabs paths

NK_SRC := ../../../src
NK_APP := ../../../app
NK_INC := ../../../inc
NK_CONFIG := ../../../config
NK_ARCHCONFIG := ../../../config_atsam

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
NK_GIT_REV := "$(shell git rev-parse HEAD)-$(shell if git diff-index --quiet HEAD --; then echo clean; else echo dirty; fi)"

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
# Get them from atmel-start's Makefile
SUB_DIRS += $(shell make -C ../atmel_start/gcc -p -n | egrep '^SUB_DIRS ' | cut -b 11- )

# Object files
# Get them from atmel-start's Makefile
OBJS += $(shell make -C ../atmel_start/gcc -p -n | egrep '^OBJS ' | cut -b 7-)
# But we don't want main.o
OBJS := $(filter-out main.o, $(OBJS))

# Add out object files
OBJS +=  \
spiflash_atsam.o \
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
nkstrtod.o

# Atmel quotes these here for some reason..
OBJS_AS_ARGS = ${OBJS}

# List the directories containing header files
# Get them from atmel-start's Makefile, but insert ../atmel_start prefix
DIR_INCLUDES += $(subst ..,../atmel_start,$(shell make -C ../atmel_start/gcc -p -n | egrep '^DIR_INCLUDES ' | cut -b 15- ))

# Add our include files
DIR_INCLUDES +=  \
-I$(NK_INC) \
-I$(NK_CONFIG) \
-I$(NK_ARCHCONFIG)

# List the dependency files
DEPS := $(OBJS:%.o=%.d)

DEPS_AS_ARGS += $(shell make -C ../atmel_start/gcc -p -n | egrep '^DEPS_AS_ARGS ' | cut -b 15-)
DEPS_AS_ARGS +=  \
"spiflash_atsam.d" \
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
"nkstrtod.d"

OUTPUT_FILE_NAME :=AtmelStart
QUOTE := "
OUTPUT_FILE_PATH +=$(OUTPUT_FILE_NAME).elf
OUTPUT_FILE_PATH_AS_ARGS +=$(OUTPUT_FILE_NAME).elf

vpath %.c $(NK_SRC) $(NK_APP) ../atmel_start/
vpath %.s $(NK_SRC) $(NK_APP) ../atmel_start/
vpath %.S $(NK_SRC) $(NK_APP) ../atmel_start/

# All Target
all: $(SUB_DIRS) $(OUTPUT_FILE_PATH)

# Rebuild version.o if anything changed
version.o: $(OBJS) $(NK_APP)/VERSION_MAJOR $(NK_APP)/VERSION_MINOR $(NK_APP)/version.c

# Linker target

$(OUTPUT_FILE_PATH): $(OBJS) version.o ../atmel_start/gcc/Makefile
	@echo Building target: $@
	@echo Invoking: ARM/GNU Linker
	$(QUOTE)arm-none-eabi-gcc$(QUOTE) -o $(OUTPUT_FILE_NAME).elf $(OBJS_AS_ARGS) version.o -Wl,--start-group -lm -Wl,--end-group -mthumb \
-Wl,-Map="$(OUTPUT_FILE_NAME).map" --specs=nano.specs -Wl,--gc-sections -mcpu=cortex-m4 \
 \
-T"same54p20a_flash.ld" \
-L"../gcc/gcc"
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




%.o: %.c ../atmel_start/gcc/Makefile
	@echo Building file: $<
	@echo ARM/GNU C Compiler
	$(QUOTE)arm-none-eabi-gcc$(QUOTE) -x c -mthumb -DDEBUG -Os -ffunction-sections -mlong-calls -g3 -Wall -c -std=gnu99 \
-D__SAME54P20A__ -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 \
$(DIR_INCLUDES) \
-DNK_VERSION_MAJOR=$(NK_VERSION_MAJOR)  -DNK_VERSION_MINOR=$(NK_VERSION_MINOR) -DNK_YEAR=$(NK_YEAR) -DNK_MONTH=$(NK_MONTH) -DNK_DAY=$(NK_DAY) -DNK_HOUR=$(NK_HOUR) -DNK_MINUTE=$(NK_MINUTE) -DNK_GIT_REV=$(NK_GIT_REV) \
-MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"  -o "$@" "$<"
	@echo Finished building: $<

%.o: %.s
	@echo Building file: $<
	@echo ARM/GNU Assembler
	$(QUOTE)arm-none-eabi-as$(QUOTE) -x c -mthumb -DDEBUG -Os -ffunction-sections -mlong-calls -g3 -Wall -c -std=gnu99 \
-D__SAME54P20A__ -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 \
$(DIR_INCLUDES) \
-MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"  -o "$@" "$<"
	@echo Finished building: $<

%.o: %.S
	@echo Building file: $<
	@echo ARM/GNU Preprocessing Assembler
	$(QUOTE)arm-none-eabi-gcc$(QUOTE) -x c -mthumb -DDEBUG -Os -ffunction-sections -mlong-calls -g3 -Wall -c -std=gnu99 \
-D__SAME54P20A__ -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 \
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

jlink:
	(cd build; openocd --file jlink.cfg)

# Update files from atmel start

../atmel_start/gcc/Makefile: ../atsame54pro.atzip
	rm -rf ../atmel_start
	mkdir ../atmel_start
	(cd ../atmel_start; unzip ../atsame54pro.atzip)
	(cd ../atmel_start; find . -type f -exec touch {} +)
