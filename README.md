
About NIOSDuino
---------------

NIOSDuino is an adaptation of Arduino libraries which can run on a system
built with QSys tool (formely SoPC Builder) from Quartus Prime. This
enables Arduino code to be reused on Altera FPGA development boards and
use Arduino-compatible 3.3V hardware modules (or shields) with these boards.
Currently, Arduino modules with UART, SPI and plain diginal IO are supported.
I2C and PWM support are planned. Analog channels are only possible with
boards which have ADC, and their support is not planned as of yet.

Getting started
---------------

Here's a quickstart guide which should get you started if you have an FPGA
board with SDRAM memory. If you're planning to purchase a dev board, consider
reading the Hardware section below.

1. Install Quartus Prime with support package for your FPGA. I used version
17.0 with a Cyclone IV IC (specifically, EP4CE10).

2. Create/open a project with clock, reset and RAM pins assigned to correct
locations.

3. Run Qsys tool and open nios_sdram.qsys. Remove EPCS controller if you
don't have the serial flash chip or don't plan to use it as software storage.
If you do, you will need to change the CPU reset location from EPCS to RAM.
Generate the HDL (and the symbol, if you plan to use graphical schematic
editor).

4. Instantiate the generated system in your project and connect its outputs
to FPGA pins. If your board has LEDs, connect one to PIO[13].

5. Build the project and program the FPGA.

6. Open the NIOS II Eclipse IDE. Create a BSP (board support package) project
for the generated NIOS system (nios_sdram.sopcinfo). Use the "Altera HAL"
type, not the RTOS one. Set the STDIN/STDOUT to jtag_uart_0, and timestamp
and sys_clk timers to timer_0.

7. Create a NIOS II Application project called nios_duino based on the BSP
you just created. Add files from nios_duino folder to your project, keeping
the folder structure. Then add a few settings to project properties:

 - add definitions: `__AVR__` and `ARDUINO=185` to C and C++ symbols

 - add "arduino" folder and folders of libraries you're going to use
(e.g. "arduino/SPI/src") to C and C++ include directories

8. Complile and run the nios_duino software. You should see the LED you've
connected to PIO[13] blinking, and a timestamp should be printed to the
Eclipse console every 2 seconds.

Hardware
--------

Basically, what you need is a dev board with an FPGA capable of implementing
a NIOS II CPU, enough on-board RAM to run the code, and enoug flash storage
if you want your code to be persistent.

- FPGA - Ideally, you'll need any IC of Cyclone, Stratix or Arria families.
It should also be possible to implement NIOS on a MAX 10 device, but you
have to make sure the FPGA is big enough to support the CPU. Also check
that the IO voltage supported by the FPGA is compatible with the hadware
modules you have.

Note that older Cyclone II family is no logner supported by new versions
of Quartus IDE, while older Quartus versions don't have the toolchain
extensions (like C++1x) that Arduino code requires. While it's still
possible to run some older Arduino libraries using the old toolchain, it
was decided it's not worth the effort, so Cyclone II is not supported.

- RAM - NIOSDuino requires much more RAM than Arduino. Unless you have flash
which supports code execution, all program code will have to reside in RAM,
plus there's a lot of overhead in using POSIX functions to mimic the AVR
library. Depending on the libraries you will use, you should expect as much
as 500 kB of RAM to be required. Luckily, most FPGA boards come with at least
several MB of SRAM, or tens of MB of SDRAM.

- Flash - The typical solution present on Altera dev boards is to use the same
EPCS serial flash chip for FPGA configuration and program storage. Note that
serial flash cannot be used as code ROM - EPCS component provides a bootloader
which reads the contents of the flash in the RAM and executes the code from
there. Unless you have a NOR flash, you will still need the same amount of RAM
to run your code.

Cheaper dev boards come with EPCS4 chip which only provides 512kB of storage
shared between the FPGA configuration data and the software, so it will
probably be not big enough. Try to get at least EPCS16 if you plan to store
your code on the board.

SoPC
----

This project includes a sample SoPC system (nios_sdram.qsys) which should
suit most boards with SDRAM and EPCS flash. In case you need to customize
the SoPC, here's a description of components which should/could be included
in the QSys system to be supported by NIOSDuino:

- NIOS CPU. Only the "tiny" variety is available without restrictions,
bigger varieties will be either time-limited or only work while the JTAG
cable is connected if you use free edition of Quartus.

- SRAM/SDRAM controller. Unless your FPGA has 500kB of internal RAM or more
you'll have to use whatever RAM you have on your dev board.

- PIO, has to be named pio_0. This component can emulate the digital pins
of Arduino, so it's required for pretty much anything. Be sure to configure
it as Bidir, with bits which can be set/reset individually. Obviously, you
will have as many pins as you have configured, with a max of 32.

- UART and SPI controllers, have to be named spi_0, uart_0. These
will be used to interact with hadware modules you want to connect to your
dev board. It is recommended to make UART baud rate software-configurable.
It should be possible to include several UARTs and easily integrate them
in Arduino framework as `Serial1`, `Serial2`, etc.

- EPCS controller. You only need it if you plan to store your software in EPCS.
[Here](https://www.altera.com/support/support-resources/knowledge-base/solutions/rd04112006_450.html)'s
an article describing how to program the EPCS device with FPGA SOF file and
software ELF file simultaneously.

- JTAG UART, 16x2 LCD or any other compoment which can be selected as
STDIN/STDOUT.

- Timer. This will enable time functions, such as `millis()` and `micros()`.

Programming model
-----------------

Digital pins are accessible by index, starting from 0. That is,
`digitalWrite(0, LOW)` will set PIO pin 0 to LOW.

Unlike actual AVR chips, QSys components don't share pins with each other.
This means you can use all the PIO pins and SPI/UART modules in parallel.
There's also no need to e.g. configure pin direction for SPI/UART pins.
Note that existing libraries may still assume that shared pins are used, and
configure them accordingly.

Hardware UARTs are accessible as `Serial0`, `Serial1`, etc. Only baudrate
setting is taken into account in `SerialN.begin()`, bit settings have to be
configured in QSys and cannot be changed at runtime.

Serial is reserved to be whatever component you chose as STDIN/STDOUT,
which can be a UART, a JTAG UART or even an LCD. Obviously, any baudrate
or bit settings given in `Serial.begin()` are ignored. If a UART is to be
used in a sketch which refers to `Serial`, it is recommended to

    #define Serial Serial0

Note that the same UART should not be used as STDIN/STDOUT and as `SerialN`
device at the same time.

SPI controller manages 3 pins - MISO, MOSI and CLK. SS signal is not used
and should not be routed to FPGA pin. Arduino libraries typically expect
the user to assign a regular diginal IO pin to be used as chipselect. As
a result, transaction management mechanism of SPI controller is not used.
