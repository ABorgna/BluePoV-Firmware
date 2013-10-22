#ifndef MODULES_MACROS_H
#define MODULES_MACROS_H

// Registers and values

// SCI
#define SCI_MAX 1           /* Cantidad de modulos      */

// SPI
#define SPI_MAX 1           /* Cantidad de modulos      */

// TPM
#define TPM_MAX 2           /* Cantidad de modulos                  */
#define TPMxSC_off 0x40     /* Offset entre TPM1SC y TPM2SC         */
#define TPMxCxSC_off 0x03   /* Offset entre TPMxC0SC y TPMxC1SC     */
#define TPM_BUSDIV 128      /* Preescaler de frecuencia (2**k)      */

// ADC
#define ADCVREF VOLTAGE     /* Voltaje de referencia de conversion  */
#define ADC_PORT_CH_MAX 27  /* Cantidad de canales a pines          */
#define APCTLx_off 0x1      /* Offset entre APCTL1 y APCTL2         */

// GPIO
#define PTxDD_off 1         /* Offset entre PTxD y PTxDD Data Direction */
#define PTxPE_off 0x1840    /* Offset entre PTxD y PTxPE Pull-up Enable */
#define PTxSE_off 0x1841    /* Offset entre PTxD y PTxSC Slew Rate      */
#define PTxDS_off 0x1842    /* Offset entre PTxD y PTxPS Drive Strength */
#define PTxFE_off 0x1843    /* Offset entre PTxD y PTxES Input Filter   */

/*               Macros, ANSI C doesn't allow inlines :/            */

// Clock and COP wd
#define CPU_init() {                            \
    /* Disable the mighty COP watchdog */       \
    SOPT1_COPT = 0;                             \
    /* Something something with the bus clock */\
    MCGTRM = *((uchar*)0xFFAF);                 \
    MCGSC_FTRIM = *((uchar*)0xFFAE);            \
}
#define CPU_extCLK(){                                                                   \
    /* Assume 12MHz external clock source connected. */                                 \
                                                                                        \
    /* In order to use the USB we need to enter PEE mode and MCGOUT set to 48 MHz.      \
     Out of reset MCG is in FEI mode. */                                                \
                                                                                        \
    /**** Moving from FEI (FLL engaged internal) to PEE (PLL engaged external) mode. */ \
    /* switch from FEI to FBE (FLL bypassed external) */                                \
    /* enable external clock source */                                                  \
    MCGC2 = MCGC2_HGO_MASK      /* oscillator in high gain mode */                      \
          | MCGC2_EREFS_MASK    /* because crystal is being used */                     \
          | MCGC2_RANGE_MASK    /* 12 MHz is in high freq range */                      \
          | MCGC2_ERCLKEN_MASK; /* activate external reference clock */                 \
    while (MCGSC_OSCINIT == 0);                                                         \
                                                                                        \
    /* select clock mode */                                                             \
    MCGC1 = (2<<6)          /* CLKS = 10 -> external reference clock. */                \
          | (3<<3);         /* RDIV = 3 -> 12MHz/8=1.5 MHz */                           \
                                                                                        \
    /* wait for mode change to be done */                                               \
    while (MCGSC_IREFST != 0);                                                          \
    while (MCGSC_CLKST != 2);                                                           \
                                                                                        \
    /* switch from FBE to PBE (PLL bypassed internal) mode */                           \
    MCGC3 = MCGC3_PLLS_MASK                                                             \
          | (8<<0);         /* VDIV=6 -> multiply by 32 -> 1.5MHz * 32 = 48MHz */       \
    while(MCGSC_PLLST != 1);                                                            \
    while(MCGSC_LOCK != 1);                                                             \
                                                                                        \
    /* finally switch from PBE to PEE (PLL enabled external mode) */                    \
    MCGC1 = (0<<6)          /* CLKS = 0 -> PLL or FLL output clock. */                  \
          | (3<<3);         /* RDIV = 3 -> 12MHz/8=1.5 MHz */                           \
    while(MCGSC_CLKST!=3);                                                              \
                                                                                        \
    /* Now MCGOUT=48MHz, BUS_CLOCK=24MHz */                                             \
}



// Serial Comunication Interface
#define prescaler ((BUSCLK/16)/bauds)&0xFFFF
#define SCI_init(bauds,flags){                                      \
    /* Set baud rate    */                                          \
    SCI1BDL =  prescaler & 0xFF;                                    \
    SCI1BDH = (prescaler >> 8) & 0x1F;                              \
    /* Configure bits & parity  */                                  \
    SCI1C1_M = flags & SCI_NINEBITS ? 1 : 0;     /* 8+1b     */     \
                                                /* Parity enable*/  \
    SCI1C1_PE = flags & (SCI_ODDPARITY|SCI_EVENPARITY) ? 1 : 0;     \
    SCI1C1_PT = flags & SCI_ODDPARITY ? 1 : 0;   /* Parity   */     \
    /* Enable Tx & Rx   */                                          \
    SCI1C2_TE = 1;                                                  \
    SCI1C2_RE = 1;                                                  \
}
#undef prescaler
#define SCI_stop() {            \
    SCI_disableinterrupts();    \
    SCIC2_TE = 0;               \
    SCIC2_RE = 0;               \
}
#define SCI_disableinterrupts() {   \
    SCIC2_TCIE = 0;                 \
    SCIC2_RIE = 0;                  \
}
#define SCI_rxPoll SCI1S1_RDRF /* True if buffer full */



