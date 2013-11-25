/*
 * Some modules that didn't deserve a separated file... 
 */

#include "other.h"

void id_init(void){
	volatile uchar a = 8;
	ID_PIN_PULL = 1;
	while(--a){};
	ID = ID_PIN;
	ID_PIN_PULL = 0;
}

// Synchronise both uCs at the beginning of the image
// | KBI1P4 , PTG4 |
void sync_inter(void);

void sync_init(void){
	if(ID){
		SYNC_PIN = 1;
		SYNC_PIN_PORT = 1;
	}
	else{
		KBI_init(4);
		KBI_enableInterrupts(sync_inter);
	}
}
void sync_send(void){
	SYNC_PIN = 0;
	asm{
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
	}
	SYNC_PIN = 1;
	asm{
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
	}
	SYNC_PIN = 0;
	asm{
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
	}
	SYNC_PIN = 1;
}
void sync_inter(void){
	volatile uchar a = 8;
	while(SYNC_PIN == 0){
		if(!--a)
			return;
	};
	a = 8;
	while(SYNC_PIN == 1){
		if(!--a)
			return;
	};
	a = 8;
	while(SYNC_PIN == 0){
		if(!--a)
			return;
	};
	LED_dimm_state = 0;
	LED_column = MX_width/2;
	LED_subcolumn = 0;
	LED_columnByte = 0;
	LED_enabled = 1;
	FTM1_resetCount();
}
