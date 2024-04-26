#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "keypad.h"

void Keypad::Init(void) {
    DDRD |= 0xF0;
    PORTD &= 0x0F;
    DDRB &= 0xF0;
    PORTB |= 0x0F;

    LastKeyToDefault();
    CurrentKeyToDefault();

    const char* input[][COLUMN_COUNT] = {
        {".,!„#$%&()*+-", "abc", "def"},
        {"ghi", "jkl", "mno"},
        {"pqrs", "tuv", "wxyz"},
        {"\r", " ", "\n"},

    };
    data = ConvertToStructs(input);
}

void Keypad::Scan(void) {
    if ((PINB & 0x0F) == 0x0F) {
        CurrentKeyToDefault();
        return;
    }

    DDRD &= 0x0F;
    PORTD |= 0xF0;
    DDRB |= 0x0F;
    PORTB &= 0xF0;
    _delay_us(SCAN_DELAY);
    uint8_t pressValue = (PIND & 0xF0);

    // switch data direction
    DDRD |= 0xF0;
    PORTD &= 0x0F;
    DDRB &= 0xF0;
    PORTB |= 0x0F;
    _delay_us(SCAN_DELAY);
    pressValue |= (PINB & 0x0F);

    currentKey = BinaryToKey(~pressValue);
}

uint8_t Keypad::LastKeyValue(void) {
    return GetKeyValue(lastKey);
}

uint8_t Keypad::CurrentKeyValue(void) {
    return GetKeyValue(currentKey);
}

char Keypad::GetKeyValue(const Key& key) {
    char c = 0;
    switch (padState)
    {
        case upperCase:
            c -= ASCII_UPPER_CASE_OFFSET;
        case lowerCase:
            c += data[key.row][key.column].GetValue(timesPressed);
            if (data[key.row][key.column].GetValue(timesPressed) == ' ') c = ' ';
            break;

        case numbers:
            c = GetNumberFromCoordinates(key) + ASCII_NUMERAL_OFFSET;
            break;
        default:
            break;
    }
    return c;
}

uint8_t Keypad::GetNumberFromCoordinates(const Key& key) {
    // suppose the numpad looks like this:
    // +-------+
    // | 1 2 3 |
    // | 4 5 6 |
    // | 7 8 9 |
    // | x 0 x |
    // +-------+
    if (-1 < key.column && key.column < 3 && -1 < key.row && key.row < 3) {
        return key.column + key.row * 3 + 1;
    }
    else {
        return 0;
    }
}

bool Keypad::CurrentIsSingleChar(void) {
    return data[currentKey.row][currentKey.column].IsSingleCharacter();
}

void Keypad::LastKeyToDefault(void) {
    lastKey = Key{-1, -1};
}

void Keypad::CurrentKeyToDefault(void) {
    currentKey = Key{-1, -1};
}

void Keypad::MakeCurrentToLast(void) {
    lastKey = currentKey;
}

void Keypad::SwitchLetterCase(void) {
    if (padState == lowerCase) {
        padState = upperCase;
    }
    else {
        padState = lowerCase;
    }
}

void Keypad::SwitchToNumeral(void) {
    // save settings for case: LC -> UP -> NUM -> UP (by default, it NUM -> LC)
    if (padState != numbers) {
        lastWasUpperCase = (padState == upperCase);
        padState = numbers;
    }
    else {
        if (lastWasUpperCase) padState = upperCase;
        else padState = lowerCase;
        lastWasUpperCase = false;
    }

}

int FirstSetBit(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        if ((data & (1 << i))) return i;
    }
    return -1; // default "error" value
}

Key BinaryToKey(uint8_t data) {
    return Key{FirstSetBit((data & 0x0F)), FirstSetBit(data >> 4)};
}

StringInfo** ConvertToStructs(const char* input[][COLUMN_COUNT]) {
    StringInfo** result = new StringInfo*[ROW_COUNT];

    for (int i = 0; i < ROW_COUNT; ++i) {
        result[i] = new StringInfo[COLUMN_COUNT];
        for (int j = 0; j < COLUMN_COUNT; ++j) {
            result[i][j].str = input[i][j];
            result[i][j].length = strlen(input[i][j]);
        }
    }
    return result;
}