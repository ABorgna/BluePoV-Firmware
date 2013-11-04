#include "modules.h"

// Get a pointer to a register given the module number and name
// Eg: module(SCIxBDL,2) -> *SCI2BDL
#define getModPtr(reg,num) *(&(reg)+(reg##_off)*((num)-1))


// Clock and COP
void CPU_init(void){                            
    /* Disable the mighty COP watchdog */       
    SOPT_COPE = 0;
    /* Something something with the bus clock */
    //MCGTRM = *((uchar*)0xFFAF);                 
    //MCGSC_FTRIM = *((uchar*)0xFFAE);            
}
void CPU_extCLK(void){                                                
    /* Assume 12MHz external clock source connected. */  
                                                     
    /* In order to use the USB we need to enter PEE mode and MCGOUT set to 48 MHz.
     Out of reset MCG is in FEI mode. */                                                
                                                                                        
    /**** Moving from FEI (FLL engaged internal) to PEE (PLL engaged external) mode. */ 
    /* switch from FEI to FBE (FLL bypassed external) */                                
    /* enable external clock source */                                                  
    MCGC2 = MCGC2_HGO_MASK      /* oscillator in high gain mode */                      
          | MCGC2_EREFS_MASK    /* because crystal is being used */                     
          | MCGC2_RANGE_MASK    /* 12 MHz is in high freq range */                      
          | MCGC2_ERCLKEN_MASK; /* activate external reference clock */                 
    while (MCGSC_OSCINIT == 0){};                                                         
                                                                                        
    /* select clock mode */                                                             
    MCGC1 = (2<<6)          /* CLKS = 10 -> external reference clock. */                
          | (3<<3);         /* RDIV = 3 -> 12MHz/8=1.5 MHz */                           
                                                                                        
    /* wait for mode change to be done */                                               
    while (MCGSC_IREFST != 0){};                                                          
    while (MCGSC_CLKST != 2){};                                                           
                                                                                        
    /* switch from FBE to PBE (PLL bypassed internal) mode */                           
    MCGC3 = MCGC3_PLLS_MASK                                                             
          | (8<<0);         /* VDIV=6 -> multiply by 32 -> 1.5MHz * 32 = 48MHz */       
    //      | (12<<0);         /* VDIV=6 -> multiply by 48 -> 1.5MHz * 48 = 72MHz */       
    while(MCGSC_PLLST != 1){};                                                            
    while(MCGSC_LOCK != 1){};                                                             
                                                                                        
    /* finally switch from PBE to PEE (PLL enabled external mode) */                    
    MCGC1 = (0<<6)          /* CLKS = 0 -> PLL or FLL output clock. */                  
          | (3<<3);         /* RDIV = 3 -> 12MHz/8=1.5 MHz */                           
    while(MCGSC_CLKST!=3){};                                                              
                                                                                        
    /* Now MCGOUT=48MHz, BUS_CLOCK=24MHz */                                             
}



// SCI
/**/
void (*SCI_rxInterruptFunction)(uchar);
void (*SCI_txInterruptFunction)(void);
void SCI_txInterrupt(void);

void SCI_init(long bauds, MOD_Flags flags){
	uint prescaler = ((BUSCLK/16)/bauds)&0xFFFF;
    // Set baud rate
    SCI2BDL =  prescaler & 0xFF;                                   
    SCI2BDH = (prescaler >> 8) & 0x1F;                              
    // Configure bits & parity
    SCI2C1_M = flags & SCI_NINEBITS ? 1 : 0;     // 8+1b
                                                // Parity enable
    SCI2C1_PE = flags & (SCI_ODDPARITY|SCI_EVENPARITY) ? 1 : 0;     
    SCI2C1_PT = flags & SCI_ODDPARITY ? 1 : 0;   // Parity
    // Enable Tx & Rx
    SCI2C2_TE = 1;                                                  
    SCI2C2_RE = 1;                                                  
}
void SCI_stop(void) {            
	SCI_disableinterrupts();  
    SCI2C2_TE = 0;               
    SCI2C2_RE = 0;               
}
void SCI_enableRxInterrupts(void (*function)(uchar)){
    // Enable interrupts from Rx (SCIS1_RDRF)
    // Call the given function on desired interruption
    SCI_rxInterruptFunction = function;
    SCI2C2_RIE = 1;
}
void SCI_enableTxInterrupts(void (*function)(void)){
    // Enable interrupts from Rx (SCIS1_RDRF)
    // Call the given function on desired interruption
    SCI_txInterruptFunction = function;
    SCI2C2_TCIE = 1;
}
void SCI_disableinterrupts(void) {  
    SCI2C2_TCIE = 0;                 
    SCI2C2_RIE = 0;                  
}
uchar SCI_read(void){
    return SCI2D;
}
void SCI_write(uchar data){
    while(!SCI2S1_TDRE){};
    SCI2D = data;
}
uchar SCI_rxPoll(void){
	// True if buffer full
	return SCI2S1_RDRF;
}

