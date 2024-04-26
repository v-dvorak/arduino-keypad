#include <avr/io.h>

/*These values need to be known during compilation;
it is significantly easier to extract them like this,
rather than building a structure for searching pointers to registers
just for one LED (the class takes care of the rest).*/
#define LED_PORTn PORTB
#define LED_DDRn DDRB

class CLED {
private:
    int m_pin;
    int MASK;
    bool turnedOn = false;

public:
    CLED(int t_pin);
    void TurnOff(void);
    void TurnOn(void);
    void SetOutput(void);
};
