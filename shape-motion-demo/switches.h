#ifndef switches_included
#define switches_included

//#define SW1 BIT3 /* switch1 is p1.3 */
//#define SWITCHES SW1 /* only 1 switch on this board */

#define SW1 BIT0
#define SW2 BIT1
#define SW3 BIT2
#define SW4 BIT3
#define SWITCHES (SW1|SW2|SW3|SW4)

void switch_init();
void switch_interrupt_handler();

extern char switch_state_down1 , switch_state_changed, switch_state_down2, switch_state_down3, switch_state_down4; /* effectively boolean */
#endif // included
