#include "modules.h"

// Get a pointer to a register given the module number and name
// Eg: module(SCIxBDL,2) -> *SCI2BDL
#define getModPtr(reg,num) *(&(reg)+(reg##_off)*((num)-1))



// SCI

void (*SCI_rxInterruptFunction)(void);
void (*SCI_txInterruptFunction)(void);
volatile uchar SCI_readData;

void SCI_enableRxInterrupts(void (*function)(void)){
    // Enable interrupts from Rx (SCIS1_RDRF)
    // Call the given function on desired interruption
    SCI_rxInterruptFunction = function;
    SCI_readData = 0;
    SCI1C2_RIE = 1;
}
void SCI_enableTxInterrupts(void (*function)(void)){
    // Enable interrupts from Rx (SCIS1_RDRF)
    // Call the given function on desired interruption
    SCI_txInterruptFunction = function;
    SCI1C2_TCIE = 1;
}
uchar SCI_read(){
    return SCI1C2_RIE ? SCI_readData:SCI1D;
}
void SCI_write(uchar data){
    while(!SCI1S1_TDRE);
    SCI1D = data;
}

__interrupt VectorNumber_Vsci1rx void SCI_rxInterrupt(void){
    volatile uchar a;
    // Cleans the interrupt flag
    a = SCI1S1_RDRF;
    SCI_readData = SCI1D;
    // Calls the function
    SCI_rxInterruptFunction();
}
__interrupt VectorNumber_Vsci1tx void SCI_txInterrupt(void){
    SCI_txInterruptFunction();
}



// SPI

void (*SPI_rxInterruptFunction)(void);
void (*SPI_txInterruptFunction)(void);

void SPI_init(long bauds, MOD_Flags flags){
    int prescaler, scaler;

    // Set as master / slave
    SPI1C1_MSTR = flags & SPI_SLAVE ? 0 : 1;
    // Transmit less significant bit first
    SPI1C1_LSBFE = flags & SPI_LSBFIRST ? 1 : 0;
    // Invert the clock polarity, 1 = active low
    SPI1C1_CPOL = flags & SPI_INVERTCLK ? 1 : 0;

    // Set baud rate
    // Min rate: BUSCLK / 2048
    // Max rate: BUSCLK / 2
    scaler = (BUSCLK/bauds)&0xFFFF;
    if (scaler > 256){
        prescaler = (scaler / 256) - 1;
        scaler = 256;
    }
    else
        prescaler =  0;
    scaler = LOG2_i(scaler) - 1;
    SPI1BR = (prescaler << 4) | scaler;

    // Enable the module
    SPI1C1_SPE = 1;
}
void SPI_stop(void){
    SPI_disableRxInterrupts();
    SPI_disableTxInterrupts();
    SPI1C1_SPE = 0;
}
void SPI_enableRxInterrupts(void (*function)(void)){
    SPI_rxInterruptFunction = function;
    // Enable interrupts
    SPI1C1_SPIE = 1;
}
void SPI_enableTxInterrupts(void (*function)(void)){
    SPI_txInterruptFunction = function;
    // Enable interrupts
    SPI1C1_SPTIE = 1;
}
void SPI_disableRxInterrupts(void){
    SPI1C1_SPIE = 0;
}
void SPI_disableTxInterrupts(void){
    SPI1C1_SPTIE = 0;
}
void SPI_write(uchar data){
    while(!SPI1S_SPTEF);
    SPI1D = data;
}
uchar SPI_read(void){
    return SPI1D;
}
uchar SPI_txBufferEmpty(void){
    return SPI1S_SPTEF;
}
uchar SPI_rxBufferReady(void){
    return SPI1S_SPRF;
}

__interrupt VectorNumber_Vspi1 void SPI_Interrupt(void){
    volatile a;
    if(SPI1C1_SPIE & SPI1S_SPRF){
        // Cleans the interrupt flag
        a = SPI1S_SPRF;
        a = SPI1D;
        // Calls the function
        SPI_rxInterruptFunction();
    }
    else if (SPI1C1_SPTIE & SPI1S_SPTEF){
        // Cleans the interrupt flag
        // Must write to spid after this
        a = SPI1S_SPTEF;
        // Calls the function
        SPI_txInterruptFunction();
    }
}



// RTC

void (*RTC_interruptFunction)(void);

void RTC_stop(void){
    RTC_disableInterrupts();
    RTCSC_RTCPS = 0;
}
void RTC_enableInterrupts(void (*function)(void)){
    RTC_interruptFunction = function;
    RTCSC_RTIE = 1;
}
void RTC_disableInterrupts(void){
    RTCSC_RTIE = 0;
}
__interrupt VectorNumber_Vrtc void RTC_interrupt(void){
    RTC_interruptFunction();
    // Cleans the interrupt flag
    RTCSC_RTIF = 1;
}



// TPM

#define reg(regist) (*(&(regist)+offset))
#define regCh(regist) (*(&(regist)+offset+chOffset))

void TPM_init(uchar module){
    uchar offset = TPMxSC_off * (module-1);
    // Center-aligned, clock source = BUSCLK/TPM_BUSDIV
    reg(TPM1SC) = 0x28 | (LOG2_i(TPM_BUSDIV)&0x7);
}

