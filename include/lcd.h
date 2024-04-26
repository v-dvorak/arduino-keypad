#include <util/delay.h>
#include "config.h"
#include "hd44780pcf8574.h"

#define BUFFER_DEFAULT      ' '
#define NEW_LINE            '\n'
#define LCD_I2C_ADDR        0x3F    // LCD address on I2C, for address detection see:
// https://github.com/salmanfarisvp/Arduino-Sensor/tree/master/LiquidCrystal_I2C/examples/I2C_Address%20Scanner

class LCD {
private:
    uint8_t x;
    uint8_t y;
    char buffer[HD44780_COLS];

public:
    void Init(void);
    void WriteSingleChar(char data);
    void Delete(void);
    void UpdateChar(char data);
    void Scroll(void);
    bool IsDeletable(void);

private:
    void UpdatePosition(int howMuch);
    void RowOverflow(void);
    void SwitchToNewLine(void);
    void WriteOutBuffer(int written);
    void ResetBuffer(void);
    void UpdateBuffer(char data);
    void ResetScreen(void);
};