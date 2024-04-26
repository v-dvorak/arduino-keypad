#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "config.h"
#include "USART.h"
#include "timer.h"
#include "lcd.h"
#include "keypad.h"
#include "key.h"
#include "led.h"

Timer timer;
Keypad keypad;
LCD lcd;
USART_Com usart = USART_Com(UBRR);
unsigned long lastTransmit;
unsigned long currentTime;

void SendOut(uint8_t );
void Flush(void);
void DebugPressMessage(const Key&, uint8_t);

int main(void)
{
	// COMPONENT INIT
	// lcd init
	lcd.Init();
	// keypad init
	keypad.Init();
	// job init
	timer.Init();
	sei();
	unsigned long lastTransmit = 0; // default
	CLED led = CLED(5);
	// usart init
	usart = USART_Com(UBRR);
	if (DEBUG_PRINT_KEYPAD_READING) usart.WriteOutKeypadInfo(keypad.data);

	// main loop
	while (1)
	{
		// loop initalization
		keypad.Scan();
		unsigned long currentTime = timer.Get();

		// TIME RAN OUT
		// -------------------------------
		// if delay between button presses is greater than this, device sends out the last pressed key
		if (currentTime - lastTransmit > PRESS_DELAY && !keypad.pressed) {
			Flush();
		}
		// -------------------------------

		if (keypad.currentKey.IsValidKey() && !keypad.pressed) {
			led.TurnOn(); // visual indication of button being pressed
			keypad.pressed = true;

			// debug
			if (DEBUG_PRINT_KEYPAD_READING) {
				DebugPressMessage(keypad.currentKey, keypad.timesPressed);
			}

			// ACTION KEYS
			// -------------------------------
			// delete last character
			if (keypad.currentKey == Key{3, 3}) {
				// flush
				Flush();
				// delete
				if (!CAP_DEL_DISPLAY_SIZE || lcd.IsDeletable()) usart.Transmit(DELETE_SYMBOL);
				lcd.Delete();
			}
			// scroll !without! sending new line to usart - scrolling is only visual
			else if (keypad.currentKey == Key{2, 3}) {
				// flush
				Flush();
				// scroll
				lcd.Scroll();
			}
			// switch betweens numerals and letters
			else if (keypad.currentKey == Key{1, 3}) {
				// flush
				Flush();
				// switch between nums and letters
				keypad.SwitchToNumeral();
			}
			// switch between lower case and upper case
			else if (keypad.currentKey == Key{0, 3}) {
				// flush
				Flush();
				// switch between lower/upper case
				keypad.SwitchLetterCase();
			}	
			// -------------------------------

			// STANDARD KEY PROCESSING
			// -------------------------------
			// send character immediately
			else if (keypad.CurrentIsSingleChar() || keypad.padState == numbers) {
				if (keypad.lastKey.IsMainKey()) { 
					// send
					SendOut(keypad.LastKeyValue());
				}
				// send
				SendOut(keypad.CurrentKeyValue());
				// reset
				keypad.timesPressed = 0;
				keypad.CurrentKeyToDefault();
			}
			// scroll through options
			else if (keypad.currentKey == keypad.lastKey) {
				keypad.timesPressed += 1;
				// update LCD
				lcd.UpdateChar(keypad.LastKeyValue());
			}
			// different key was pressed, send out the last key
			else if (keypad.lastKey.IsMainKey()) {
				// send
				SendOut(keypad.LastKeyValue());
				// reset
				keypad.timesPressed = 0;
			}
			// -------------------------------

			// update
			keypad.MakeCurrentToLast();
			lastTransmit = currentTime;
		}
		else if (!keypad.currentKey.IsValidKey() && keypad.pressed) {
			// reset
			keypad.pressed = false;
			lastTransmit = currentTime;
			led.TurnOff(); // visual indication
		}
		_delay_ms(DEBOUNCE_DELAY);
	}
}

void DebugPressMessage(const Key& key, uint8_t timesPressed) {
	usart.TransmitNumeral(key.row);
	usart.Transmit(" ");
	usart.TransmitNumeral(key.column);
	usart.Transmit(" ");
	usart.TransmitNumeral(timesPressed);
	usart.TransmitNl();
}

void Flush(void) {
	if (keypad.lastKey.IsMainKey()) {
		SendOut(keypad.LastKeyValue());
	}
	lastTransmit = currentTime;
	keypad.LastKeyToDefault();
	keypad.timesPressed = 0;
}

void SendOut(uint8_t data) {
	usart.Transmit(data);
	lcd.WriteSingleChar(data);
}