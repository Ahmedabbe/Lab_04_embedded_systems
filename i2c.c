#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include <stdio.h>
#include <string.h>

#include "i2c.h"

void i2c_init(void)
{
	/*
		-Set TWEN bit in TWCR reg = enable TWI(Two Wire Interface)
		-Set TWSR reg byte to 0x00 = prescaler of 1
		-Set TWBR reg byte to 0x48 = sets bit rate to 100 kHz
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
		-Set TWINT bit in TWCR reg = clears flag and starts TWI operation
		-Set TWSTA bit in TWCR reg = generate start condition. Is cleared in function "i2c_xmit_addr"
		-Set TWEN bit in TWCR reg = activate SCL/SDA pins and enable TWI operations
			wait for TWINT bit to be set
	*/
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)))
		;
}

inline void i2c_stop()
{
	/*
		-Set TWINT bit in TWCR reg = clears flag and starts TWI operation
		-Set TWSTO bit in TWCR reg = generate stop condition. Is automatically cleared after executing
		-Set TWEN bit in TWCR reg = activate SCL/SDA pins and enable TWI operations
			wait for TWSTO bit to clear
	*/

	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

	while ((TWCR & (1 << TWSTO)))
		;
}

inline uint8_t i2c_get_status(void)
{
	/*

	*/
	uint8_t status;
	status = TWSR & 0xF8;

	return status;
}

inline void i2c_xmit_addr(uint8_t address, uint8_t rw)
{
	/*
		In Transmit mode, TWDR contains the next byte to be transmitted. 
		In Receive mode, the TWDR contains the last byte received.

		-Assign eeprom adress followed by read/write bit to TWDR register
		-Set TWINT bit in TWCR reg = clears flag and starts TWI operation
		-Set TWEN bit in TWCR reg = activate SCL/SDA pins and enable TWI operations
			wait for TWINT bit to be set
	*/

	TWDR = (eeprom_addr & 0xfe) | (i2c_rw & 0x01);

	TWCR = (1 << TWINT) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)))
		;
}

inline void i2c_xmit_byte(uint8_t data)
{
	/*
		Assign next byte to transmit to TWDR

		-Set TWINT bit in TWCR reg = clears flag and starts TWI operation
		-Set TWEN bit in TWCR reg = activate SCL/SDA pins and enable TWI operations
			wait for TWINT bit to be set
	*/

	TWDR = byte;

	TWCR = (1 << TWINT) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)))
		;
}

inline uint8_t i2c_read_ACK()
{
	/*
		-Set TWINT bit in TWCR reg = clears flag and starts TWI operation
		-Set TWEN bit in TWCR reg = activate SCL/SDA pins and enable TWI operations
		-Set TWEA bit in TWCR reg = generates ACK pulse on the TWI bus
			wait for TWINT bit to be set
		
		return received data byte
	*/

	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

	while (!(TWCR & (1 << TWINT)))
		;

	return TWDR;
}

inline uint8_t i2c_read_NAK()
{
	/*
		-Set TWINT bit in TWCR reg = clears flag and starts TWI operation
		-Set TWEN bit in TWCR reg = activate SCL/SDA pins and enable TWI operations
			wait for TWINT bit to be set and no ACK (NAK) pulse confirmed

		return received data byte
	*/

	TWCR = (1 << TWINT) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT)))
		;

	return TWDR;
}

inline void eeprom_wait_until_write_complete()
{
	// ...
}

uint8_t eeprom_read_byte(uint8_t addr)
{
	// ...
}

void eeprom_write_byte(uint8_t addr, uint8_t data)
{
	// ...
}

void eeprom_write_page(uint8_t addr, uint8_t *data)
{
	// ... (VG)
}

void eeprom_sequential_read(uint8_t *buf, uint8_t start_addr, uint8_t len)
{
	// ... (VG)
}
