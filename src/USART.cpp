#include "USART.h"
#include "config.h"

USART_Com::USART_Com(unsigned int ubrr) {
    Init(ubrr);
}

void USART_Com::Init(unsigned int ubrr) {
    /* Set baud rate */
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;

    /* Enable receiver and transmitter */
    UCSR0B = ((1 << RXEN0) | (1 << TXEN0));

    /* Set frame format: 8data, 2stop bit */
    UCSR0C = ((1 << USBS0) | (3 << UCSZ00));
}

void USART_Com::TransmitNl(auto data) {
    Transmit(data);
    TransmitNl();
}

void USART_Com::TransmitNl(void) {
    Transmit("\n");
}

void USART_Com::Transmit(const char* str) {
    int i = 0;
    char current = str[i];
    while (current != 0) {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = current;
        i++;
        current = str[i];
    }
}

void USART_Com::Transmit(char* str) {
    int i = 0;
    char current = str[i];
    while (current != 0) {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = current;
        i++;
        current = str[i];
    }
}

void USART_Com::Transmit(int data) {
    /* Wait for empty transmit buffer */
    while (!(UCSR0A & (1 << UDRE0)));
    /* Put data into buffer, sends the data */
    UDR0 = data;
}

void USART_Com::TransmitNumeral(int num) {
    Transmit(num + ASCII_NUMERAL_OFFSET);
}

void USART_Com::WriteOutKeypadInfo(StringInfo**& data) {
    for (int i = 0; i < ROW_COUNT; i++) {
        for (int j = 0; j < COLUMN_COUNT; j++) {
            Transmit(data[i][j].str);
            Transmit(" ");
            TransmitNumeral(data[i][j].length);
            Transmit(" ");
        }
        TransmitNl();
    }
}
