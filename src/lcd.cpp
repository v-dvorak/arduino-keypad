#include "lcd.h"

void LCD::Init(void) {
    HD44780_PCF8574_Init(LCD_I2C_ADDR);
    ResetScreen();
    ResetBuffer();
}

void LCD::WriteSingleChar(char data) {
    if (data == NEW_LINE) {
        SwitchToNewLine();
    }
    else {
        HD44780_PCF8574_DrawChar(LCD_I2C_ADDR, data);
        UpdateBuffer(data);
        UpdatePosition(1);
    }
}

void LCD::Delete(void) {
    if (IsDeletable()) {
        x -= 1;
        buffer[x] = BUFFER_DEFAULT;
        HD44780_PCF8574_Shift(LCD_I2C_ADDR, HD44780_CURSOR, HD44780_LEFT);
        HD44780_PCF8574_DrawChar(LCD_I2C_ADDR, ' ');
        HD44780_PCF8574_Shift(LCD_I2C_ADDR, HD44780_CURSOR, HD44780_LEFT);
    }
}

bool LCD::IsDeletable(void) {
    return (x > 0);
}

void LCD::UpdateChar(char data) {
    HD44780_PCF8574_DrawChar(LCD_I2C_ADDR, data);
    HD44780_PCF8574_PositionXY(LCD_I2C_ADDR, x, y); // mark for rewriting
}

void LCD::Scroll(void) {
    SwitchToNewLine();
}

void LCD::UpdatePosition(int howMuch) {
    x += howMuch;
    if (x + howMuch > HD44780_COLS) {
        RowOverflow();
    }
    else if (x < 0) x = 0;
}

void LCD::RowOverflow(void) {
    if (y != 0) {
        HD44780_PCF8574_DisplayClear(LCD_I2C_ADDR);
        HD44780_PCF8574_PositionXY(LCD_I2C_ADDR, 0, 0);
        WriteOutBuffer(HD44780_COLS);
    }
    y = 1; x = 0;
    HD44780_PCF8574_PositionXY(LCD_I2C_ADDR, x, y);
}

void LCD::SwitchToNewLine(void) {
    if (y == 0) {
        // screen looks like this:
        // +---------------------------+  y
        // | ABCDEF... _               |  0
        // |                           |  1
        // +---------------------------+

        // if current row is empty (x == 0), start typing at first row
        if (x == 0) y = 0;
        else y = 1;
        x = 0;
        HD44780_PCF8574_PositionXY(LCD_I2C_ADDR, x, y);

        // screen looks like this:
        // +---------------------------+  y
        // | ABCDEF...                 |  0
        // | _                         |  1
        // +---------------------------+
    }
    else {
        // screen looks like this:
        // +---------------------------+  y
        // | ABCDEF...                 |  0
        // | GHIJKL... _               |  1
        // +---------------------------+

        // reset display
        HD44780_PCF8574_DisplayClear(LCD_I2C_ADDR);
        HD44780_PCF8574_PositionXY(LCD_I2C_ADDR, 0, 0);
        
        WriteOutBuffer(x);
        ResetBuffer();
        
        // if current row is empty (x == 0), start typing at first row
        if (x == 0) y = 0;
        else y = 1;
        x = 0;

        HD44780_PCF8574_PositionXY(LCD_I2C_ADDR, x, y);
        // screen looks like this:
        // +---------------------------+  y
        // | GHIJKL...                 |  0
        // | _                         |  1
        // +---------------------------+
    }
}

void LCD::WriteOutBuffer(int written) {
    for (int i = 0; i < written; i++) {
        HD44780_PCF8574_DrawChar(LCD_I2C_ADDR, buffer[i]);
    }
}

void LCD::ResetBuffer(void) {
    for (int i = 0; i < HD44780_COLS; i++) {
        buffer[i] = BUFFER_DEFAULT;
    }
}

void LCD::UpdateBuffer(char data) {
    if (y == 0) return;
    buffer[x] = data;
}

void LCD::ResetScreen(void) {
    // display clear
    HD44780_PCF8574_DisplayClear(LCD_I2C_ADDR);
    // display on
    HD44780_PCF8574_DisplayOn(LCD_I2C_ADDR);
    HD44780_PCF8574_CursorOn(LCD_I2C_ADDR);
    // cursor blinking causes too much distraction while typing out letters,
    // thus it is not used in this implementation

    // reset coordinates and position
    int x = 0;
    int y = 0;
    HD44780_PCF8574_PositionXY(LCD_I2C_ADDR, x, y);
}