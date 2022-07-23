all: obj ulx3s_impl1.mcs fpga.bin

# Get application version number

NK_VERSION_MAJOR := $(shell cat VERSION_MAJOR)
NK_VERSION_MINOR := $(shell cat VERSION_MINOR)

# Get date/time

NK_DATE := $(shell date -u -Iminute)
NK_YEAR := $(shell expr $(shell echo $(NK_DATE) | cut -b 1-4) + 0)
NK_MONTH := $(shell expr $(shell echo $(NK_DATE) | cut -b 6-7) + 0)
NK_DAY := $(shell expr $(shell echo $(NK_DATE) | cut -b 9-10) + 0)
NK_HOUR := $(shell expr $(shell echo $(NK_DATE) | cut -b 12-13) + 0)
NK_MINUTE := $(shell expr $(shell echo $(NK_DATE) | cut -b 15-16) + 0)

# A define for the platform
NK_PLATFORM := NK_PLATFORM_RISCV

# Get git hash as a string
# It is postfixed with -dirty if there are uncommitted changed; otherwise, it is postfixed with -clean.
NK_GIT_REV := \"$(shell git rev-parse HEAD)-$(shell if git diff-index --quiet HEAD --; then echo clean; else echo dirty; fi)\"

# Path to Lattice Diamond: usually diamond environement setup will have this
FOUNDRY ?= /opt/lattice/radiant/2.2/ispfpga

# GCC bin directory
TOOL = /opt/riscv32imc/bin

CC = ${TOOL}/riscv32-unknown-elf-gcc

AS = ${TOOL}/riscv32-unknown-elf-gcc

ASFLAGS = -march=rv32imc -c

CFLAGS = -I libnklabs -I . -O -march=rv32imc -Wall -Wconversion -Wunused -Wwrite-strings \
 -Wstrict-overflow=4 -Wmissing-include-dirs -Winit-self -Wcast-qual \
 -Wundef -Wlogical-op -Wmissing-declarations -Wformat \
 -Wmissing-format-attribute -Wformat-nonliteral \
 -Wswitch-enum -Wshadow -Wextra -Wformat-security \
 -D$(NK_PLATFORM) -DNK_PLATFORM=\"$(NK_PLATFORM)\" -DNK_VERSION_MAJOR=$(NK_VERSION_MAJOR)  -DNK_VERSION_MINOR=$(NK_VERSION_MINOR) -DNK_YEAR=$(NK_YEAR) -DNK_MONTH=$(NK_MONTH) -DNK_DAY=$(NK_DAY) -DNK_HOUR=$(NK_HOUR) -DNK_MINUTE=$(NK_MINUTE) -DNK_GIT_REV=$(NK_GIT_REV)

# -ffreestanding

LDFLAGS = $(MCU) -specs=nano.specs -Tsections.lds $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

OBJS = start.o main.o fpga.o nkarch_riscv.o nkuart_riscv.o nkymodem_cmd.o tunnel_fpga.o database.o \
basic_cmds.o \
info_cmd.o \
libnklabs/nkchecked.o \
libnklabs/nkdirect.o \
libnklabs/nkcli.o \
libnklabs/nkcrclib.o \
libnklabs/nkdbase.o \
libnklabs/nkinfile.o \
libnklabs/nkmcuflash.o \
libnklabs/nkoutfile.o \
libnklabs/nkprintf.o \
libnklabs/nkreadline.o \
libnklabs/nkscan.o \
libnklabs/nksched.o \
libnklabs/nkserialize.o \
libnklabs/nkstring.o \
libnklabs/nkymodem.o

# Keep them in a subdirectory
MOST_OBJS = $(addprefix obj/, $(OBJS))

SUBDIR_OBJS = $(MOST_OBJS) obj/version.o

# Rebuild version.o if any other file changed
obj/version.o: $(MOST_OBJS) VERSION_MAJOR VERSION_MINOR

