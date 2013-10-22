    // Controls the uC modules
/*
 * Tested:
 *      CPU
 *      SCI
 *      RTC
 *      SPI
 *      TPM
 *
 * Done:
 *      ADC
 *      GPIO
 *
 * Todo:
 *      I2C
 *      MTIM
 *      WTF
 */

#ifndef MODULES_H_GUARD
#define MODULES_H_GUARD

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h"
#include "snippets.h"

#include "modules_macros.h"

// Flags

typedef enum {     // todo: separar esto
    MOD_NONE = 0,
    // SCI
    SCI_EVENPARITY = 0x1,
    SCI_ODDPARITY = 0x2,
    SCI_NINEBITS = 0x4,
    // SPI
    SPI_MASTER = 0x1,
    SPI_SLAVE = 0x2,
    SPI_LSBFIRST = 0x4,
    SPI_INVERTCLK = 0x8,
    // RTC
    RTC_SECOND = 0x1,
    RTC_100MSECOND = 0x2,
    RTC_10MSECOND = 0x4,
    RTC_MSECOND = 0x8,
    // ADC
    ADC_8B = 0x0,
    ADC_12B = 0x1,
    ADC_10B = 0x2,
    ADC_LOWPOW = 0x4,
    ADC_CONTINUOUS = 0x8,
    // TPM / PWM
    TPM_OUT_COMP = 0x0,
    TPM_IN_COMP = 0x1,
    TPM_EDGE_ALING = 0x2,
    // IRQ
    IRQ_EDGE = 0x0,
    IRQ_LEVEL = 0x2,
    IRQ_PULLUP = 0x0,
    IRQ_NO_PULLUP = 0x4,
    IRQ_RISING = 0x8,
    IRQ_FALLING = 0x10
} MOD_Flags;


// Funciones

// Clock and COP wd
//void CPU_init(void);                                  // Macro
//void CPU_extCLK(void);                                // Macro

// Serial Comunication Interface
//void SCI_init(long bauds, MOD_Flags flags);           // Macro
//void SCI_stop(void);                                  // Macro
void SCI_enableRxInterrupts(void (*function)(void));
void SCI_enableTxInterrupts(void (*function)(void));
//void SCI_disableInterrupts(void);                     // Macro
void SCI_write(uchar);
uchar SCI_read(void);
//SCI_rxPoll /* True if buffer full */

// Serial Peripheral Interface
void SPI_init(long bauds, MOD_Flags flags);
void SPI_stop(void);
void SPI_enableRxInterrupts(void (*function)(void));
void SPI_enableTxInterrupts(void (*function)(void));
void SPI_disableRxInterrupts(void);
void SPI_disableTxInterrupts(void);
void SPI_write(uchar);
uchar SPI_read(void);
uchar SPI_txBufferEmpty(void);
uchar SPI_rxBufferReady(void);
#define SPI_WRITE(data) {while(!SPI1S_SPTEF); SPI1D = data;}
#define SPI_TXBUFFEREMPTY() {SPIS_SPTEF}

// Interrupt 1S or 1mS
//void RTC_init(MOD_Flags flags);                       // Macro
void RTC_stop(void);
void RTC_enableInterrupts(void (*function)(void));
void RTC_disableInterrupts(void);

// Timer Pulse-Width Modulator
void TPM_init(uchar module);
void TPM_stop(uchar module);
void TPM_setFrequency(uchar module, uint frequency);    // Resets duty
void TPM_enableChannel(uchar module, uchar channel);
void TPM_disableChannel(uchar module, uchar channel);
void TPM_setDuty(uchar module, uchar channel, uint duty); // duty in 0.1% steps

// Analog-Digital Converter
void ADC_init(MOD_Flags flags);
void ADC_stop(void);
void ADC_setChannel(uchar channel);
void ADC_enableInterrupts(void (*function)(void));
void ADC_disableInterrupts(void);
void ADC_conv(void);    // Blocking
void ADC_convNB(void);  // Non-blocking
uchar ADC_res(void);
uint ADC_res_i(void);
#define ADC_RES ADCRL /* 8b */
#define ADC_RES_I ADCR /* 16b */

// General Purpose I/O
void GPIO_out(uchar *port,uchar offset);
void GPIO_pullup(uchar *port,uchar offset);

// Keyboard Interrupt
void KBI_interrupt(uchar *port,uchar offset,uchar positive);
void KBI_enableInterrupts(uchar *port,void (*function)(void),uchar level);
void KBI_disableInterrupts(uchar *port);

// Interrupt Request Pin
void IRQ_init(MOD_Flags flags);
void IRQ_stop(void);
void IRQ_enableInterrupts(void (*function)(void));

// FlexTimer Module 1, busclk / 2**prescaler
//void FTM1_init(uchar prescaler);      // Macro
//void FTM1_setMod(uint);               // Macro
//uint FTM1_count;                      // Macro
void FTM1_enableInterrupts(void (*function)(void));

// FlexTimer Module, busclk / 2 / 2**prescaler
//void FTM2_init(uchar prescaler);      // Macro
//void FTM2_setMod(uint);               // Macro
//uint FTM2_count;                      // Macro
void FTM2_enableInterrupts(void (*function)(void));

#endif // MODULES_H_GUARD
