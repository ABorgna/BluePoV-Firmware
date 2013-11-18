#include "encoder.h"

/* Encoder - pin 62 = PTC2 */

// Internal declatations
void fps_inter(void);

void KBI_interrupt();
void KBI_enableInterrupts(void (*function)(void));
void KBI_disableInterrupts();/**/

// External functions
void fps_init(void){
    // Signal input, active high
	/**
    IRQ_init(IRQ_EDGE|IRQ_NO_PULLUP|IRQ_RISING);
    IRQ_enableInterrupts(fps_inter);
    /*
     *	Checking on led column interrupt 
     */

    FTM2_init(FPS_PRESCALER);
}

void fps_poll(void){
	static uchar encoder_last_state = 1;
			
	if (!ENCODER_IN && encoder_last_state != ENCODER_IN){
		encoder_last_state = ENCODER_IN;
		fps_inter();
	}
	encoder_last_state = ENCODER_IN;
}

// Internal functions
void fps_inter(void){
	ulong temp;

    // The column clock has a 256 preescaler
    temp = (ulong)FTM2_getCount() * 256;
    FTM2_resetCount();
    // Interrupt in every column
    temp /= MX_totalWidth;
    // When showing a 2-bits depth interrupt in the sub-columns
    FPS_clockMod_real /= (MX_depth == 1) ? 1 : 3;

    // Set the clock modulo, with a defined hysteresis
    if(FPS_clockMod_real-FPS_HYSTERESIS > FPS_clockMod_actual){
    	FPS_clockMod_actual = FPS_clockMod_real - FPS_HYSTERESIS;
    }
    else if (FPS_clockMod_real+FPS_HYSTERESIS < FPS_clockMod_actual){
        FPS_clockMod_actual = FPS_clockMod_real + FPS_HYSTERESIS;
    }

    // Reset the coordinates
    led_enable();
}



// Last minute fix...
void led_enable(void){
    LED_column = ((uint)0)-1;
    LED_subcolumn = 0;
    LED_columnByte = 0;
    //FTM1_enableInterrupts(led_col_interrupt);
    LED_enabled = 1;
    FTM1_resetCount();
}
