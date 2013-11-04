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

// Flags

typedef enum {
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
    RTC_SECOND = 7,
    /*
    RTC_100MSECOND = 0x2,
    RTC_10MSECOND = 0x4,
    RTC_MSECOND = 0x8,
    */
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

#endif
