/*
    todo:
    	id

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
#include "other.h"

void main (void){

    CPU_init();
    //CPU_extCLK();
    
    // Get the id (even/odd)
    id_init();

    // Encoder input
    fps_init();

    // Leds!
    led_init();
    
    // Serial coms, over bluetooth
    serial_init();

    EnableInterrupts;
    

    /* --------- TEST ---------- */
    SPI_write(0x55);
    
    LED_OUT_CLK = 1;
    LED_OUT_CLK = 0;
    /* -------- /TEST ---------- */

    for(;;){
        // Polls for UART input
    	serial_update();
    	//__RESET_WATCHDOG();	/* feeds the dog */
    }
}
