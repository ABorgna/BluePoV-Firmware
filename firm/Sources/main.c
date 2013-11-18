/*
    todo:
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

void seg(void);

void main (void){

    CPU_init();
    CPU_extCLK();

    // Get the id (even/odd)
    id_init();

    // Encoder input
    //fps_init();

    // Leds!
    led_init();

    // Serial coms, over bluetooth
    serial_init();

    EnableInterrupts;


    /* --------- TEST ---------- *
    FTM2_setMod(0x0200); // clk/2**12 = bus/2**20
    FTM2_init(7); // bus/2**8
    
    FTM2_enableInterrupts(seg);

	SPI_init(SPI_BAUDS,SPI_MASTER|SPI_INVERTCLK);

    // Negated
    LED_OUT_EN = 0;
    LED_OUT_EN_PORT = 1;
    // Rising
    LED_OUT_CLK = 0;
    LED_OUT_CLK_PORT = 1;

    /* -------- /TEST ---------- */

    for(;;){
        // Polls for UART input
    	serial_update();
    	//__RESET_WATCHDOG();	/* feeds the dog */
    }
}

void seg(void){
	static unsigned long long a = 0;
    static uint c;
    static uchar i = 0;

    switch(i++){
    	case 0:
			LED_OUT_CLK = 1;

			/**/
			
			a = ~((unsigned long long)0);
			
			/**
			
			a  = (unsigned long long)1<<(((c/7) %16)*3+0);
			a |= (unsigned long long)1<<(((c/11) %16)*3+1);
			a |= (unsigned long long)1<<(((c/17) %16)*3+2);
			a |= (unsigned long long)3<<(45-((c/13) %16)*3+0);
			a |= (unsigned long long)5<<(45-((c/19) %16)*3+0);
			a |= (unsigned long long)6<<(45-((c/25) %16)*3+0);

			c++;

			/**/
			
			SPI_WRITE(((a>>40)&0xff)^0xff);
			break;
    	case 1:
    	    SPI_WRITE(((a>>32)&0xff)^0xff);
			break;
    	case 2:
    	    SPI_WRITE(((a>>24)&0xff)^0xff);
			break;
    	case 3:
    	    SPI_WRITE(((a>>16)&0xff)^0xff);
			break;
    	case 4:
    	    SPI_WRITE(((a>> 8)&0xff)^0xff);
			break;
    	case 5:
    	    SPI_WRITE(((a>> 0)&0xff)^0xff);
    	    LED_OUT_CLK = 0;
    	    i = 0;
			break;
    }
}
