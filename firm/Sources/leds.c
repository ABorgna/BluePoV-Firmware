#include "leds.h"

// Internal declatations
void led_interrupt(void);

// External definitions
void led_init(void){
    FTM2_init(LED_PRESCALER);
    FTM2_setMod(FPS_clockMod_actual);

    FTM2_enableInterrupts(led_interrupt);
}

// Internal definitions
void led_interrupt(void){
    uint data;

    FTM2_setMod(FPS_clockMod_actual);

    if(MX_depth==1){
        data = MX_pixelArray0[LED_column][LED_columnByte];
    }
}
