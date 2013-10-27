// Globals

#include "constants.h"

#ifndef GLOBAL_H_
#define GLOBAL_H_

// The arrays where the pixel data is stored,
// array0 stores the MSB
// array1 stores the next bit
extern uchar MX_pixelArray0[MX_MAX_WIDTH][MX_MAX_HEIGHT*3/2/8];
extern uchar MX_pixelArray1[MX_MAX_WIDTH][MX_MAX_HEIGHT*3/2/8];

// Leds current status
extern uint LED_column;
extern uchar LED_subcolumn;
extern uchar LED_columnByte;

// Matrix status
extern uint MX_height;
extern uint MX_width;
extern uchar MX_depth;           // Color-depth, in bits
extern uint MX_totalWidth;      // Number of pixels
                                // the complete perimeter would occupy

// Rotational speed variables
extern uint FPS_clockMod_real;
extern uint FPS_clockMod_actual;

// Id
extern bool ID;

#endif
