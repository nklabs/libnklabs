
# Nucleo-STM32L073RZT6 Target

## Nucleo Board

The nucleo board hardware was configured for the debug MCO to be used as the STM32L073 oscillator. 
However, the MCU did not run when configured to use the external HSE and no 8MHz was observed on the PH_OSC pin with a scope. Thus, this project
is only tested using the internal MCU oscillator.

* See section 6.7.1 in the User Guide: 

[https://www.st.com/resource/en/user_manual/um1724-stm32-nucleo64-boards-mb1136-stmicroelectronics.pdf](https://www.st.com/resource/en/user_manual/um1724-stm32-nucleo64-boards-mb1136-stmicroelectronics.pdf)

## MCU Configuration

* Pin configurations and names follow the conventions in other projects. 
* Clock configuration follows that in the nucleo-g071 build target project. 
Note that this project used the internal RC oscillator and has not been tested with the nucleo board debug MCU MCO as an external oscillator.
* PA5 is overloaded: it is connected to both the on-board LED and to Arduino
D13 (often used for SPI SCLK).  The software disabled the blinking LED
whenever Arduino SPI transactions are performed.

## Development Tools

This project has been built and flashed running Ubuntu 18.04 WSL2 on Windows. There is no reason it should not work on native Linux, but this has not been tested. 