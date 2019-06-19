
About NIOSDuino
---------------

NIOSDuino is an adaptation of Arduino core libraries which run on a system
built with Qsys tool (formely SoPC Builder) from Quartus Prime. This
enables Arduino code to be reused on Intel (formely Altera)
FPGA development boards and the Arduino hardware modules (or shields)
to be used with FPGA boards.

Getting started
---------------

Here's a quickstart guide which should get you started if you have an FPGA
board with an optional SDRAM chip. If you're planning to purchase a dev board,
consider reading the Hardware section below first.

1. Install Quartus Prime with support package for your FPGA. I used version
17.0 with a MAX1000 dev board based on MAX 10.

2. Create/open a project with pin assignements corresponding to the dev board
that you have.

3. Run Qsys tool and open nios_duino.qsys. Generate the HDL (and the
symbol, if you plan to use graphical schematic editor).

4. Instantiate the generated system in your project and connect its outputs
to FPGA pins. If your board has LEDs, connect one to PIO[13].

5. Build the project and program the FPGA.

6. Open the NIOS II Eclipse IDE. Create a BSP (board support package) and
NIOS II application project from template. If you have less than 100KB of RAM
(typically, when using on-chip memory), pick "Hello world small" as a base,
otherwise pick regular "Hello world".  Set the STDIN/STDOUT to `jtag_uart_0`,
and system timer to `timer_0`. Additionally, if you have picked
"Hello world small", go to BSP advanced settings and enable C++ support.


7. In the NIOS II Application project, remove `hello_world.c` file, and
add files from `nios_duino` folder, keeping the folder structure. Then add
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
a NIOS II CPU, enough on-board memory to run the code, and enough flash
storage if you want your code to be persistent.

- FPGA - the smallest NIOS II CPU implementation requires just under 5K LEs,
so the FPGA you choose must be at least that big. If you plan to build custom
peripherals, or you want to be able to comfortably debug your system with
SignalTap, you should get a bigger chip.
MAX 10 devices are particularly interesting if you need the ADC functionality
or persistent flash storage, since these are built it. Note that this only
applies to "Analog" (featuring ADC and Flash) and "Flash" (Flash only) variants.
The "Compact" MAX 10 variant will have no ADC and very little internal flash.

Also check that the IO voltage supported by the FPGA is compatible with the
external hadware modules you have. Typically, 3.3V modules work fine with
FPGAs which support 3.3V IO.

Older Cyclone II family is no logner supported by new versions
of Quartus IDE, and older Quartus versions don't have the toolchain
extensions (like C++1x) that recent Arduino code requires. While it's still
possible to run some older Arduino libraries using the old toolchain, it
was decided it's not worth the effort, so Cyclone II is not supported.

- RAM - NIOSDuino requires much more RAM than Arduino. Unless you have flash
which supports code execution, all program code will have to reside in RAM,
plus there's a lot of overhead in using HAL functions to mimic the AVR
library API. When using small C library and optimization (-O3 or -Os),
a typical Arduino program should fit into ~64KB. With standard C library
and light optimization (-01) you will need 128KB or more.

If you execute the code from flash, much less RAM is needed, but you will
still need more than a real Arduino has, because of the API overhead. Pretty
much any FPGA capable of implementing a NIOS II CPU will have at least
32 KB of on-chip RAM, which is enough for typical Arduino programs.

Debugging your software is much easier when the code resides in RAM, so
consider getting a dev board with enough on-chip RAM or on-board SDRAM
even if you want the execute the code from flash in the end.

Note that Intel FPGAs provide on-chip RAM as M9K blocks, so you
must divide your FPGA RAM capacity in kbits by 9 (and not by 8) in order
to get the usable capacity in KB.

- Flash - SRAM-based FPGA dev boards have EPCS/EPCQ serial flash
chips for FPGA configuration, which can also be used as program storage.
Typically, you don't want to execute code "in-place" from flash,
which is slow and cannot be programmed by the debugger. Rather, you will
want to use a "boot copier" which copies flash contents into RAM on a reset,
and then runs the code from RAM.

MAX 10 FPGAs (except for "Compact" variant) have internal flash storage
(CFM/UFM) which can be used as program flash.

Cheap Cyclone dev boards come with EPCS4 chip which only provides 512kB of
storage shared between the FPGA configuration data and the software, so it will
probably be not big enough. Try to get at least EPCS16 if you plan to store
your code on the board. EPCS16 is also the smallest external serial flash with
in-place code execution support.

SoPC
----

This project includes a sample SoPC system which should suit most boards.
Before it can be used, you should remove components for peripherals not
supported by your dev board and those you don't plan to use, and configure
the remaining components to suit your hardware and needs as described below:

- NIOS CPU. Only the "tiny" variety is available without restrictions,
bigger varieties will be either time-limited or only work while the JTAG
cable is connected if you use free edition of Quartus.

- SDRAM controller. This is an interface to common single
data rate SDRAM chips. Make sure to go though its options and set up the
RAS/CAS latency and address / data width corresponding to your SDRAM chip.
On many boards, running SDRAM at higher frequencies requires a separate
clock with a phase shift compared to system clock: refer to a sample project
for your dev board to find out what the correct PLL settings are.
Remove the component if your board doesn't have SDRAM or you don't use it.
If you have SRAM, rather than SDRAM, you need to use a generic tri-state
controller instead (again, the sample project for your board is a good start).

