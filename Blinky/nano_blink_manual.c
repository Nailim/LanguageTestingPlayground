#include <avr/io.h>




const uint8_t led_pin = 5;
const uint8_t led_bit = 1 << led_pin;

#define loop_ms 0x0a52

void delay(uint16_t ms) {
    uint16_t count = 0;
    while (count < ms) {
        uint16_t loop = 0;
        while (loop < loop_ms) {
            __asm__ __volatile__ ("nop\n\t");
            loop += 1;
        }
        count += 1;
    }
}


int main (void) {
    DDRB |= _BV(DDB5);
    PORTB = led_bit;
    
    
    
    while(1) {
        PORTB ^= led_bit;
        delay(500);
    }
}

