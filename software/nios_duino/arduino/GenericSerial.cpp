/*
  GenericSerial.cpp - Generic serial library using STDIN/STDOUT

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
*/

//! NIOSDuino custom file, (c) Dmitry Grigoryev, 2018

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

#include "GenericSerial.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

void GenericSerial::begin(unsigned long baud, byte config)
{

}

void GenericSerial::end()
{
  flush();
}

int GenericSerial::available(void)
{
  return 1;
}

int GenericSerial::peek(void)
{
  return -1;
}

int GenericSerial::read(void)
{
	char c;
	::read(0, &c, 1);
	return (unsigned char)c;
}

int GenericSerial::availableForWrite(void)
{
  return 1;
}

void GenericSerial::flush()
{
#ifdef ALT_SEMIHOSTING
  alt_putbufflush();
#endif
}

size_t GenericSerial::write(uint8_t c)
{
	::write(2, (char *)&c, 1);
	return ::write(1, (char *)&c, 1);
}

GenericSerial Serial;
