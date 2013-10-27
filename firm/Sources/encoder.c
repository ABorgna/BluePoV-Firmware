#include "encoder.h"

// Internal declatations
void fps_inter(void);

// External functions
void fps_init(void){
    // Signal input, active high
    IRQ_init(IRQ_EDGE|IRQ_NO_PULLUP|IRQ_RISING);

    FTM1_init(0);
}

// Internal functions
void fps_inter(void){
	ulong temp;

    // Reset the coordinates
    LED_column = 0;
    LED_subcolumn = 0;
    LED_columnByte = 0;

    // The column clock has a 256 preescaler
    temp = (ulong)FTM1_getCount() * 256;
    FTM1_resetCount();
    // Interrupt in every column
    temp /= MX_totalWidth;
    // When showing a 2-bits depth interrupt in the sub-columns
    FPS_clockMod_real /= MX_depth == 1 ? 1 : 3;

    // Set the clock modulo, with a defined hysteresis
    if(FPS_clockMod_real-FPS_HYSTERESIS > FPS_clockMod_actual){
        FPS_clockMod_actual = FPS_clockMod_real - FPS_HYSTERESIS;
    }
    else if (FPS_clockMod_real+FPS_HYSTERESIS < FPS_clockMod_actual){
        FPS_clockMod_actual = FPS_clockMod_real + FPS_HYSTERESIS;
    }
}
