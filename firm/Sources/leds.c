#include "leds.h"

// Internal declatations
void led_col_interrupt(void);
void led_tx_interrupt(void);

uint LED_bytesPerCol;

// External definitions
void led_init(void){
	// Serial output
	SPI_init(SPI_BAUDS,SPI_MASTER|SPI_INVERTCLK);
    
    // Negated
    LED_OUT_EN = 0;
    LED_OUT_EN_PORT = 1;
    // Rising
    LED_OUT_CLK = 0;
    LED_OUT_CLK_PORT = 1;
    
    FTM1_init(LED_PRESCALER);
    FTM1_setMod(FPS_clockMod_actual);

    FTM1_enableInterrupts(led_col_interrupt);
}

// Internal definitions
void led_col_interrupt(void){
	// Show the data sent
	LED_OUT_CLK = 1;
    
    // Update the position counters
    LED_columnByte = 0;
	if(MX_depth==1 || LED_subcolumn++>=2){
		LED_column++;
		LED_subcolumn = 0;
	}
	
	// Adjust to the motor speed
    FTM1_setMod(FPS_clockMod_actual);
    
    // Calculate the number of bytes per column
    // Height * colors/ bits per byte/ number of uCs
    LED_bytesPerCol = MX_height *3/8 /2;

    // Since the tx buffer is empty, it'll start sending right now
    SPI_enableTxInterrupts(led_tx_interrupt);
    
    LED_OUT_CLK = 0;
}

void led_tx_interrupt(void){
	
	if(MX_depth==1){
		SPI_write(MX_pixelArray0[LED_column][LED_columnByte]);
	}
	else{
		if(LED_subcolumn&0x1){
			SPI_write(MX_pixelArray1[LED_column][LED_columnByte]);
		}
		else{
			SPI_write(MX_pixelArray0[LED_column][LED_columnByte]);
		}
	}

	if(++LED_columnByte >= LED_bytesPerCol){
		SPI_disableTxInterrupts();
	}
}
