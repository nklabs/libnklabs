#include "custom_ops.S"

	.section .text
	.global irq

reset_vec:
	// no more than 16 bytes here !
	picorv32_maskirq_insn(zero, zero)
	j start

/* Interrupt handler */

	.balign 16
irq_vec:
	/* save registers */
	sw gp,   0*4+0x080(zero)
	sw x1,   1*4+0x080(zero)
	sw x2,   2*4+0x080(zero)
	sw x3,   3*4+0x080(zero)
	sw x4,   4*4+0x080(zero)
	sw x5,   5*4+0x080(zero)
	sw x6,   6*4+0x080(zero)
	sw x7,   7*4+0x080(zero)
	sw x8,   8*4+0x080(zero)
	sw x9,   9*4+0x080(zero)
	sw x10, 10*4+0x080(zero)
	sw x11, 11*4+0x080(zero)
	sw x12, 12*4+0x080(zero)
	sw x13, 13*4+0x080(zero)
	sw x14, 14*4+0x080(zero)
	sw x15, 15*4+0x080(zero)
	sw x16, 16*4+0x080(zero)
	sw x17, 17*4+0x080(zero)
	sw x18, 18*4+0x080(zero)
	sw x19, 19*4+0x080(zero)
	sw x20, 20*4+0x080(zero)
	sw x21, 21*4+0x080(zero)
	sw x22, 22*4+0x080(zero)
	sw x23, 23*4+0x080(zero)
	sw x24, 24*4+0x080(zero)
	sw x25, 25*4+0x080(zero)
	sw x26, 26*4+0x080(zero)
	sw x27, 27*4+0x080(zero)
	sw x28, 28*4+0x080(zero)
	sw x29, 29*4+0x080(zero)
	sw x30, 30*4+0x080(zero)
	sw x31, 31*4+0x080(zero)

	/* call interrupt handler C function */

	// Interrupt handler has his own stack?
	// lui sp, %hi(0x8000)
	// addi sp, sp, %lo(0x8000)

	// arg0 = address of regs
	lui a0, %hi(0x080)
	addi a0, a0, %lo(0x080)

	// arg1 = interrupt type
	addi a1, tp, 0

	// call to C function
	jal ra, irq

	/* restore registers */

	// new irq_regs address returned from C code in a0
	// ???
	addi a1, zero, 0x080
	beq a0, a1, 1f
	ebreak
1:

	lw gp,   0*4+0x080(zero)
	lw x1,   1*4+0x080(zero)
	lw x2,   2*4+0x080(zero)
	// do not restore x3 (gp)
	lw x4,   4*4+0x080(zero)
	lw x5,   5*4+0x080(zero)
	lw x6,   6*4+0x080(zero)
	lw x7,   7*4+0x080(zero)
	lw x8,   8*4+0x080(zero)
	lw x9,   9*4+0x080(zero)
	lw x10, 10*4+0x080(zero)
	lw x11, 11*4+0x080(zero)
	lw x12, 12*4+0x080(zero)
	lw x13, 13*4+0x080(zero)
	lw x14, 14*4+0x080(zero)
	lw x15, 15*4+0x080(zero)
	lw x16, 16*4+0x080(zero)
	lw x17, 17*4+0x080(zero)
	lw x18, 18*4+0x080(zero)
	lw x19, 19*4+0x080(zero)
	lw x20, 20*4+0x080(zero)
	lw x21, 21*4+0x080(zero)
	lw x22, 22*4+0x080(zero)
	lw x23, 23*4+0x080(zero)
	lw x24, 24*4+0x080(zero)
	lw x25, 25*4+0x080(zero)
	lw x26, 26*4+0x080(zero)
	lw x27, 27*4+0x080(zero)
	lw x28, 28*4+0x080(zero)
	lw x29, 29*4+0x080(zero)
	lw x30, 30*4+0x080(zero)
	lw x31, 31*4+0x080(zero)

	picorv32_retirq_insn()

start:

# zero-initialize register file
	addi x1, zero, 0
# x2 (sp) is initialized by reset
	addi x3, zero, 0
	addi x4, zero, 0
	addi x5, zero, 0
	addi x6, zero, 0
	addi x7, zero, 0
	addi x8, zero, 0
	addi x9, zero, 0
	addi x10, zero, 0
	addi x11, zero, 0
	addi x12, zero, 0
	addi x13, zero, 0
	addi x14, zero, 0
	addi x15, zero, 0
	addi x16, zero, 0
	addi x17, zero, 0
	addi x18, zero, 0
	addi x19, zero, 0
	addi x20, zero, 0
	addi x21, zero, 0
	addi x22, zero, 0
	addi x23, zero, 0
	addi x24, zero, 0
	addi x25, zero, 0
	addi x26, zero, 0
	addi x27, zero, 0
	addi x28, zero, 0
	addi x29, zero, 0
	addi x30, zero, 0
	addi x31, zero, 0

