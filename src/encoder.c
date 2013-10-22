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
    long temp;

    // The column clock has a 256 preescaler
    temp = FTM1_count * 256;
    FTM1_count = 0;
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
