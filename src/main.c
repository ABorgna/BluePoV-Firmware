/*
    Partes:


*/

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h"

#include "constants.h"
#include "global.h"
#include "snippets.h"
#include "modules.h"

void rtc_1mS(void);

void main (void){

    CPU_init();
    CPU_extCLK();

    RTC_init(RTC_MSECOND);
    RTC_enableInterrupts(rtc_1mS);

    EnableInterrupts;

    for(;;){
        // Polls for UART input
        serial_update();
    }
}

void rtc_1mS(void){
    // Every 1mS
    ;
}