// Real Time Counter
#define RTC_init(flags){                            \
    /* 1kHz oscilator */                            \
    RTCSC_RTCLKS = 0;                               \
    /* Preescaler, 1 or 1k */                       \
    RTCSC_RTCPS =   flags & RTC_MSECOND ? 8 :       \
                    flags & RTC_10MSECOND ? 11 :    \
                    flags & RTC_100MSECOND ? 13 :   \
                    15;  /* Second*/                \
}




// Serial Peripheral Interface
#define SPI__scaler (BUSCLK/(bauds))&0xFFFF
#define SPI_INIT(bauds,flags) {                             \
    int prescaler, scaler;                                  \
                                                            \
    /* Set as master / slave                        */      \
    SPIC1_MSTR = flags & SPI_SLAVE ? 0 : 1;                 \
    /* Transmit less significant bit first          */      \
    SPIC1_LSBFE = flags & SPI_LSBFIRST ? 1 : 0;             \
    /* Invert the clock polarity, 1 = active low    */      \
    SPIC1_CPOL = flags & SPI_INVERTCLK ? 1 : 0;             \
                                                            \
    /* Set baud rate                                */      \
    /* Min rate: BUSCLK / 2048                      */      \
    /* Max rate: BUSCLK / 2                         */      \
    SPIBR = SPI__scaler > 256 ?                             \
                (((SPI__scaler / 256) - 1) << 4) | 7 :      \
                LOG2_i(SPI__scaler)- 1;                     \
                                                            \
    /* Enable the module                            */      \
    SPIC1_SPE = 1;                                          \
}
#undef SPT__scaler




// Timer Pulse-Width Modulator
#define TPM_INIT(module) {                                          \
    uchar offset = TPMxSC_off * (module-1);                         \
    /* Center-aligned, clock source = BUSCLK/TPM_BUSDIV*/           \
    *(&TPM1SC+offset) = 0x28 | (LOG2_i(TPM_BUSDIV)&0x7);            \
}






// Analog-Digital Converter
#define ADC_INIT(flags){                                            \
    ADC_DISABLEINTERRUPTS();                                        \
    ADCSC1_ADCO = 0;                                                \
    ADCSC2 = 0;                                                     \
                                                                    \
    ADCCFG_ADLPC = flags & ADC_LOWPOW ? 1 : 0;                      \
    ADCCFG_ADIV = flags & ADC_LOWPOW ? 3 : 0;                       \
    ADCCFG_ADLSMP = flags & ADC_LOWPOW ? 1 : 0;                     \
    ADCCFG_MODE = flags & ADC_10B ? 2 : 0;                          \
}
#define ADC_SETCHANNEL(channel){                                    \
    if (channel < ADC_PORT_CH_MAX){                                 \
        *(&APCTL1+APCTLx_off*(ADCSC1_ADCH/8)) = 0;                  \
        *(&APCTL1+APCTLx_off*(channel/8)) = (1<<channel)&0xff;      \
    }                                                               \
    ADCSC1_ADCH = channel;                                          \
}
#define ADC_DISABLEINTERRUPTS() {ADCSC1_AIEN = 0;}
#define ADC_CONV(){    /* Blocking      */                          \
    ADCSC1_COCO = 0;                                                \
    while (ADCSC2_ADACT);                                           \
}
#define ADC_CONVNB(){  /* Non-blocking  */                          \
    ADCSC1_COCO = 0;                                                \
}

// General Purpose I/O
#define GPIO_OUT(port,offset) {SNP_maskWrite(port+PTxDD_off,0xff,1<<offset);}
#define GPIO_PULLUP(port,offset) {SNP_maskWrite(port+PTxPE_off,0xff,1<<offset);}

// FlexTimer Module 1, busclk / 2**prescaler
#define FTM2_init(prescaler){   \
    /* Bus clock */             \
    FTM2SC_CPWMS = 1;           \
    FTM2MODE_FTMEN = 1;         \
    /* Prescaler */             \
    FTM2SC_PS = prescaler;      \
}
#define FTM1_setMod(mod) {FTM1MOD = mod;}
#define FTM1_count FTMCNT

// FlexTimer Module, busclk / 2 / 2**prescaler
#define FTM2_init(prescaler){   \
    /* Bus clock /2 */          \
    FTM2SC_CPWMS = 1;           \
    /* Prescaler */             \
    FTM2SC_PS = prescaler;      \
}
#define FTM2_setMod(mod) {FTM1MOD = mod;}
#define FTM2_count FTMCNT


#endif
