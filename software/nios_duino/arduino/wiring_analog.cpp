/*
  wiring_analog.c - analog input and output
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  Modified 28 September 2010 by Mark Sproul
*/

//! NIOSDuino custom file, (c) Dmitry Grigoryev, 2019

#include "Arduino.h"
#include "pins_arduino.h"

#ifdef __ALTERA_MODULAR_ADC
#include <altera_modular_adc.h>
#include <io.h>
int analogRead(uint8_t pin)
{
	//adc_stop(MODULAR_ADC_0_SEQUENCER_CSR_BASE);
	//adc_set_mode_run_once(MODULAR_ADC_0_SEQUENCER_CSR_BASE);
	//adc_start(MODULAR_ADC_0_SEQUENCER_CSR_BASE);
    //while(IORD_ALTERA_MODULAR_ADC_SEQUENCER_CMD_REG(MODULAR_ADC_0_SEQUENCER_CSR_BASE)
    //         & ALTERA_MODULAR_ADC_SEQUENCER_CMD_RUN_MSK);
	return IORD_32DIRECT((MODULAR_ADC_0_SAMPLE_STORE_CSR_BASE + (pin << 2)),0);
}
#else
int analogRead(uint8_t pin){return 0;}
#endif

uint8_t analog_reference = DEFAULT;

void analogReference(uint8_t mode)
{
	// can't actually set the register here because the default setting
	// will connect AVCC and the AREF pin, which would cause a short if
	// there's something connected to AREF.
	analog_reference = mode;
}



// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite(uint8_t pin, int val)
{
	// We need to make sure the PWM output is enabled for those pins
	// that support it, as we turn it off when digitally reading or
	// writing with them.  Also, make sure the pin is in output mode
	// for consistenty with Wiring, which doesn't require a pinMode
	// call for the analog output pins.
	pinMode(pin, OUTPUT);

	if (val < 128) {
		digitalWrite(pin, LOW);
	} else {
		digitalWrite(pin, HIGH);
	}
}

