/*
 * lcd_i2c_keypad.c
 *
 * Created: 10/22/2024 5:38:08 PM
 * Author : Jorge Adx
 */ 

#define F_CPU 16000000UL		// MICROCONTROLLER FREQUENCY
#include <avr/io.h>				// AVR LIBRARY
#include <util/delay.h>			// DELAY LIBRARY
#include <avr/interrupt.h>		// INTERRUPT LIBRARY	
#include <compat/twi.h>			// I2C LIBRARY
#include <stdio.h>				// LIBRARY OF I/O OPS (JUST IN CASE)	

#define I2C_ADDR 0x27			// MODULE I2C ADDRESS (CHECK IF NECESARY, IF NOT WORK ANOTHER POSSIBLE ADDRESS IS 0x3F)
#define LCD_BACKLIGHT 0x08		// TURN ON THE BACKLIGHT
#define ENABLE 0x04				// ENABLE BIT
#define RW 0x02					// READ/WRITE BIT
#define RS 0x01					// REGISTER SELECT BIT

// LIST OF VARIABLES SHOWN AT START FOR CORRECT OPERATION
void I2C_Init(void);			
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write(uint8_t data);
void LCD_Init(void);
void LCD_Command(uint8_t cmd);
void LCD_Char(unsigned char data);
void LCD_Send(uint8_t value, uint8_t mode);
void LCD_EnablePulse(uint8_t data);
void LCD_XY(int x, int y);
void LCD_String (int x, int y, const char*str);


int main(void) {
	I2C_Init();					// INITIALIZE I2C
	LCD_Init();					// INITIALIZE LCD VIA I2C
	
	//  DISPLAY TEXT ON LCD			
	LCD_XY(0, 5);				// PLEASE, SELECT MANUALLY THE LINE TO START -> x= 0 TO 3, y= 0 TO 19;
	LCD_String(0, 5, "HI, WORLD!");
	LCD_XY(2, 7);
	LCD_String(2, 7, "I AM");
	LCD_XY(3, 4);
	LCD_String(3, 4, "JORGE ADX :)");
	
	while (1) {
		// Infinite loop
	}
}

void I2C_Init(void) {	 		// INITIALIZE I2C MODULE AT 100KHZ
	TWSR = 0x00;				// SET PRESCALER TO 1
	TWBR = 0x48;				// SET BITRATE TO 100KHZ
	TWCR = (1<<TWEN);			// ENABLE I2C MODULE
}

void I2C_Start(void) {			// START CONDITION
	TWCR = (1<<TWSTA) | (1<<TWEN) | (1<<TWINT);  
	while (!(TWCR & (1<<TWINT)));
}

void I2C_Stop(void) {			// STOP CONDITION
	TWCR = (1<<TWSTO) | (1<<TWEN) | (1<<TWINT);
}

void I2C_Write(uint8_t data) {	// DATA WRITE
	TWDR = data;				// WRITE DATA
	TWCR = (1<<TWEN)|(1<<TWINT);// SEND DATA
	while (!(TWCR&(1<<TWINT)));	// WAIT FOR COMPLETION
}

void LCD_Init(void) {			// INITIALIZE LCD
	_delay_ms(50);				// DELAY TO GET READY
	
	LCD_Command(0x03);			// INITIALIZE 3 TIMES TO WORK (ACCORDING TO DATASHEET)
	_delay_ms(5);
	LCD_Command(0x03);
	_delay_ms(5);
	LCD_Command(0x03);
	_delay_ms(5);
	LCD_Command(0x02);			// SET IN A 4-BIT MODE
	
	// LCD SETTINGS
	LCD_Command(0x28);			// 4 BITS, 4 LINES, 5x8 CHAR
	LCD_Command(0x0C);			// DISPLAY: ON, CURSOR: OFF
	LCD_Command(0x06);			// INPUT MODE, INCREASE CURSOR DIRECTION
	LCD_Command(0x01);			// CLEAR LCD
	_delay_ms(5);
}

void LCD_Command(uint8_t cmd) {	// SEND COMMAND TO LCD
	LCD_Send(cmd, 0);
}

void LCD_Char(unsigned char data) {
	LCD_Send(data, RS);			// RS = 1 TO SEND DATA
}

void LCD_Send(uint8_t value, uint8_t mode) {
	uint8_t highNibble = value & 0xF0;				// HIGH NIBBLE
	uint8_t lowNibble = (value << 4) & 0xF0;		// LOW NIBBLE
	
	// SEND HIGH NIBBLE
	LCD_EnablePulse(highNibble | mode | LCD_BACKLIGHT);
	
	// SEND LOW NIBBLE
	LCD_EnablePulse(lowNibble | mode | LCD_BACKLIGHT);
}

void LCD_EnablePulse(uint8_t data) {
	I2C_Start();
	I2C_Write(I2C_ADDR << 1);
	I2C_Write(data | ENABLE);	// ENABLE E
	_delay_us(1);
	I2C_Write(data & ~ENABLE);	// DISABLE E
	_delay_us(50);
	I2C_Stop();
}

void LCD_XY (int x, int y){		// LCD LINE CODE 20X4 (SWITCH CASE CAN BE USED INSTEAD)
	if (x==0) {
		LCD_Command(0x80 + y);	// 0x80 FOR FIRST LINE
	} else if (x==1) {
		LCD_Command(0xC0 + y);	// 0xC0 FOR SECOND LINE
	} else if (x==2) {
		LCD_Command(0x94 + y);	// 0x94 FOR THIRD LINE
	} else if (x==3) {
		LCD_Command(0xD4 + y);	// 0xD4 FOR FOURTH LINE
	}
}

void LCD_String (int x, int y, const char*str){		// DISPLAY A STRING OF CHAR INSTEAD JUST 1 CHAR
	while (*str) {
		LCD_Char(*str);			// SHOW EVERY CHAR OF THE STRING
		str++;					// GO TO THE NEXT CHAR
	}
}
		