# Use Diamond "deployment tool" to combine FPGA configuration file with firmware
# Note that byte mirroring must be enabled for the firmware.
ulx3s_impl1.mcs: ../impl1/ulx3s_impl1.bit ulx3s.bin
	${FOUNDRY}/../bin/lin64/ddtcmd -oft -advanced -dev LFE5U-85F \
		-if ../impl1/ulx3s_impl1.bit -format int -flashsize 128 \
		-usermirror -userdata 2 \
		-userfile ulx3s.bin -address 0x00E00000 \
		-userfile ulx3s.bin -address 0x00600000 \
		-of ulx3s_impl1.mcs
	cp ulx3s_impl1.mcs ../impl1/ulx3s_impl1.mcs

# create fpga.bin from .bit file

fpga.bin: ../impl1/ulx3s_impl1.bit ./exotobin ./brev
	/bin/rm -f fpga.exo
	${FOUNDRY}/../bin/lin64/ddtcmd -oft -mot -dev LFE5U-85F -if ../impl1/ulx3s_impl1.bit -of fpga.exo
	${FOUNDRY}/../bin/lin64/ddtcmd -oft -int -dev LFE5U-85F -if ../impl1/ulx3s_impl1.bit -of fpga.mcs
	./exotobin fpga.exo -o fpga.rbin
	./brev <fpga.rbin >fpga.bin

# .exo file to .bin file converter

exotobin: exotobin.c
	cc -o exotobin exotobin.c

# utility to reverse bits in each byte

brev: brev.c
	cc -o brev brev.c

ulx3s: ulx3s.mem ulx3s.hex

ulx3s.elf: sections.lds $(SUBDIR_OBJS)
	${TOOL}/riscv32-unknown-elf-ld -Bstatic -T sections.lds --strip-debug -o ulx3s.elf -Map=ulx3s.map --cref $(SUBDIR_OBJS) ${TOOL}/../lib/gcc/riscv32-unknown-elf/8.2.0/libgcc.a

# Word hex file for bus_rom.v
ulx3s.mem: ulx3s.bin bintov
	./bintov ulx3s.bin ulx3s.mem

# Byte hex file for simulation
ulx3s.hex: ulx3s.bin bintob
	./bintob ulx3s.bin ulx3s.hex

bintov: bintov.c
	cc -o bintov bintov.c

bintob: bintob.c
	cc -o bintob bintob.c

ulx3s.bin: ulx3s.elf
	${TOOL}/riscv32-unknown-elf-objcopy -O binary ulx3s.elf ulx3s.bin

clean:
	rm -f ulx3s.elf ulx3s.hex ulx3s.bin ulx3s.map
	rm -rf obj

# Automatically create object file directory

obj:
	mkdir obj

# Bump version numbers...

bump_minor:
	@expr `cat VERSION_MINOR` + 1 >VERSION_MINOR
	@echo
	@echo New version V`cat VERSION_MAJOR`R`cat VERSION_MINOR`
	@echo

bump_major:
	@expr `cat VERSION_MAJOR` + 1 >VERSION_MAJOR
	@echo 0 > VERSION_MINOR
	@echo
	@echo New version V`cat VERSION_MAJOR`R`cat VERSION_MINOR`
	@echo

# include dependancy files if they exist
-include $(SUBDIR_OBJS:.o=.d)

# compile and generate dependency info
obj/%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o obj/$*.o
	@$(CC) -MM $(CFLAGS) $*.c > obj/$*.d
# Improve dependency file produced by gcc... allows files to be renamed and source files to exist
# in subdirectories.
	@mv -f obj/$*.d obj/$*.d.tmp
	@sed -e 's|.*:|obj/$*.o:|' < obj/$*.d.tmp > obj/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < obj/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> obj/$*.d
	@rm -f obj/$*.d.tmp

obj/%.o: %.S
	$(CC) -c $(CFLAGS) $*.S -o obj/$*.o
