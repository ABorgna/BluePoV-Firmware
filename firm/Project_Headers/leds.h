#ifndef LEDS_H_
#define LEDS_H_

#include "constants.h"
#include "global.h"
#include "snippets.h"
#include "modules.h"
// Fixing the fixes
#include "other.h"

void led_init(void);
void led_disable(void);
void led_enable(void);
void led_reset(void);

#endif /* LEDS_H_ */