__interrupt VectorNumber_Vsci2rx void SCI_rxInterrupt(void){
    uchar data;
    volatile uchar a;
    // Cleans the interrupt flag
    a = SCI2S1;
    data = SCI2D;
    // Calls the function
    SCI_rxInterruptFunction(data);
}
__interrupt VectorNumber_Vsci2tx void SCI_txInterrupt(void){
    SCI_txInterruptFunction();
}
/**/


// SPI
/**/
void (*SPI_rxInterruptFunction)(void);
void (*SPI_txInterruptFunction)(void);
void SPI_Interrupt(void);

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
    SPI1BR = ((uchar)prescaler << 4) | (uchar)scaler;

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
    while(!SPI1S_SPTEF){};
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
/**/


// RTC
/**/
void (*RTC_interruptFunction)(void);

void RTC_init(MOD_Flags flags){     
	// The colfire implementation (RTI) only allows
	// base-2 divisors, :/
    // 1kHz oscilator                            
    SRTISC_RTICLKS = 0;
    // Preescaler                       
    SRTISC_RTIS = flags;	// RTC_SECOND = 7 = 1.024 s
    						// 1 = 8 mS
}
void RTC_stop(void){
    RTC_disableInterrupts();
    SRTISC_RTIS = 0;
}
void RTC_enableInterrupts(void (*function)(void)){
    RTC_interruptFunction = function;
    SRTISC_RTIE = 1;
}
void RTC_disableInterrupts(void){
	SRTISC_RTIE = 0;
}
__interrupt VectorNumber_Vrti void RTC_interrupt(void){
    RTC_interruptFunction();
    // Cleans the interrupt flag
    SRTISC_RTIACK = 1;
}
/**/


// Analog-Digital Converter
/**
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
/**/

// General Purpose I/O
/**
void GPIO_out(uchar *port,uchar offset){
    SNP_maskWrite(port+PTxDD_off,0xff,1<<offset);
}
void GPIO_pullup(uchar *port,uchar offset){
    SNP_maskWrite(port+PTxPE_off,0xff,1<<offset);
}
/**/

// Keyboard Interrupt
/**
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
/**/

// Interrupt Request Pin
void (*IRQ_interruptFunction)(void);
void IRQ_interrupt(void);

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


// FlexTimer Module 1 / 2**prescaler
void (*FTM1_interruptFunction)(void);

void FTM1_init(uchar prescaler){   
    /* Prescaler */             
    FTM1SC_PS = prescaler;     
    /* Bus clock */            
    FTM1MODE_FTMEN = 1;        
	FTM1SC_CLKSA = 1;              
}
void FTM1_setMod(uint mod) {
	FTM1MOD = mod;
}
uint FTM1_getCount(void) {
	return FTM1CNT;
}
void FTM1_resetCount(void){
	FTM1CNT = 0;
}
void FTM1_enableInterrupts(void (*function)(void)){
    FTM1_interruptFunction = function;
    // Interrupt enable
    FTM1SC_TOIE = 1;
}
__interrupt VectorNumber_Vftm1ovf void FTM1_interrupt(void){
    // Cleans the interrupt flag
    volatile uchar a = FTM1SC;
    FTM1SC_TOF = 0;
    // Calls the function
    FTM1_interruptFunction();
}


// FlexTimer Module 2 / 2 / 2**prescaler
void (*FTM2_interruptFunction)(void);

void FTM2_init(uchar prescaler){            
    /* Prescaler */             
    FTM2SC_PS = prescaler;    
    /* Bus clock /2 */             
    FTM2SC_CLKSA = 1;    
}
void FTM2_setMod(uint mod) {
	FTM2MODL = mod&0xFF;
	FTM2MODH = (mod>>8)&0xFF;
}
uint FTM2_getCount(void) {
	return FTM2CNT;
}
void FTM2_resetCount(void){
	FTM2CNT = 0;
}
void FTM2_enableInterrupts(void (*function)(void)){
    FTM2_interruptFunction = function;
    // Interrupt enable
    FTM2SC_TOIE = 1;
}
__interrupt VectorNumber_Vftm2ovf void FTM2_interrupt(void){
    // Cleans the interrupt flag
    volatile uchar a = FTM2SC;
    FTM2SC_TOF = 0;
    // Calls the function
    FTM2_interruptFunction();
}



// TPM
/**
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
/**/
