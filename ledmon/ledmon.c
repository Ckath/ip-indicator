/* see LICENSE file for copyright and license details. */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "lib/usart.h"
#include <util/delay.h>
#include <stdint.h>

enum { 
    CORRECT_IP = '0', 
    WRONG_IP = '1', 
    NO_IP = '2'
};
typedef enum {
    RED    = 0x02,
    YELLOW = 0x04,
    GREEN  = 0x08,
    OFF    = 0x00
} led_color;
led_color color;

volatile uint8_t delay = 0;
volatile uint8_t update = 0, last_update = 0, update_error = 1;
volatile uint16_t no_update = 0;

ISR(TIMER2_OVF_vect)
{
    if (++delay > 100) {
        delay = 0;

        /* if no updates have been received 
           10 times in a row blink error pattern,
           otherwise blink status. */
        if (update == last_update) {
            if (++no_update > 10) {
                no_update = 0;
                update_error = 1;
            }
        } else {
            last_update = update;
            update_error = 0;
            no_update = 0;
        }

        if (!update_error) {
            PORTB = color;
            _delay_ms(300);
            PORTB = OFF;
        } else {
            PORTB = GREEN;
            _delay_ms(100);
            PORTB = YELLOW;
            _delay_ms(100);
            PORTB = RED;
            _delay_ms(100);
            PORTB = OFF;
        }
    }
}

int main() {
    /* setup outputs */
    DDRD = RED | YELLOW | GREEN;

    /* setup the timer interrupt, 
       start interrupt and usart */
    TCCR2B |= (1 << CS22) | (1 << CS20);
    TIMSK2 |= (1 << TOIE2);
    TCNT2 = 0;
    sei();
    USART_init(9600);

    for (;;) {
        switch (USART_getch()) {
            case CORRECT_IP:
                color = GREEN;
                update++;
                break;
            case WRONG_IP:
                color = YELLOW;
                update++;
                break;
            case NO_IP:
                color = RED;
                update++;
                break;
        }
    }
}
