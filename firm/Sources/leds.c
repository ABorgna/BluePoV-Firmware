#include "leds.h"

// Internal declatations
void led_col_interrupt(void);
void led_tx_interrupt(void);

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

    FTM1_setMod(FPS_clockMod_actual);
    FTM1_init(LED_PRESCALER);
    
    led_enable();

    FTM1_enableInterrupts(led_col_interrupt);
}
void led_disable(void){
	LED_enabled = 0;
    //FTM1_disableInterrupts();
	SPI_disableTxInterrupts();
}

// Internal definitions
void led_col_interrupt(void){

#ifdef ENCODER_POLLING
	fps_poll();
#endif
	
	if(!LED_enabled)
		return;
	
	// Show the data sent
	LED_OUT_CLK = 1;
	SPI_disableTxInterrupts();
    
    // Update the position counters
    LED_columnByte = 0;
	if(MX_depth==1 || !LED_subcolumn--){
		LED_column++;
		LED_subcolumn = 2;
	}
	
	// If we completed the matrix go idle
	if(LED_column+MX_offset >= MX_width){
		led_enable();	// <---------------------------- debug -| todo |-
		LED_column++;
		LED_subcolumn--;
	}//led_disable();

	// Adjust to the motor speed
    FTM1_setMod(FPS_clockMod_actual);
    FTM1_resetCount();
    
    // Calculate the number of bytes per column
    // Height * colors/ bits per byte/ number of uCs
    LED_bytesPerCol = (MX_height/8/2)*3;

    // Since the tx buffer is empty, it'll start sending right now
    SPI_enableTxInterrupts(led_tx_interrupt);
    
    LED_OUT_CLK = 0;
}

void led_tx_interrupt(void){
	
	if(MX_depth==1 || !(LED_subcolumn&0x1)){
		SPI_WRITE(~MX_pixelArray0[MX_width-LED_column-MX_offset][LED_columnByte]);
	}
	else{
		SPI_WRITE(~MX_pixelArray1[MX_width-LED_column-MX_offset][LED_columnByte]);
	}


#ifdef ENCODER_POLLING
	fps_poll();
#endif

	if(++LED_columnByte >= LED_bytesPerCol){
		LED_columnByte = 0;
		SPI_disableTxInterrupts();
	}
}
