# Where is atmel_start code?
ATMEL_START_DIR := ../atmel_start
ATZIP := atsamd21pro.atzip
# Where is ATZIP relative to ATMEL_START_DIR?
ATZIP_DIR := ../

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
# Get them from atmel-start's Makefile
SUB_DIRS := $(shell make -C ../atmel_start/gcc -p -n | egrep '^SUB_DIRS ' | cut -b 11- )

# List the object files
# Get them from atmel-start's Makefile
OBJS := $(shell make -C ../atmel_start/gcc -p -n | egrep '^OBJS ' | cut -b 7-)
# But we don't want main.o
OBJS := $(filter-out main.o, $(OBJS))



# List the directories containing header files
# Get them from atmel-start's Makefile, but insert ../atmel_start prefix
DIR_INCLUDES := $(subst ..,../atmel_start,$(shell make -C ../atmel_start/gcc -p -n | egrep '^DIR_INCLUDES ' | cut -b 15- ))


OUTPUT_FILE_NAME :=AtmelStart
QUOTE := "
OUTPUT_FILE_PATH +=$(OUTPUT_FILE_NAME).elf
OUTPUT_FILE_PATH_AS_ARGS +=$(OUTPUT_FILE_NAME).elf

# All Target
all: $(SUB_DIRS) $(OUTPUT_FILE_PATH)

include ../../atsam.mk

vpath %.c $(NK_SRC) $(NK_APP) $(ATMEL_START_DIR)
vpath %.s $(NK_SRC) $(NK_APP) $(ATMEL_START_DIR)
vpath %.S $(NK_SRC) $(NK_APP) $(ATMEL_START_DIR)

# Linker target

$(OUTPUT_FILE_PATH): $(OBJS) version.o
	@echo Building target: $@
	@echo Invoking: ARM/GNU Linker
	$(QUOTE)arm-none-eabi-gcc$(QUOTE) -o $(OUTPUT_FILE_NAME).elf $(OBJS_AS_ARGS) version.o -Wl,--start-group -lm -Wl,--end-group -mthumb \
-Wl,-Map="$(OUTPUT_FILE_NAME).map" --specs=nano.specs -Wl,--gc-sections -mcpu=cortex-m0plus \
 \
-T"samd21j18a_flash.ld" \
-L"samd21a/gcc/gcc"
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

%.o: %.c $(ATMEL_START_DIR)/gcc/Makefile
	@echo Building file: $<
	@echo ARM/GNU C Compiler
	$(QUOTE)arm-none-eabi-gcc$(QUOTE) -x c -mthumb -DDEBUG -Os -ffunction-sections -mlong-calls -g3 -Wall -c -std=gnu99 \
-D__SAMD21J18A__ -mcpu=cortex-m0plus  \
$(DIR_INCLUDES) \
-DNK_PLATFORM=\"$(NK_PLATFORM)\" -D$(NK_PLATFORM) -DNK_VERSION_MAJOR=$(NK_VERSION_MAJOR)  -DNK_VERSION_MINOR=$(NK_VERSION_MINOR) -DNK_YEAR=$(NK_YEAR) -DNK_MONTH=$(NK_MONTH) -DNK_DAY=$(NK_DAY) -DNK_HOUR=$(NK_HOUR) -DNK_MINUTE=$(NK_MINUTE) -DNK_GIT_REV=$(NK_GIT_REV) \
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