void TPM_stop(uchar module){
    uchar offset = TPMxSC_off * (module-1);
    // Disables the module
    reg(TPM1SC) = 0x0;
}
void TPM_enableChannel(uchar module, uchar channel){
    uchar offset = TPMxSC_off * (module-1);
    uchar chOffset = TPMxCxSC_off * (channel);
    // Reserves the pin
    regCh(TPM1C0SC) = 0x28;
}
void TPM_disableChannel(uchar module, uchar channel){
    uchar offset = TPMxSC_off * (module-1);
    uchar chOffset = TPMxCxSC_off * (channel);
    // Disables de pin as output
    regCh(TPM1C0SC) = 0x00;
}
void TPM_setDuty(uchar module, uchar channel, uint duty){
    // duty in 0.1% steps
    uchar offset = TPMxSC_off * (module-1);
    uchar chOffset = TPMxCxSC_off * (channel);
    // Set the channel module relative to the clock's
    regCh(TPM1C0V) = reg(TPM1MOD)*duty/1000;
}
void TPM_setFrequency(uchar module, uint frequency){
    uchar offset = TPMxSC_off * (module-1);
    // Sets the module counter
    if (frequency)
        reg(TPM1MOD) = ((BUSCLK/TPM_BUSDIV)/frequency)&0xFFFF;
    else
        reg(TPM1MOD) = 0;
}
#undef reg
#undef regCh

// Analog-Digital Converter
void (*ADC_interruptFunction)(void);

void ADC_init(MOD_Flags flags){
    ADC_DISABLEINTERRUPTS();
    ADCSC1_ADCO = 0;
    ADCSC2 = 0;

    ADCCFG_ADLPC = flags & ADC_LOWPOW ? 1 : 0;
    ADCCFG_ADIV = flags & ADC_LOWPOW ? 3 : 0;
    ADCCFG_ADLSMP = flags & ADC_LOWPOW ? 1 : 0;
    ADCCFG_MODE = flags & ADC_12B ? 1 :
                  flags & ADC_10B ? 2 :
                  0; // ADC_8B
}
void ADC_stop(void){
    ADC_DISABLEINTERRUPTS();

    if (ADCSC1_ADCH < ADC_PORT_CH_MAX)
        *(&APCTL1+APCTLx_off*(ADCSC1_ADCH/8)) = 0;

    ADCSC1_ADCH = 0x1f;
}
void ADC_setChannel(uchar channel){
    if (channel < ADC_PORT_CH_MAX){
        *(&APCTL1+APCTLx_off*(ADCSC1_ADCH/8)) = 0;
        *(&APCTL1+APCTLx_off*(channel/8)) = (1<<channel)&0xff;
    }

    ADCSC1_ADCH = channel;
}
void ADC_enableInterrupts(void (*function)(void)){
    ADC_interruptFunction = function;
    ADCSC1_AIEN = 1;
}
void ADC_disableInterrupts(void){
    ADCSC1_AIEN = 0;
}
void ADC_conv(void){    // Blocking
    ADCSC1_COCO = 0;
    while (ADCSC2_ADACT);
}
void ADC_convNB(void){  // Non-blocking
    ADCSC1_COCO = 0;
}
uchar ADC_res(void){
    while (ADCSC2_ADACT);
    return ADCRL;
}
uint ADC_res_i(void){
    while (ADCSC2_ADACT);
    return ADCR;
}
__interrupt VectorNumber_Vadc void ADC_interrupt(void){
    // Cleans the interrupt flag
    volatile var = ADCRL;
    // Calls the function
    ADC_interruptFunction();
}

// General Purpose I/O
void GPIO_out(uchar *port,uchar offset){
    SNP_maskWrite(port+PTxDD_off,0xff,1<<offset);
}
void GPIO_pullup(uchar *port,uchar offset){
    SNP_maskWrite(port+PTxPE_off,0xff,1<<offset);
}

// Keyboard Interrupt
void KBI_interrupt(uchar *port,uchar offset,uchar positive){
    SNP_maskWrite(port+PTxPS_off,0xff,1<<offset);
    SNP_maskWrite(port+PTxES_off,positive?0:1,1<<offset);
}
void KBI_enableInterrupts(uchar *port,void (*function)(void),uchar level){
    GPIO_interruptFunction = function;
    SNP_maskWrite(port+PTxSC_off,0xff,2);
    SNP_maskWrite(port+PTxSC_off,level?1:0,1);
}
void KBI_disableInterrupts(uchar *port){
    SNP_maskWrite(port+PTxSC_off,0,2);
}

__interrupt VectorNumber_Vkeyboard void KBI_interrupt(void){
    // Cleans the interrupt flag
    SNP_maskWrite(&PTAD+PTxSC_off,0xff,4);   // PTxSC_ACK
    // Calls the function
    GPIO_interruptFunction();
}


// Interrupt Request Pin
void (*IRQ_interruptFunction)(void);

void IRQ_init(MOD_Flags flags){
    // Pullup
    IRQSC_IRQPDD = flags & IRQ_NO_PULLUP ? 1 : 0;
    // Level
    IRQSC_IRQEDG = flags & IRQ_RISING ? 1 : 0;
    // Edge or edge and level
    IRQSC_IRQMOD = flags & IRQ_LEVEL ? 1 : 0;

    // Pin Enable
    IRQSC_IRQPE = 1;
}
void IRQ_stop(void){
    // Cleans the interrupt flag
    IRQSC_IRQACK = 1;
    // Pin Disable
    IRQSC_IRQPE = 0;
}
void IRQ_enableInterrupts(void (*function)(void)){
    // Sets the function
    IRQ_interruptFunction = function;
    // Enables Interruptions
    IRQSC_IRQIE = 1;
}
__interrupt VectorNumber_Virq void IRQ_interrupt(void){
    // Cleans the interrupt flag
    IRQSC_IRQACK = 1;
    // Calls the function
    IRQ_interruptFunction();
}
