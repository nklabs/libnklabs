Set up the Nucleo so that 8 MHz from ST-LINK (MCO output pin of ST-LINK MCU)
is used the HSE input clock.  This requires:

	SB55 Off
	SB54 On
	SB16 On
	SB50 On
	R35 and R37 removed

Boards marked with MB1136 C-02 or higher will be factory configured this
way.
