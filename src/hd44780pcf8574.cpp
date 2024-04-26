/** 
 * ---------------------------------------------------------------+ 
 * @desc        HD44780 with PCF8574 8-bit I/O expander for TWI 
 * ---------------------------------------------------------------+ 
 *              Copyright(C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko(mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       18.11.2020
 * @file        hd44780pcf8574.c
 * @tested      AVR Atmega328p
 *
 * @depend      twi, pcf8574
 * ---------------------------------------------------------------+
 */

// include libraries
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include "twi.h"
#include "hd44780pcf8574.h"

// +---------------------------+
// |         Power on          |
// | Wait for more than 15 ms  |   // 15 ms wait
// | after VCC rises to 4.5 V  |
// +---------------------------+
//              |
// +---------------------------+ 
// |  RS R/W DB7 DB6 DB5 DB4   |   
// |   0   0   0   0   1   1   |   // Initial sequence 0x30
// | Wait for more than 4.1 ms |   // 4.1 ms us writing DATA into DDRAM or CGRAM
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   
// |   0   0   0   0   1   1   |   // Initial sequence 0x30
// | Wait for more than 0.1 ms |   // 100 us writing DATA into DDRAM or CGRAM
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   // Initial sequence 0x30
// |   0   0   0   0   1   1   |   // 37 us writing DATA into DDRAM or CGRAM 4 us tadd - time after busy flag disapeared
// | Wait for more than 45 us  |   // 37 us + 4 us = 41 us *(270/250) = 45us
// +---------------------------+  
//              |
// +---------------------------+   // 4bit mode 0x20 !!! MUST BE SET TIME, BF CHECK DOESN'T WORK CORRECTLY !!!
// |  RS R/W DB7 DB6 DB5 DB4   |   // 
// |   0   0   0   0   1   0   |   // 37 us writing DATA into DDRAM or CGRAM 4 us tadd - time after busy flag disapeared
// | Wait for more than 45 us  |   // !!! MUST BE SET DELAY TIME, BUSY FLAG CHECK DOESN'T WORK CORRECTLY !!!
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   // Display off 0x28
// |   0   0   0   0   1   0   |   // 
// |   0   0   1   0   0   0   |   // 
// |    Wait for BF Cleared    |   // Wait for 50us
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   // Display clear 0x01
// |   0   0   0   0   0   0   |   //
// |   0   0   0   0   0   1   |   //
// |    Wait for BF Cleared    |   // Wait for 50us
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   // Entry mode set 0x06
// |   0   0   0   0   0   0   |   // 
// |   0   0   0   1   1   0   |   // shift cursor to the left, without text shifting
// |    Wait for BF Cleared    |   // Wait for 50us
// +---------------------------+

/**
 * @desc    LCD init - initialisation routine
 *
 * @param   uint8_t
 *
 * @return  uint8_t
 */
uint8_t HD44780_PCF8574_Init(uint8_t addr)
{
  // delay > 15ms
  _delay_ms(16);

  // Init TWI
  TWI_Init();

  // TWI: start
  // -------------------------
  TWI_MT_Start();

  // TWI: send SLAW
  // -------------------------
  TWI_Transmit_SLAW(addr);

  // DB7 BD6 DB5 DB4 P3 E RW RS 
  // DB4=1, DB5=1 / BF cannot be checked in these instructions
  // ---------------------------------------------------------------------
  HD44780_PCF8574_Send_4bits_M4b_I(PCF8574_PIN_DB4 | PCF8574_PIN_DB5);
  // delay > 4.1ms
  _delay_ms(5);

  // DB4=1, DB5=1 / BF cannot be checked in these instructions
  // ---------------------------------------------------------------------
  HD44780_PCF8574_Send_4bits_M4b_I(PCF8574_PIN_DB4 | PCF8574_PIN_DB5);
  // delay > 100us
  _delay_us(110);

  // DB4=1, DB5=1 / BF cannot be checked in these instructions
  // ---------------------------------------------------------------------
  HD44780_PCF8574_Send_4bits_M4b_I(PCF8574_PIN_DB4 | PCF8574_PIN_DB5);
  // delay > 45us(=37+4 * 270/250)
  _delay_us(50);

  // DB5=1 / 4 bit mode 0x20 / BF cannot be checked in these instructions
  // ----------------------------------------------------------------------
  HD44780_PCF8574_Send_4bits_M4b_I(PCF8574_PIN_DB5);
  // delay > 45us(=37+4 * 270/250)
  _delay_us(50);

  // TWI Stop
  TWI_Stop();

  // 4 bit mode, 2 rows, font 5x8
  HD44780_PCF8574_SendInstruction(addr, HD44780_4BIT_MODE | HD44780_2_ROWS | HD44780_FONT_5x8);

  // display off 0x08 - send 8 bits in 4 bit mode
  HD44780_PCF8574_SendInstruction(addr, HD44780_DISP_OFF);

  // display clear 0x01 - send 8 bits in 4 bit mode
  HD44780_PCF8574_SendInstruction(addr, HD44780_DISP_CLEAR);

  // entry mode set 0x06 - send 8 bits in 4 bit mode
  HD44780_PCF8574_SendInstruction(addr, HD44780_ENTRY_MODE);

  // return success
  return PCF8574_SUCCESS;
}

