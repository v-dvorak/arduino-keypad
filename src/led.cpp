#include "led.h"
#include "config.h"

CLED::CLED(int t_pin) {
    m_pin = t_pin;
    MASK = (1 << m_pin);
    SetOutput();
    TurnOff();
}

void CLED::TurnOff(void) {
    LED_PORTn &= ~MASK;
    turnedOn = false;
}

void CLED::TurnOn(void) {
    LED_PORTn |= MASK;
    turnedOn = true;
}

void CLED::SetOutput(void) {
    LED_DDRn |= MASK;
}
