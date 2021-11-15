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
	char page1[8] = "01234567";
	char page2[8] = "89101112";
	char page3[8] = "13141516";

	char eepromOutput[50] = "";

	i2c_init();
	uart_init();
	sei();

	// for (int i = 0; i < sizeof(name); i++)
	// {
	// 	eeprom_write_byte(EEPROM_ADDR_WRITE+i, name[i]);
	// }
	uint8_t lenOfAllInputs = strlen(page1) + strlen(page2) + strlen(page3);
	
	printf_P(PSTR("start: \n"));
	eeprom_write_page(EEPROM_ADDR_WRITE, page1);
	printf_P(PSTR("page1: \n"));
	eeprom_write_page((EEPROM_ADDR_WRITE + strlen(page1)), page2);
	printf_P(PSTR("page2: \n"));
	eeprom_write_page((EEPROM_ADDR_WRITE + strlen(page1) + strlen(page2)), page3);
	printf_P(PSTR("page3: \n"));

	eeprom_sequential_read(eepromOutput, EEPROM_ADDR_WRITE, lenOfAllInputs);
	printf_P(PSTR("String: %s\n"), eepromOutput);
	printf_P(PSTR("String: \n"));

	while (1)
	{
		// for (int i = 0; i < sizeof(name); i++)
		// {
		// 	printf_P(PSTR("%c"), eeprom_read_byte(EEPROM_ADDR_WRITE + i));
		// }
		// printf_P(PSTR("\n"));
		// _delay_ms(1000);
	}
}