/**
 * @desc    LCD E pulse
 *
 * @param   uint8_t
 *
 * @return  void
 */
void HD44780_PCF8574_E_pulse(uint8_t data)
{
  // E pulse
  // ----------------------------------
  TWI_Transmit_Byte(data | PCF8574_PIN_E);
  // PWeh delay time > 450ns
  _delay_us(0.5);
  // E down
  TWI_Transmit_Byte(data & ~PCF8574_PIN_E);
  // PWeh delay time > 450ns
  _delay_us(0.5);
}

/**
 * @desc    LCD send 4bits in 4 bit mode
 *
 * @param   uint8_t
 *
 * @return  void
 */
void HD44780_PCF8574_Send_4bits_M4b_I(uint8_t data)
{
  // Send upper nibble, E up
  // ----------------------------------
  TWI_Transmit_Byte(data);
  // E pulse
  HD44780_PCF8574_E_pulse(data);
}

/**
 * @desc    LCD send 8bits in 4 bit mode
 *
 * @param   uint8_t
 * @param   uint8_t
 *
 * @return  void
 */
void HD44780_PCF8574_Send_8bits_M4b_I(uint8_t addr, uint8_t data, uint8_t annex)
{
  // upper nible with backlight
  uint8_t up_nibble = (data & 0xF0) | annex;
  // lower nibble with backlight
  uint8_t low_nibble = (data << 4) | annex;

  // TWI: start
  // -------------------------
  TWI_MT_Start();

  // TWI: send SLAW
  // -------------------------
  TWI_Transmit_SLAW(addr);

  // Send upper nibble, E up
  // ----------------------------------
  TWI_Transmit_Byte(up_nibble);
  // E pulse
  HD44780_PCF8574_E_pulse(up_nibble);

  // Send lower nibble, E up
  // ----------------------------------
  TWI_Transmit_Byte(low_nibble);
  // E pulse
  HD44780_PCF8574_E_pulse(low_nibble);

  // TWI Stop
  TWI_Stop();
}

/**
 * @desc    LCD check BF
 *
 * @param   uint8_t
 *
 * @return  void
 */
void HD44780_PCF8574_CheckBF(uint8_t addr)
{
}

/**
 * @desc    LCD Send instruction 8 bits in 4 bits mode
 *
 * @param   uint8_t
 * @param   uint8_t
 *
 * @return  void
 */
void HD44780_PCF8574_SendInstruction(uint8_t addr, uint8_t instruction)
{
  // send instruction
  HD44780_PCF8574_Send_8bits_M4b_I(addr, instruction, PCF8574_PIN_P3);
  // check BF
  //HD44780_PCF8574_CheckBF(addr);
  _delay_ms(50);
}

/**
 * @desc    LCD Send data 8 bits in 4 bits mode
 *
 * @param   uint8_t
 * @param   uint8_t
 *
 * @return  void
 */
void HD44780_PCF8574_SendData(uint8_t addr, uint8_t data)
{
  // send data
  // data/command -> pin RS High
  // backlight -> pin P3
  HD44780_PCF8574_Send_8bits_M4b_I(addr, data, PCF8574_PIN_RS | PCF8574_PIN_P3);
  // check BF
  //HD44780_PCF8574_CheckBF(addr);
  //_delay_ms(50);
}

/**
 * @desc    LCD Go to position x, y
 *
 * @param   uint8_t
 * @param   uint8_t
 * @param   uint8_t
 *
 * @return  uint8_t
 */
