
About NIOSDuino
---------------

NIOSDuino is an adaptation of Arduino core libraries which run on a system
built with QSys tool (formely SoPC Builder) from Quartus Prime. This
enables Arduino code to be reused on Altera FPGA development boards and
Arduino-compatible hardware modules (or shields) to be used with these boards.
Currently, Arduino modules with UART, I2C, SPI and plain diginal IO interfaces
are supported. PWM support (aka `AnalogWrite()`) might be implemented in the
future. Analog channels (aka `AnalogRead()`) would require boards
which have ADC, and their support is not planned as of yet. Internal EEPROM
is currently simulated with a 1KB RAM block, but it might be implemented
using EPCS/EPCQ EEPROM which is present virtually on all Altera FPGA boards.

Getting started
---------------

Here's a quickstart guide which should get you started if you have an FPGA
board with optional SDRAM memory and EPCS flash. If you're planning to
purchase a dev board, consider reading the Hardware section below.

1. Install Quartus Prime with support package for your FPGA. I used version
17.0 with a Cyclone IV IC (specifically, EP4CE10).

2. Create/open a project with clock, reset, EPCS and SDRAM (if you have SDRAM)
pins assigned to correct locations.

3. Run Qsys tool and open nios_sdram.qsys (if you plan to use SDRAM) or
nios_onchip.qsys (if you'll be using internal RAM). Generate the HDL (and the
symbol, if you plan to use graphical schematic editor).

4. Instantiate the generated system in your project and connect its outputs
to FPGA pins. If your board has LEDs, connect one to PIO[13].

5. Build the project and program the FPGA.

6. Open the NIOS II Eclipse IDE. Create a BSP (board support package) and
NIOS II application project from template. If you have less than 100KB of RAM
(typically, when using on-chip memory), pick "Hello world small" as a base,
otherwise pick regular "Hello world".  Set the STDIN/STDOUT to jtag_uart_0,
and sys_clk timer to timer_0. Additionally, if you have picked
"Hello world small", go to BSP advanced settings and enable C++ support.


7. In the NIOS II Application project, remove hello_world.c file, and
add files from nios_duino folder, keeping the folder structure. Then add
a few settings to project properties:

 - add `-std=gnu++11` to the compiler switches

 - add definitions: `__AVR__` and `ARDUINO=185` to C and C++ symbols

 - add "arduino" folder and folders of libraries you're going to use
(e.g. "arduino/SPI/src") to C and C++ include directories

 - enable dead code elimination with `-ffunction-sections` and `--gc-sections`

The easiest way to do this is to edit the following lines in the Makefile:

    ALT_INCLUDE_DIRS := arduino arduino/Wire/src arduino/SPI/src
    ALT_CXXFLAGS := -std=gnu++11
    ALT_CPPFLAGS := -D__AVR__ -DARDUINO=185 -ffunction-sections
    ALT_LDFLAGS := -Wl,--gc-sections

8. Complile and run the software. You should see the LED you've connected
to PIO[13] blinking, and a timestamp should be printed to the Eclipse
console every ~2 seconds. Use this timestamp to calibrate the `delay()` and
`delayMicroseconds()` macros so that `delay(1000)` corresponds to one second.

Hardware
--------

NIOSDuino should run on any dev board with an FPGA capable of implementing
a NIOS II CPU, enough on-board RAM to run the code, and enoug flash storage
if you want your code to be persistent.

- FPGA - Ideally, you'll need any IC of Cyclone, or Arria families.
Stratix is even more powerful, but requires a commercial Quartus license.
It should also be possible to implement NIOS on a MAX 10 device, but you
have to make sure the FPGA is big enough to support the CPU. Also check
that the IO voltage supported by the FPGA is compatible with the hadware
modules you have. Typically, 3.3V modules should work fine.

Note that older Cyclone II family is no logner supported by new versions
of Quartus IDE, while older Quartus versions don't have the toolchain
extensions (like C++1x) that Arduino code requires. While it's still
possible to run some older Arduino libraries using the old toolchain, it
was decided it's not worth the effort, so Cyclone II is not supported.

- RAM - NIOSDuino requires much more RAM than Arduino. Unless you have flash
which supports code execution, all program code will have to reside in RAM,
plus there's a lot of overhead in using HAL functions to mimic the AVR
library. When using small C library and optimization (-O3 or -Os), a typical
Arduino program should fit into ~64KB. With standard C library and light
optimization (-01) you will need 128KB or more.

If you have an FPGA with 500 kbit of internal memory or more, you can try
on-chip RAM, which is significanly faster than pretty much any off-chip
solution, unless you implement cache. Many FPGA boards come with external
SRAM or SDRAM, which will offer you much more flexibility.

- Flash - The typical solution present on Altera dev boards is to use the same
EPCS serial flash chip for FPGA configuration and program storage. Typically,
you don't want to execute code from flash, which is slow and cannot be
programmed by the debugger. Rather, you will want to use a "boot copier" which
copies flash contents into RAM on a reset, and run the code from RAM.

