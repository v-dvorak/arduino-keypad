/** 
 * ---------------------------------------------------------------+ 
 * @desc        HD44780 with PCF8574 8-bit I/O expander for TWI 
 * ---------------------------------------------------------------+ 
 *              Copyright(C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko(mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       18.11.2020
 * @file        hd44780pcf8547.h
 * @tested      AVR Atmega328p
 *
 * @depend      twi, pcf8547
 * ---------------------------------------------------------------+
 */
#ifndef __HD44780PCF8574_H__
#define __HD44780PCF8574_H__

#include <avr/io.h>
#include <avr/pgmspace.h>

  #define PCF8574_SUCCESS         0
  #define PCF8574_ERROR           1
  #define PCF8574_ADDRESS      0x27


  #define PCF8574_PIN_RS       0x01
  #define PCF8574_PIN_RW       0x02
  #define PCF8574_PIN_E        0x04
  #define PCF8574_PIN_P3       0x08
  #define PCF8574_PIN_DB4      0x10
  #define PCF8574_PIN_DB5      0x20
  #define PCF8574_PIN_DB6      0x40
  #define PCF8574_PIN_DB7      0x80

  #define HD44780_BUSY_FLAG    HD44780_DB7
  #define HD44780_INIT_SEQ     0x30
  #define HD44780_DISP_CLEAR   0x01
  #define HD44780_DISP_OFF     0x08
  #define HD44780_DISP_ON      0x0C
  #define HD44780_CURSOR_ON    0x0E
  #define HD44780_CURSOR_BLINK 0x0F
  #define HD44780_RETURN_HOME  0x02 
  #define HD44780_ENTRY_MODE   0x06
  #define HD44780_4BIT_MODE    0x20
  #define HD44780_8BIT_MODE    0x30
  #define HD44780_2_ROWS       0x08
  #define HD44780_FONT_5x8     0x00
  #define HD44780_FONT_5x10    0x04
  #define HD44780_POSITION     0x80

  #define HD44780_SHIFT        0x10
  #define HD44780_CURSOR       0x00
  #define HD44780_DISPLAY      0x08
  #define HD44780_LEFT         0x00
  #define HD44780_RIGHT        0x04

  #define HD44780_ROWS         2
  #define HD44780_COLS         16

  #define HD44780_ROW1_START   0x00
  #define HD44780_ROW1_END     HD44780_COLS
  #define HD44780_ROW2_START   0x40
  #define HD44780_ROW2_END     HD44780_COLS
  
  // set bit
  #define SETBIT(REG, BIT) { REG |=(1 << BIT); }
  // clear bit
  #define CLRBIT(REG, BIT) { REG &= ~(1 << BIT); }
  // set port / pin if bit is set
  #define SET_IF_BIT_IS_SET(REG, PORT, DATA, BIT) { if((DATA & BIT) > 0) { SETBIT(REG, PORT); } }

  /**
   * @desc    LCD init - initialisation routine
   *
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t HD44780_PCF8574_Init(uint8_t);

  /**
   * @desc    LCD E pulse
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void HD44780_PCF8574_E_pulse(uint8_t);

  /**
   * @desc    LCD send instruction
   *
   * @param   uint8_t
   * @param   uint8_t
   *
   * @return  void
   */
  void HD44780_PCF8574_SendInstruction(uint8_t, uint8_t);

  /**
   * @desc    LCD Send data 8 bits in 4 bits mode
   *
   * @param   uint8_t
   * @param   uint8_t
   *
   * @return  void
   */
  void HD44780_PCF8574_SendData(uint8_t, uint8_t);

  /**
   * @desc    LCD check BF
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void HD44780_PCF8574_CheckBF(uint8_t);

  /**
   * @desc    LCD send 4bits in 4 bit mode
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void HD44780_PCF8574_Send_4bits_M4b_I(uint8_t);

  /**
   * @desc    LCD send 8bits in 4 bit mode
   *
   * @param   uint8_t
   * @param   uint8_t
   * @param   uint8_t
   *
   * @return  void
   */
  void HD44780_PCF8574_Send_8bits_M4b_I(uint8_t, uint8_t, uint8_t);

  /**
   * @desc    LCD display clear
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void HD44780_PCF8574_DisplayClear(uint8_t);

  /**
   * @desc    LCD display on
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void HD44780_PCF8574_DisplayOn(uint8_t);
  
  /**
   * @desc    LCD display off (ADDED)
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void HD44780_PCF8574_DisplayOff(uint8_t);

  /**
   * @desc    LCD cursor on, display on
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void HD44780_PCF8574_CursorOn(uint8_t);

  /**
   * @desc    LCD cursor blink, cursor on, display on
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void HD44780_PCF8574_CursorBlink(uint8_t);

  /**
   * @desc    LCD draw uint8_t
   *
   * @param   uint8_t
   * @param   uint8_t
   *
   * @return  void
   */
  void HD44780_PCF8574_DrawChar(uint8_t, char);

  /**
   * @desc    LCD draw string
   *
   * @param   uint8_t
   * @param   uint8_t *
   *
   * @return  void
   */
  void HD44780_PCF8574_DrawString(uint8_t, char *);

  /**
   * @desc    LCD Go to position x, y
   *
   * @param   uint8_t
   * @param   uint8_t
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t HD44780_PCF8574_PositionXY(uint8_t, uint8_t, uint8_t);

  /**
   * @desc    Shift cursor / display to left / right
   *
   * @param   uint8_t addr
   * @param   uint8_t item {HD44780_CURSOR; HD44780_DISPLAY}
   * @param   uint8_t direction {HD44780_RIGHT; HD44780_LEFT}
   *
   * @return  uint8_t
   */
  uint8_t HD44780_PCF8574_Shift(uint8_t, uint8_t, uint8_t);

#endif