uint8_t HD44780_PCF8574_PositionXY(uint8_t addr, uint8_t x, uint8_t y)
{
  if(x > HD44780_COLS || y > HD44780_ROWS) {
    // error
    return PCF8574_ERROR;
  }
  // check which row
  if(y == 0) {
    // send instruction 1st row
    HD44780_PCF8574_SendInstruction(addr,(HD44780_POSITION |(HD44780_ROW1_START + x)));
  } else if(y == 1) {
    // send instruction 2nd row
    HD44780_PCF8574_SendInstruction(addr,(HD44780_POSITION |(HD44780_ROW2_START + x)));
  }
  // success
  return PCF8574_SUCCESS;
}

/**
 * @desc    LCD display clear
 *
 * @param   uint8_t
 *
 * @return  void
 */
void HD44780_PCF8574_DisplayClear(uint8_t addr)
{
  // Diplay clear
  HD44780_PCF8574_SendInstruction(addr, HD44780_DISP_CLEAR);
}

/**
 * @desc    LCD display on
 *
 * @param   uint8_t
 *
 * @return  void
 */
void HD44780_PCF8574_DisplayOn(uint8_t addr)
{
  // send instruction - display on
  HD44780_PCF8574_SendInstruction(addr, HD44780_DISP_ON);
}

/**
 * @desc    LCD display off (ADDED)
 *
 * @param   uint8_t
 *
 * @return  void
 */
void HD44780_PCF8574_DisplayOff(uint8_t addr)
{
  // send instruction - display off
  HD44780_PCF8574_SendInstruction(addr, HD44780_DISP_OFF);
}

/**
 * @desc    LCD cursor on, display on
 *
 * @param   uint8_t
 *
 * @return  void
 */
void HD44780_PCF8574_CursorOn(uint8_t addr)
{
  // send instruction - cursor on
  HD44780_PCF8574_SendInstruction(addr, HD44780_CURSOR_ON);
}

/**
 * @desc    LCD cursor blink, cursor on, display on
 *
 * @param   uint8_t
 *
 * @return  void
 */
void HD44780_PCF8574_CursorBlink(uint8_t addr)
{
  // send instruction - Cursor blink
  HD44780_PCF8574_SendInstruction(addr, HD44780_CURSOR_BLINK);
}

/**
 * @desc    LCD draw uint8_t
 *
 * @param   uint8_t
 * @param   uint8_t
 *
 * @return  void
 */
void HD44780_PCF8574_DrawChar(uint8_t addr, char uint8_tacter)
{
  // Draw uint8_tacter
  HD44780_PCF8574_SendData(addr, uint8_tacter);
}

/**
 * @desc    LCD draw string
 *
 * @param   uint8_t
 * @param   uint8_t *
 *
 * @return  void
 */
void HD44780_PCF8574_DrawString(uint8_t addr, char *str)
{
  unsigned short int i = 0;
  // loop through uint8_ts
  while(str[i] != '\0') {
    // draw individual uint8_ts
    HD44780_PCF8574_DrawChar(addr, str[i++]);
  }
}

/**
 * @desc    Shift cursor / display to left / right
 *
 * @param   uint8_t addr
 * @param   uint8_t item {HD44780_CURSOR; HD44780_DISPLAY}
 * @param   uint8_t direction {HD44780_RIGHT; HD44780_LEFT}
 *
 * @return  uint8_t
 */
uint8_t HD44780_PCF8574_Shift(uint8_t addr, uint8_t item, uint8_t direction)
{
  // check if item is cursor or display or direction is left or right
  if((item != HD44780_DISPLAY) && (item != HD44780_CURSOR)) {
    // error
    return PCF8574_ERROR;
  }
  // check if direction is left or right
  if((direction != HD44780_RIGHT) && (direction != HD44780_LEFT)) {
    // error
    return PCF8574_ERROR;
  }
  // cursor shift
  if(item == HD44780_CURSOR) {
    // right shift
    if(direction == HD44780_RIGHT) {
      // shit cursor to right
      HD44780_PCF8574_SendInstruction(addr, HD44780_SHIFT | HD44780_CURSOR | HD44780_RIGHT);
    } else {
      // shit cursor to left
      HD44780_PCF8574_SendInstruction(addr, HD44780_SHIFT | HD44780_CURSOR | HD44780_LEFT);
    }
  // display shift
  } else {
    // right shift
    if(direction == HD44780_RIGHT) {
      // shit display to right
      HD44780_PCF8574_SendInstruction(addr, HD44780_SHIFT | HD44780_DISPLAY | HD44780_RIGHT);
    } else {
      // shit display to left
      HD44780_PCF8574_SendInstruction(addr, HD44780_SHIFT | HD44780_DISPLAY | HD44780_LEFT);
    }
  }
  // success
  return PCF8574_SUCCESS;
}
