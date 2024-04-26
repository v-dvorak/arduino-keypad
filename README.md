# Keypad v1.0

Connects a matrix keypad to a receiver. Characters are displayed on an LCD and sent via USB (with USART). While typing, you can switch between uppercase and lowercase letters and numbers. The implementation supports character deletion.

![](/docs/keypad_banner.jpg)

For code documentation see projects website.

## Physical components

- **Arduino UNO board with ATmega328P** - the core of the device, communicates with outer devices via USB, other similar devices might also work
- **matrix keypad** - used for device control
- **LCD HD44780 16x2** - displays the text written by the user
- **I2C converter for LC**D - ensures communication conversion between the board and the LCD

## Features

- displaying typed characters on the LCD and sending them via USB
  - the last two lines of text are shown on the LCD
  - characters are sent via USB one by one
- when pressed repeatedly, the currently selected letter is displayed at the cursor position
- customizable key mapping (in the source code)
- customizable macro functions for the `A`, `B`, `C`, `D` buttons, default configurations are as follows:
- toggling between uppercase and lowercase letters and numbers
  - uppercase/lowercase toggled with `A`
  - numbers toggled with `B`
  - when transitioning from uppercase letters to numbers and back, uppercase letters are maintained
- scrolling on the LCD
  - scrolling on the display is possible using `C`
  - scrolling does not affect the transmitted data and is not equivalent to `\n`; it is purely visual
- deleting characters using `D`
- the LED lights up while any key is being pressed

## Wiring diagram

![](/docs/schematic.png)

## Possible improvements

### Overall project improvements

- More keyboard modes and improved indication (e.g., using multiple differently colored LEDs).
- On-the-fly key mapping configuration. Entering a menu where the user can click to choose which letters they want on the keys.
- On/Off button.
- Placing the device in a compact enclosure, approximately 9x12 cm.
- Bluetooth, Wi-Fi connectivity to the computer, enabling text input from virtually anywhere.
- Linking with a PC to use the device as a full-fledged keyboard. Currently, I only display its output in a terminal monitoring the serial link.
- Mouse cursor control mode. The keypad has enough buttons for it, and we could even support diagonal movement.

### Code improvements

- Extract key mapping to a config file.
- Get rid of active waiting, instead rely on interrupts.

## Acknowledgments

The communication between the Arduino UNO board and LCD via I2C is managed by the library [**HD44780_PCF8574**](https://github.com/Matiasus/HD44780_PCF8574) by [**Matiasus**](https://github.com/Matiasus).