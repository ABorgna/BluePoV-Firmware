// uC module wrappers

#ifndef MODULES_H_GUARD
#define MODULES_H_GUARD

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h"
#include "snippets.h"

#include "modules_defs.h"

// Funciones

// Clock and COP wd
void CPU_init(void);
void CPU_extCLK(void);

// Serial Comunication Interface
void SCI_init(long bauds, MOD_Flags flags);
void SCI_stop(void);
void SCI_enableRxInterrupts(void (*function)(uchar));
void SCI_enableTxInterrupts(void (*function)(void));
void SCI_disableinterrupts(void);
void SCI_write(uchar);
uchar SCI_read(void);
uchar SCI_rxPoll(void); /* True if buffer full */
#define SCI_READ SCI2D /* True if buffer full */
#define SCI_RXPOLL SCI2S1_RDRF /* True if buffer full */
#define SCI_WRITE(data) {while(!SCI2S1_TDRE){}; SCI2D = (data);}

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
#define SPI_WRITE(data) {while(!SPI1S_SPTEF){}; SPI1D = (data);}
#define SPI_TXBUFFEREMPTY() {SPIS_SPTEF}

// Interrupt 1S or 1mS
void RTC_init(MOD_Flags flags);
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
void FTM1_init(uchar prescaler);
void FTM1_setMod(uint);
uint FTM1_getCount(void);
void FTM1_resetCount(void);
void FTM1_enableInterrupts(void (*function)(void));

// FlexTimer Module, busclk / 2 / 2**prescaler
void FTM2_init(uchar prescaler);
void FTM2_setMod(uint);
uint FTM2_getCount(void);
void FTM2_resetCount(void);
void FTM2_enableInterrupts(void (*function)(void));

#endif // MODULES_H_GUARD
