/*
  HardwareSerial.cpp - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 23 November 2006 by David A. Mellis
  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
  Modified 3 December 2013 by Matthijs Kooijman
*/

//! NIOSDuino custom file, (c) Dmitry Grigoryev, 2018

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include <altera_avalon_uart_regs.h>
#include <altera_avalon_uart.h>

#include "HardwareSerial.h"



#if defined(UART_0_BASE) || defined(UART_1_BASE)


void HardwareSerial::begin(unsigned long baud, byte config)
{
  alt_u32 divisor = (UART_0_FREQ/baud)-1;
  IOWR_ALTERA_AVALON_UART_DIVISOR(UART_0_BASE, divisor);
   fp = fopen(UART_0_NAME, "r+");
}

void HardwareSerial::end()
{
  fclose(fp);
}

int HardwareSerial::available(void)
{
  return 1;
}

int HardwareSerial::peek(void)
{
  return -1;
}

int HardwareSerial::read(void)
{
  return fgetc(fp);
}

int HardwareSerial::availableForWrite(void)
{
  return 1;
}

void HardwareSerial::flush()
{
  fflush(fp);
}

size_t HardwareSerial::write(uint8_t c)
{
  return fputc(c, fp);
}

#endif // whole file