- On-chip RAM. This component could be used to store data and / or code 
of NIOS II CPU, and on systems with no flash memory it can be used to emulate
the EEPROM. Its size should be adjusted depending on the usage. If, at the
end of the synthesis your FPGA still has unused M9K blocks, you will
likely want to increase the size of your on-chip RAM.

- PIO. This component can emulate the digital pins of Arduino, so it's
required for pretty much anything. Make sure it's configured as Bidir,
with bits which can be set/reset individually. You will have as many
digital IO pins as you have configured, with a max of 32.

- PWM. This component provides you with output pins supporting PWM
functionality, such as `analogWrite()` and `tone()`. Unlike other
components, this one is not standard and must be
[obtained](https://github.com/dimag0g/avalon_pwm) separately.
Refer to its own README file for instructions.

- ADC. Provides an interface to built-in ADC block in MAX 10 devices.
The analog pin locations are fixed, however, you can still configure a
mapping between analog pins and software ADC channels in the Sequencer tab.

- JTAG UART, 16x2 LCD or any other compoment which can be selected as
STDIN/STDOUT.

- UART, SPI and I2C controllers. These will be used to interact with
hadware modules you want to connect to your dev board. It is recommended
to make UART baud rate software-configurable. It should be possible to
include several UARTs and easily integrate them in Arduino framework as
`Serial1`, `Serial2`, etc.

Note that I2C requires pull-up resistors on SDA and SCL pins to function.
You may get away with enabling internal pull-ups in the FPGA, but it's
recommended to solder actual 5-10kOhm resistors.

- Timer. This will enable time-related functions, such as `millis()`
and `delay()`. It is expected to be configured with a period of 1 ms.

- EPCS/EPCQ controller (not included). If your board uses an EPCS/EPCQ device
for FPGA configuration, you may use it to store the FPGA configuration
and your software simultaneously. In such case, you should add a corresponding
component to your Qsys system and point the CPU reset vector to it.

- UMF controller. This component provides access to user-mode flash memory
in MAX 10 FPGAs, which can be used to store your software and also to emulate
EEPROM (the latter is not implemented yet).

Software
--------

The goal of NIOSDuino is to allow Arduino sketches to run with minimum
modifications or none at all. Usually, all you need to do is to rename
the sketch file from `*.ino` to `*.cpp` and compile. Some sketches rely on
the Arduino IDE preprocessing to access the necessary include files.
Eclipse will not do that for you, so if you see errors when compiling your
sketch, try adding the following to the beginning of the file:

    #include <Arduino.h>

Also make sure you declare all your functions which are called from `setup()`
and `loop()` before they are used.

Additional libraries you'd like to use should be copied to your project folder.
Then you'll have to update the Makefile to include new header files locatons
to `ALT_INCLUDE_DIRS` and new rules to build the `*.cpp` files (which is done
by right-clicking on your Eclipse project and selecting "Refresh").

Unlike actual AVR chips, Qsys components don't share pins with each other.
This means you can use all the PIO/PWM/SPI/UART pins in parallel.
There's also no need to e.g. configure the direction for SPI/UART/PwM pins.
Note that existing libraries may still assume that shared pins are used, and
configure random PIO pins in a way that would be required on AVR. This is
usually not a problem, but it may have undesired side-effects in some cases.

Digital pins are accessible by index, starting from 0. That is,
`digitalWrite(0, LOW)` will set PIO pin 0 to LOW. Macros like
`digitalPinToPort()` and `portOutputRegister()` are also provided,
but remember to update port data types from `uint8_t` to `uint32_t`.

PWM pins are separate from PIO pins (so `digitalWrite(pin, LOW)` doesn't stop
the output started by `analogWrite(pin, duty_cycle)`). You will not be able
to reconfigure PWM pins as inputs. If you need to output constant LOW/HIGH
levels, use `analogWrite(pin, 0)` and `analogWrite(pin, 255)` respectively.

ADC pins are continuously polled by the sequencer, and are accessible
via `analogRead(pin)`, where `pin` corresponds to the index of the respective
channel in the sequencer, starting from 0. The refresh rate will depend on
the ADC module configuration and the number of channels. A single channel can
be continuously sampled at 1MSps.

Hardware UARTs are accessible as `Serial0`, `Serial1`, etc. Only the baudrate
setting is taken into account in `SerialN.begin()`, bit settings have to be
configured in Qsys and cannot be changed at runtime.

`Serial` is whatever component you chose as STDIN/STDOUT,
which can be a UART, a JTAG UART (default) or even a 16x2 LCD. Obviously, any
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

EEPROM emulation requires either internal flash memory, or a RAM block which
is not cleared on a CPU reset. This memory must hold a section with
[`(NOLOAD)`](https://electronics.stackexchange.com/a/442757/72179)
directive to avoid being zeroed by the startup code.

Disclaimer
----------

This is a personal project which is not endorsed by or affiliated with either
Arduino or Intel, who have no obligations to provide you with technical support
or warranty in regards to this library, even if you are entitled to a warrany
or support in regards to their own products.

If you have bug reports or feature requests, feel free to open a new issue.
General support questions can be asked here too, however, if you're familiar with
https://electronics.stackexchange.com/ or https://arduino.stackexchange.com/,
I advise you to post such questions there, *provided that they are on-topic
on these sites*, as doing so will promote the project and you're likely to get
an answer much quicker from a larger community. Make sure to write your
question in a way that it can be understood without any prior knowledge of
this project, and that makes it clear what your specific problem is and what
you have tried to solve it. You can always come back here if your question
doesn't get an answer on Stack Exchange, but avoid double-posting from the start.
