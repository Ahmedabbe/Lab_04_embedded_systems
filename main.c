#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#include "adc.h"
#include "gpio.h"
#include "i2c.h"
#include "serial.h"
#include "timer.h"


void main(void)
{
	char name[] = "Ahmed Alhasani heter jag";

	i2c_init();
	uart_init();
	sei();

	// Write bytes to eeprom memory
	for (int i = 0; i < sizeof(name); i++)
	{
		eeprom_write_byte(EEPROM_ADDR_WRITE+i, name[i]);
	}

	while (1)
	{
		// Print out bytes stored on eeprom memory
		for (int i = 0; i < sizeof(name); i++)
		{
			printf_P(PSTR("%c"), eeprom_read_byte(EEPROM_ADDR_WRITE + i));
		}
		printf_P(PSTR("\n"));
		_delay_ms(1000);
	}
}
