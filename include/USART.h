#pragma once
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "strinfo.h"

class USART_Com {
public:
    USART_Com(unsigned int ubrr);

    void TransmitNl(auto data);
    void TransmitNl(void);
    void Transmit(const char* str);
    void Transmit(char* str);
    void Transmit(int data);
    void TransmitNumeral(int num);
    void WriteOutKeypadInfo(StringInfo**& data);

private:
    void Init(unsigned int ubrr);
};