// Serial coms,
// polling mode

#ifndef SERIAL_H_
#define SERIAL_H_

#include "constants.h"
#include "global.h"
#include "snippets.h"
#include "modules.h"

#include "leds.h"

void serial_init(void);
void serial_update(void);

#endif /* SERIAL_H_ */
