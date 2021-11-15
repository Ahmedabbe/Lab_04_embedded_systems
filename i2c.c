#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include <stdio.h>
#include <string.h>

#include "i2c.h"

void i2c_init(void)
{
	/*
		Set TWEN bit in TWCR reg = enable TWI(Two Wire Interface)
		Set TWSR reg byte to 0x00 = prescaler of 1
		Set TWBR reg byte to 0x48 = sets bit rate to 100 kHz
		Calculation for TWBR:
			SCLfreq = F_CPU / (16 + (2 * TWBR * Prescaler))
			100 000 = 16 000 000 / (16 + (2 * TWBR * 1)
			TWBR = 72 = 0x48
	*/
	TWCR = (1 << TWEN);
	TWSR = 0x00;

	TWBR = 0x48;
}

void i2c_meaningful_status(uint8_t status)
{
	switch (status)
	{
	case 0x08: // START transmitted, proceed to load SLA+W/R
		printf_P(PSTR("START\n"));
		break;
	case 0x10: // repeated START transmitted, proceed to load SLA+W/R
		printf_P(PSTR("RESTART\n"));
		break;
	case 0x38: // NAK or DATA ARBITRATION LOST
		printf_P(PSTR("NOARB/NAK\n"));
		break;
	// MASTER TRANSMIT
	case 0x18: // SLA+W transmitted, ACK received
		printf_P(PSTR("MT SLA+W, ACK\n"));
		break;
	case 0x20: // SLA+W transmitted, NAK received
		printf_P(PSTR("MT SLA+W, NAK\n"));
		break;
	case 0x28: // DATA transmitted, ACK received
		printf_P(PSTR("MT DATA+W, ACK\n"));
		break;
	case 0x30: // DATA transmitted, NAK received
		printf_P(PSTR("MT DATA+W, NAK\n"));
		break;
	// MASTER RECEIVE
	case 0x40: // SLA+R transmitted, ACK received
		printf_P(PSTR("MR SLA+R, ACK\n"));
		break;
	case 0x48: // SLA+R transmitted, NAK received
		printf_P(PSTR("MR SLA+R, NAK\n"));
		break;
	case 0x50: // DATA received, ACK sent
		printf_P(PSTR("MR DATA+R, ACK\n"));
		break;
	case 0x58: // DATA received, NAK sent
		printf_P(PSTR("MR DATA+R, NAK\n"));
		break;
	default:
		printf_P(PSTR("N/A %02X\n"), status);
		break;
	}
}

inline void i2c_start()
{
	/*
		Set TWEN bit in TWCR reg = activate SCL/SDA pins and enable TWI operations
		TWSTA bit set in TWCR register to generate start condition, will be cleared in i2c_xmit_addr
		TWINT bit set in TWCR register clears the flag and starts the operation of TWI
		Await until TWI has finished its current job AKA TWINT bit is set
	*/
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)))
		;
}

inline void i2c_stop()
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

	while ((TWCR & (1 << TWSTO)))
		;
}

inline uint8_t i2c_get_status(void)
{
	uint8_t status;
	status = TWSR & 0xF8;

	return status;
}

inline void i2c_xmit_addr(uint8_t address, uint8_t rw)
{
	TWDR = (address & 0xfe) | (rw & 0x01);

	TWCR = (1 << TWINT) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)))
		;
}

inline void i2c_xmit_byte(uint8_t data)
{
	TWDR = data;

	TWCR = (1 << TWINT) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)))
		;
}

inline uint8_t i2c_read_ACK()
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

	while (!(TWCR & (1 << TWINT)))
		;

	return TWDR;
}

inline uint8_t i2c_read_NAK()
{
	TWCR = (1 << TWINT) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)))
		;

	return TWDR;
}

inline void eeprom_wait_until_write_complete()
{
	while (i2c_get_status() != 0x18)
	{
		i2c_start();
		i2c_xmit_addr(EEPROM_ADDR, I2C_W);
	}
}

uint8_t eeprom_read_byte(uint8_t addr)
{
	uint8_t readByte;

	i2c_start();
	i2c_xmit_addr(EEPROM_ADDR, I2C_W);
	i2c_xmit_byte(addr);

	i2c_start();
	i2c_xmit_addr(EEPROM_ADDR, I2C_R);
	readByte = i2c_read_NAK();

	i2c_stop();

	return readByte;
}

void eeprom_write_byte(uint8_t addr, uint8_t data)
{
	i2c_start();
	i2c_xmit_addr(EEPROM_ADDR, I2C_W);

	i2c_xmit_byte(addr);
	i2c_xmit_byte(data);

	i2c_stop();
	eeprom_wait_until_write_complete();
}

void eeprom_write_page(uint8_t addr, char *data)
{
	while(addr % 8 != 0){ 
		addr++;
	}
	
	i2c_start();
	i2c_xmit_addr(EEPROM_ADDR, I2C_W);
	i2c_xmit_byte(addr);

	for (int i = 0; i < strlen(data); i++)
	{
		i2c_xmit_byte(data[i]);
	}

	i2c_stop();
	eeprom_wait_until_write_complete;
}

void eeprom_sequential_read(char *buf, uint8_t start_addr, uint8_t len)
{
	while(start_addr % 8 != 0){
		start_addr++;
	}

	i2c_start();
	i2c_xmit_addr(EEPROM_ADDR, I2C_W);
	printf_P(PSTR("xmit addr \n"));
	i2c_xmit_byte(start_addr);
	printf_P(PSTR("xmit byte \n"));

	i2c_start();
	i2c_xmit_addr(EEPROM_ADDR, I2C_R);
	printf_P(PSTR("xmit word addr \n"));

	for (int i = 0; i < (len - 1); i++)
	{
		buf[i] = i2c_read_ACK();
	}
	printf_P(PSTR("ACK \n"));
	buf[len - 1] = i2c_read_NAK();
	printf_P(PSTR("NAK \n"));
	i2c_stop();
	printf_P(PSTR("stop \n"));
}
