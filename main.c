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
	char seqWriteString[] = "Denna meningen testar sequential write";
	char eepromOutput[100] = " ";

	uint8_t lenOfAllInputs = strlen(page1) + strlen(page2) + strlen(page3);

	i2c_init();
	uart_init();
	sei();

	/*	"Clears" memory by writing blanks	*/
	for (int i = 0; i < 100; i++)
	{
		eeprom_write_byte(EEPROM_ADDR_WRITE+i, eepromOutput[i]);
	}
	
	/*	write page, read and print	*/
	eeprom_write_page(EEPROM_ADDR_WRITE, page1);
	eeprom_write_page(EEPROM_ADDR_WRITE + 8, page2);
	eeprom_write_page(EEPROM_ADDR_WRITE + 16, page3);

	eeprom_sequential_read(eepromOutput, EEPROM_ADDR_WRITE, lenOfAllInputs);
	print_data(eepromOutput);

	/*	Sequential write, read and print	*/
	eeprom_sequential_write(EEPROM_ADDR_WRITE, seqWriteString);

	eeprom_sequential_read(seqWriteString, EEPROM_ADDR_WRITE, strlen(seqWriteString));
	print_data(seqWriteString);

	while (1)
	{

	}
}

void print_data(char *data){
	printf_P(PSTR("\n\nString: %s\n\nHex: \n"), data);

	for (int i = 0; i < strlen(data); i++)
	{
		printf_P(PSTR("0x%X "), data[i]);
	}
}