# Update LEDs
	li a0, 0x0200000c
	li a1, 1
	sw a1, 0(a0)

# zero initialize entire scratchpad memory
	li a0, 0x00000000
setmemloop:
	sw a0, 0(a0)
	addi a0, a0, 4
	blt a0, sp, setmemloop

# Update LEDs
	li a0, 0x0200000c
	li a1, 3
	sw a1, 0(a0)

# copy data section
	la a0, _sidata
	la a1, _sdata
	la a2, _edata
	bge a1, a2, end_init_data
loop_init_data:
	lw a3, 0(a0)
	sw a3, 0(a1)
	addi a0, a0, 4
	addi a1, a1, 4
	blt a1, a2, loop_init_data
end_init_data:

# Update LEDs
	li a0, 0x0200000c
	li a1, 7
	sw a1, 0(a0)

# zero-init bss section
	la a0, _sbss
	la a1, _ebss
	bge a0, a1, end_init_bss
loop_init_bss:
	sw zero, 0(a0)
	addi a0, a0, 4
	blt a0, a1, loop_init_bss
end_init_bss:

# Update LEDs
	li a0, 0x0200000c
	li a1, 15
	sw a1, 0(a0)

# Enable timer interrupts..
	lui x1, %hi(12000000)
	addi x1, x1, %lo(12000000)
	picorv32_timer_insn(zero, x1);

# call main
	call main
loop:
	j loop

	.global flashio_worker_begin
	.global flashio_worker_end

	.balign 4

# We copy this function to RAM.  When called, it temporarily
# disables the memory mapped SPI-flash access, performs a SPI transaction
# to/from a buffer in RAM, re-enables access and returns.

# SPI ctrl register bits:
#   3:0   data
#   4     clock
#   5     cs_l
#   11:8  oe
#   20    cont
#   21    qspi
#   22    ddr
#   31    config_en

# a0 ... data pointer
# a1 ... data length
# a2 ... optional WREN cmd (0 = disable)

flashio_worker_begin:

# address of SPI ctrl reg
	li   t0, 0x02000000

# Set CS high, IO0 is output
	li   t1, 0x120
	sh   t1, 0(t0)

# Enable Manual SPI Ctrl
	sb   zero, 3(t0)

# Send optional WREN cmd
	beqz a2, flashio_worker_L1
	li   t5, 8               # Bit count
	andi t2, a2, 0xff        # Data byte to t2

flashio_worker_L4:
	srli t4, t2, 7           # Get t2.7 into t4.0
	sb   t4, 0(t0)           # Output data bit
	ori  t4, t4, 0x10        # Clock rising edge
	sb   t4, 0(t0)
	slli t2, t2, 1           # Shift left
	andi t2, t2, 0xff        # We only care about the low 8 bits
	addi t5, t5, -1          # Done?
	bnez t5, flashio_worker_L4   # Loop
	sb   t1, 0(t0)           # Lower clock, raise chip select

# SPI transfer
flashio_worker_L1:
	beqz a1, flashio_worker_L3
	li   t5, 8
	lbu  t2, 0(a0)
flashio_worker_L2:
	srli t4, t2, 7
	sb   t4, 0(t0)
	ori  t4, t4, 0x10
	sb   t4, 0(t0)
	lbu  t4, 0(t0)
	andi t4, t4, 2
	srli t4, t4, 1
	slli t2, t2, 1
	or   t2, t2, t4
	andi t2, t2, 0xff
	addi t5, t5, -1
	bnez t5, flashio_worker_L2
	sb   t2, 0(a0)
	addi a0, a0, 1
	addi a1, a1, -1
	j    flashio_worker_L1

flashio_worker_L3:
	sb   t1, 0(t0)           # Lower clock, raise chip select

# Wait until flash not busy
flashio_worker_check:
	li	t5, 16		# Transfer count
	li	t2, 0x05	# Status command

flashio_worker_L5:
	srli t4, t2, 7          # Get t2.7 into t4.0
	sb   t4, 0(t0)		# Output data bit
	lbu  t3, 0(t0)		# Input data bit
	ori  t4, t4, 0x10	# Rising clock edge
	sb   t4, 0(t0)
	slli t2, t2, 1		# Next bit
	andi t2, t2, 0xff	# Only care about lower 8
	addi t5, t5, -1		# Dec. count
	bnez t5, flashio_worker_L5	# Loop if not done
	sb   t1, 0(t0)		# Raise chip select
	andi t3, t3, 2		# Busy high?
	bnez t3, flashio_worker_check   # Branch if is.

# No longer busy, safe to go 
# Back to MEMIO mode
	li   t1, 0x80
	sb   t1, 3(t0)

	ret

	.balign 4
flashio_worker_end:
