/*
 * Some modules that didn't deserve a separated file... 
 */

#include "other.h"

void id_init(void){
	ID_PIN_PULL = 1;
	ID = ID_PIN;
	ID_PIN_PULL = 0;
}
