/*
 * Some modules that didn't deserve a separated file... 
 */

#ifndef OTHER_H_
#define OTHER_H_

#include "constants.h"
#include "global.h"
#include "snippets.h"
#include "modules.h"

// Get the id (odd/even)
void id_init(void);

// Synchronise both uCs at the beginning of the image
// | KBI1P4 , PTG4 |
void sync_init(void);
void sync_send(void);

// Battery measurement
// | AD1P19, PTG4 |

#endif /* OTHER_H_ */
