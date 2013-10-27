/*      Constants and defines   */

#ifndef CONSTANTS_H_GUARD
#define CONSTANTS_H_GUARD

/*      Values          */
#define BUSCLK 8000000     /* Frecuencia de bus        */
#define VOLTAGE 3.3         /* Voltaje de alimentacion  */

#define SCI_BAUDS 115200
#define SPI_BAUDS 115200

/*      Matrix params   */
#define MX_MAX_WIDTH 480
#define MX_MAX_HEIGHT 64

/*      Encoder         */
#define FPS_PRESCALER 7
#define FPS_HYSTERESIS 1

/*      Leds            */
#define LED_PRESCALER 0
#define LED_OUT_EN PTED_PTED5
#define LED_OUT_CLK PTED_PTED4
#define LED_OUT_EN_PORT PTEDD_PTEDD5
#define LED_OUT_CLK_PORT PTEDD_PTEDD4

/*      Id            	*/
#define ID_PIN PTDD_PTDD2
#define ID_PIN_PULL PTDPE_PTDPE2

/*      Types           */
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef enum {False = 0, True = 1} bool;
#define NONE 0
#define EVEN 0
#define ODD 1


/**
 *
 *  Communication token
 *
 *      ...x .... Data 1 - CMD 0
 *
 *      Cmd:
 *      ...0 x... Set 1 - Get 0
 *      ...0 .xxx Opcode
 *
 *      Data:
 *      .... x... Precoded
 *      ...1 .xxx Opcode
 *
/**/

/*      Command         */
#define COMMAND 0

#define SET 8
#define GET 0

/*      Only get        */
#define PING 0
#define FPS 1

/*      Only set        */
#define CLEAN 0
#define STORE 1

/*      Get and set     */
#define HEIGHT 4
#define WIDTH 5
#define DEPTH 6
#define TOTAL_WIDTH 7

/*      Data            */
#define DATA 0x10

#define PRECODED 8

/*      Opcodes         */
#define WRITE_COLUMN 0
#define WRITE_SECTION 1
#define BURST 2
#define INTERLACED_BURST 3

#endif // CONSTANTS_H_GUARD
