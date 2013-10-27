#include "global.h"

// The arrays where the pixel data is stored,
// array0 stores the MSB
// array1 stores the next bit
uchar MX_pixelArray0[MX_MAX_WIDTH][MX_MAX_HEIGHT*3/2/8];
uchar MX_pixelArray1[MX_MAX_WIDTH][MX_MAX_HEIGHT*3/2/8];

// Leds current status
uint LED_column;
uchar LED_subcolumn;
uchar LED_columnByte;

// Matrix status
uint MX_height = MX_MAX_HEIGHT;
uint MX_width = MX_MAX_WIDTH;
uchar MX_depth = 1;                  // Color-depth, in bits
uint MX_totalWidth = MX_MAX_WIDTH;  // Number of pixels
                                    // the complete perimeter would occupy

// Rotational speed variables
uint FPS_clockMod_real = 100;
uint FPS_clockMod_hyster = 100;

// Id
bool ID;
