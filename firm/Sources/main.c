/*
    todo:
    	id
        Led controller

        Half luminance
        Battery voltage
        Saving
*/

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h"

#include "constants.h"
#include "global.h"
#include "snippets.h"

#include "encoder.h"
#include "leds.h"
#include "serial.h"

void main (void){

    CPU_init();
    //CPU_extCLK();

    // Encoder input
    fps_init();

    // Leds!
    led_init();

    EnableInterrupts;

    for(;;){
        // Polls for UART input
    	serial_update();
    	//__RESET_WATCHDOG();	/* feeds the dog */
    }
}
