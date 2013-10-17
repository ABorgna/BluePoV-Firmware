/*      Constants and defines   */

/*      Values          */
#define BUSCLK 48000000     /* Frecuencia de bus        */
#define VOLTAGE 3.3         /* Voltaje de alimentacion  */

#define SERIAL_BAUDS 115200

/*      Matrix params   */
#define MX_MAX_WIDTH 480
#define MX_MAX_HEIGHT 64

/*      Types           */
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned char bool;
#define False 0
#define True !False
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
#define DATA = 0

#define PRECODED 8

/*      Opcodes         */
#define WRITE_COLUMN 0
#define WRITE_SECTION 1
#define BURST 2
#define INTERLACED_BURST 3
