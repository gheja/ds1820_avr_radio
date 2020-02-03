#define DDR DDRC
#define PORT PORTC
#define PIN_RADIO 0
#define PIN_LED 1

#define BIT_SET(a, b) a |= 1 << b
#define BIT_CLEAR(a, b) a &= !(1 << b)

#define RADIO_ON BIT_SET(PORT, PIN_RADIO)
#define RADIO_OFF BIT_CLEAR(PORT, PIN_RADIO)

#define LED_ON BIT_SET(PORT, PIN_LED)
#define LED_OFF BIT_CLEAR(PORT, PIN_LED)

#define RADIO_ON BIT_SET(PORT, PIN_RADIO)
#define RADIO_OFF BIT_CLEAR(PORT, PIN_RADIO)