Cheaper dev boards come with EPCS4 chip which only provides 512kB of storage
shared between the FPGA configuration data and the software, so it will
probably be not big enough. Try to get at least EPCS16 if you plan to store
your code on the board.

SoPC
----

This project includes a sample SoPC system (nios_sdram.qsys/nios_onchip.qsys)
which should suit most boards. In case you need to customize the SoPC, here's
a description of components which should/could be included in the QSys system
to be supported by NIOSDuino:

- NIOS CPU. Only the "tiny" variety is available without restrictions,
bigger varieties will be either time-limited or only work while the JTAG
cable is connected if you use free edition of Quartus.

- SRAM/SDRAM controller. Unless your FPGA has 500kB of internal RAM or more
you'll have to use whatever RAM you have on your dev board.

- PIO. This component can emulate the digital pins of Arduino, so it's
required for pretty much anything. Be sure to configure it as Bidir,
with bits which can be set/reset individually. Obviously, you will have
as many pins as you have configured, with a max of 32.

- UART, SPI and I2C controllers. These will be used to interact with
hadware modules you want to connect to your dev board. It is recommended
to make UART baud rate software-configurable. It should be possible to
include several UARTs and easily integrate them in Arduino framework as
`Serial1`, `Serial2`, etc.

Note that I2C requires pull-up resistors on SDA and SCL pins to function.
You may get away by enabling internal pull-ups in the FPGA, but it's
recommended to solder actual 5-10kOhm resistors.

- EPCS controller. You only need it if you plan to store your software in EPCS.
[Here](https://www.altera.com/support/support-resources/knowledge-base/solutions/rd04112006_450.html)'s
an article describing how to program the EPCS device with FPGA SOF file and
software ELF file simultaneously. You don't need to manually type type these
commands, NIOS Flash programmer will do that for you.

If you don't want to use EPCS controller, you can remove it from the system.
Don't forget to update the CPU reset vectors if you do.

- JTAG UART, 16x2 LCD or any other compoment which can be selected as
STDIN/STDOUT.

- Timer. This will enable time functions, such as `millis()` and `micros()`.

Software
--------

The goal of NIOSDuino is to allow Arduino sketches to run with minimum
modifications or none at all. Usually, all you need to do is to rename
the sketch file from *.ino to *.cpp and compile. Some sketches rely on
the Arduino IDE preprocessing to access the necessary include files.
Eclipse will not do that for you, so if you see errors when compiling your
sketch, try adding the following to the beginning of the file:

    #include <Arduino.h>

Also make sure you declare all your functions which are called from `setup()`
and `loop()` before they are used.

Additional libraries you'd like to use should be copied to your project folder.
Then you'll have to update the Makefile to include new header files locatons
to `ALT_INCLUDE_DIRS` and new rules to build the *.cpp files (which is done
by right-clicking on your Eclipse project and selecting "Refresh").

Unlike actual AVR chips, QSys components don't share pins with each other.
This means you can use all the PIO pins and SPI/UART modules in parallel.
There's also no need to e.g. configure the direction for SPI/UART pins.
Note that existing libraries may still assume that shared pins are used, and
configure them accordingly.

Digital pins are accessible by index, starting from 0. That is,
`digitalWrite(0, LOW)` will set PIO pin 0 to LOW. Macros like
`digitalPinToPort()` and `portOutputRegister()` are also provided,
but remember to update port data types from `uint8_t` to `uint32_t`.

Hardware UARTs are accessible as `Serial0`, `Serial1`, etc. Only the baudrate
setting is taken into account in `SerialN.begin()`, bit settings have to be
configured in QSys and cannot be changed at runtime.

`Serial` is whatever component you chose as STDIN/STDOUT,
which can be a UART, a JTAG UART (default) or even an LCD. Obviously, any
baudrate or bit settings given in `Serial.begin()` are ignored for non-UART
hardware. If a hardware UART is to be used in a sketch which refers to `Serial`,
and you don't want to reconfigure STDIN/STDOUT, you can

    #define Serial Serial0

Note that the same UART should not be used as STDIN/STDOUT and as `SerialN`
device at the same time.

SPI controller manages 3 pins - MISO, MOSI and CLK. Chipselect signal (SS or CS)
is not used and should not be routed to an FPGA pin. Arduino libraries typically
expect the user to assign a regular diginal IO pin to be used as chipselect.
As a consequence, transaction management mechanism of SPI controller is not used.

I2C controller currently implements an ugly hack to get around a limitation
of the HAL driver: when an empty (address-only) transfer is submitted to
`endTransmission()`, an extra byte is transmitted before the stop sequence.
This is notably required for I2C scanners to work, since they typically
use zero-length transfers to detect I2C slaves.
