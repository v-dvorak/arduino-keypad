#pragma once
#include "key.h"
#include "strinfo.h"
#include <avr/io.h>

enum PadState { lowerCase, upperCase, numbers };

class Keypad {
public:
    bool pressed = false;
    uint8_t timesPressed = 0;
    Key lastKey;
    Key currentKey;
    PadState padState = lowerCase;
    StringInfo** data;
private:
    bool lastWasUpperCase = false;
public:
    // call before anything else
    void Init(void);

    void Scan(void);

    // get key data
    uint8_t LastKeyValue(void);
    uint8_t CurrentKeyValue(void);
    bool CurrentIsSingleChar(void);
    char GetKeyValue(const Key&);
    uint8_t GetNumberFromCoordinates(const Key&);

    // key manipulation
    void LastKeyToDefault(void);
    void CurrentKeyToDefault(void);
    void MakeCurrentToLast(void);

    void SwitchLetterCase(void);
    void SwitchToNumeral(void);
};

int FirstSetBit(uint8_t data);

Key BinaryToKey(uint8_t data);

StringInfo** ConvertToStructs(const char* input[][COLUMN_COUNT]);
