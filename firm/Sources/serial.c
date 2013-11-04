#include "serial.h"

void encodeBytesSerie(uchar* buff, uchar depth);
void rxIdleReset(void);		// Reset the sequence if nothing is received in >1s
void receiveByte(uchar);


bool waiting_tkn = True;
bool receivedLastSecond = True;

#define rxQueueSize 64
uchar rxQueue[rxQueueSize];
// Had strange errors using pointers
uchar rxQueueHead = 0;
uchar rxQueueTail = 0;




// Command functions,
// receive the byte number, and a reference to the acknowledge byte
// return True if there are no more bytes to receive
bool fn_ping(uchar data, uchar *buffer, uint byteNum, uint *ack);
bool fn_fps(uchar data, uchar *buffer, uint byteNum, uint *ack);

bool fn_clean(uchar data, uchar *buffer, uint byteNum, uint *ack);
bool fn_store(uchar data, uchar *buffer, uint byteNum, uint *ack);

bool get_height(uchar data, uchar *buffer, uint byteNum, uint *ack);
bool get_width(uchar data, uchar *buffer, uint byteNum, uint *ack);
bool get_depth(uchar data, uchar *buffer, uint byteNum, uint *ack);
bool get_total_width(uchar data, uchar *buffer, uint byteNum, uint *ack);

bool set_height(uchar data, uchar *buffer, uint byteNum, uint *ack);
bool set_width(uchar data, uchar *buffer, uint byteNum, uint *ack);
bool set_depth(uchar data, uchar *buffer, uint byteNum, uint *ack);
bool set_total_width(uchar data, uchar *buffer, uint byteNum, uint *ack);

bool dat_write_column(uchar data, uchar *buffer, uint byteNum, uint *ack);
bool dat_write_section(uchar data, uchar *buffer, uint byteNum, uint *ack);
bool dat_burst(uchar data, uchar *buffer, uint byteNum, uint *ack);
bool dat_interlaced_burst(uchar data, uchar *buffer, uint byteNum, uint *ack);

// An array with pointers to every command function
bool (*commands[])(uchar,uchar*,uint,uint*) = {
    fn_ping,
    fn_fps,
    fn_clean,
    fn_store,
    get_height,
    get_width,
    get_depth,
    get_total_width,
    set_height,
    set_width,
    set_depth,
    set_total_width,
    dat_write_column,
    dat_write_section,
    dat_burst,
    dat_interlaced_burst
};




void serial_init(void){
    SCI_init(SCI_BAUDS,MOD_NONE);
    SCI_enableRxInterrupts(receiveByte);

    // Reset the sequence if nothing is received in 1s
    RTC_init(1);	// 8mS
    RTC_enableInterrupts(rxIdleReset);
}

void serial_update(void){
    static uchar fnNum = 0;
    static uint byteNum;
    static uchar buffer[32];
    uint ack;
    uchar read; 
    
    if(rxQueueTail == rxQueueHead){
    	return;
    }
    // FIFO
    read = rxQueue[rxQueueHead];
    if(++rxQueueHead >= rxQueueSize)
    	rxQueueHead = 0;
	
    // If this is the first byte, translate the command number to
    // its correspondent position in the commands array
    if(waiting_tkn){
        byteNum = 0;
        waiting_tkn = False;
        fnNum = 0;

        printDebug("Inicio de funcion\r\n");

        switch(read){
        	case (DATA | PRECODED | INTERLACED_BURST):
            case (DATA | INTERLACED_BURST): 			fnNum++;
            case (DATA | PRECODED | BURST):
            case (DATA | BURST):                        fnNum++;
            case (DATA | PRECODED | WRITE_SECTION):
            case (DATA | WRITE_SECTION):                fnNum++;
            case (DATA | PRECODED | WRITE_COLUMN):
            case (DATA | WRITE_COLUMN):                 fnNum++;
            case (COMMAND | SET | TOTAL_WIDTH):         fnNum++;
            case (COMMAND | SET | DEPTH):               fnNum++;
            case (COMMAND | SET | WIDTH):               fnNum++;
            case (COMMAND | SET | HEIGHT):              fnNum++;
            case (COMMAND | GET | TOTAL_WIDTH):         fnNum++;
            case (COMMAND | GET | DEPTH):               fnNum++;
            case (COMMAND | GET | WIDTH):               fnNum++;
            case (COMMAND | GET | HEIGHT):              fnNum++;
            case (COMMAND | SET | STORE):               fnNum++;
            case (COMMAND | SET | CLEAN):               fnNum++;
            case (COMMAND | GET | FPS):                 fnNum++;
            case (COMMAND | GET | PING):                break;

            default:
                waiting_tkn = True;
        }
    }
    // Call the function if there's any selected
    // and send the response if it has finished
    if (!waiting_tkn &&
    		(waiting_tkn = commands[fnNum](read,buffer,byteNum++,&ack))){
        SCI_WRITE((uchar)(ack>>8));
        SCI_WRITE((uchar)(ack&0xFF));
    }
}

// Internal function
void receiveByte(uchar data){
	// Receive a byte after an interruption,
	// and enqueue it
    rxQueue[rxQueueTail] = data;
    if(++rxQueueTail >= rxQueueSize)
    	rxQueueTail = 0;
    receivedLastSecond = True;
}
void rxIdleReset(void){
	if(!receivedLastSecond && !waiting_tkn){
		rxQueueHead = rxQueueTail;
		waiting_tkn = True;
	}
    receivedLastSecond = False;
}

// Command functions

bool fn_ping(uchar data, uchar *buffer, uint byteNum, uint *ack){
	printDebug("Ping!\r\n");
    return True;
}
bool fn_fps(uchar data, uchar *buffer, uint byteNum, uint *ack){
	//*ack = FPS_clockFreq/FPS_clockMod_actual;
    return True;
}

bool fn_clean(uchar data, uchar *buffer, uint byteNum, uint *ack){
    uint x,y;
	printDebug("Clean\r\n");
    for(x=MX_MAX_WIDTH;x-->0;){
        for(y=MX_MAX_HEIGHT*3/2/8;y-->0;){
            MX_pixelArray0[x][y] = 0;
            MX_pixelArray1[x][y] = 0;
        }
    }
    *ack = 0xffff;
    return True;
}
bool fn_store(uchar data, uchar *buffer, uint byteNum, uint *ack){
	//todo
	return True;
}

bool get_height(uchar data, uchar *buffer, uint byteNum, uint *ack){
	printDebug("Get height\r\n");
	*ack = MX_height;
    return True;
}
bool get_width(uchar data, uchar *buffer, uint byteNum, uint *ack){
	printDebug("Get width\r\n");
	*ack = MX_width;
    return True;
}
bool get_depth(uchar data, uchar *buffer, uint byteNum, uint *ack){
	printDebug("Get depth\r\n");
	*ack = MX_depth;
    return True;
}
bool get_total_width(uchar data, uchar *buffer, uint byteNum, uint *ack){
	printDebug("Get t width\r\n");
	*ack = MX_totalWidth;
    return True;
}

bool set_height(uchar data, uchar *buffer, uint byteNum, uint *ack){
	printDebug("Set height\r\n");
    switch(byteNum){
        case 0:
            return False;
        case 1:
            buffer[0] = data;
            return False;
        default:
            MX_height = (uint)(buffer[0])<<8 | data;
            *ack = 0xffff;
            return True;
    }
}
bool set_width(uchar data, uchar *buffer, uint byteNum, uint *ack){
	printDebug("Set width\r\n");
    switch(byteNum){
        case 0:
            return False;
        case 1:
            buffer[0] = data;
            return False;
        default:
            MX_width = (uint)(buffer[0])<<8 | data;
            *ack = 0xffff;
            return True;
    }
}
bool set_depth(uchar data, uchar *buffer, uint byteNum, uint *ack){
	printDebug("Set depth\r\n");
    if (byteNum == 0)
        return False;
    *ack = 0xffff;
    return True;
}
bool set_total_width(uchar data, uchar *buffer, uint byteNum, uint *ack){
	printDebug("Set t width\r\n");
    switch(byteNum){
        case 0:
            return False;
        case 1:
            buffer[0] = data;
            return False;
        default:
            MX_totalWidth = (uint)(buffer[0])<<8 & data;
            *ack = 0xffff;
            return True;
    }
}

bool dat_write_column(uchar data, uchar *buffer, uint byteNum, uint *ack){
    // buffer 0 = precoded flag
    // buffer 1 = X coordinate

    static uint totalBytes = 0;
    uint pxByteNum = (byteNum-2)>>1; // Don't count bytes for the other matrix

	printDebug("Write column\r\n");

    switch(byteNum){
        case 0:
            // Get the PRECODED flag
            buffer[0] = data & PRECODED;
            return False;
        case 1:
            // Get the X coordinate
            buffer[1] = data;

            // Calculate the total number of bytes to receive,
            // and save it in a static var
            if(buffer[0]){
                // Encoded, $bit_depth bits per color
                // Height * colors * bit depth / bits per byte + token
                totalBytes = MX_height *3 *MX_depth /8 +2;
            }
            else {
                // Non-encoded, 1 byte per color
                // Height * colors + token
                totalBytes = MX_height *3 +2;
            }

            return False;
        default:
            // Ignore the data meant for the other side
            if (byteNum%2 != !ID)
                return False;

            // Processing order differs between encoded and non encoded tx
            if(buffer[0]){
                // If encoded buffer the input and process 3 bytes at a time
                // of each depth-bit
                if(MX_depth == 1){
                    // 1 bit-depth, buffer 3 bytes
                    buffer[2+pxByteNum%3] = data;
                    if(pxByteNum%3 < 2)
                        return False;

                    // Save the data
                    MX_pixelArray0[buffer[1]][pxByteNum-2] = buffer[2];
                    MX_pixelArray0[buffer[1]][pxByteNum-1] = buffer[3];
                    MX_pixelArray0[buffer[1]][pxByteNum-0] = buffer[4];
                }
                else{
                    // 2 bit-depth, buffer 6 bytes
                    buffer[2+pxByteNum%6] = data;
                    if(pxByteNum%6 < 5)
                        return False;

                    // Save the data
                    MX_pixelArray0[buffer[1]][pxByteNum-2] = buffer[2];
                    MX_pixelArray0[buffer[1]][pxByteNum-1] = buffer[3];
                    MX_pixelArray0[buffer[1]][pxByteNum-0] = buffer[4];
                    MX_pixelArray1[buffer[1]][pxByteNum-2] = buffer[5];
                    MX_pixelArray1[buffer[1]][pxByteNum-1] = buffer[6];
                    MX_pixelArray1[buffer[1]][pxByteNum-0] = buffer[7];
                }
            }
            else{
                // If not encoded, buffer 24 bytes (R,G,B 8b per color)
                // process both color depth arrays at the same time
                buffer[2+pxByteNum %24] = data;
                if(pxByteNum %24 < 23)
                    return False;

                //
                encodeBytesSerie(buffer+2,MX_depth);

                // Write  array both arrays at the same time
                MX_pixelArray0[buffer[1]][pxByteNum/24-2] = buffer[2];
                MX_pixelArray0[buffer[1]][pxByteNum/24-1] = buffer[3];
                MX_pixelArray0[buffer[1]][pxByteNum/24-0] = buffer[4];
                if(MX_depth != 1){
                    MX_pixelArray1[buffer[1]][pxByteNum/24-2] = buffer[5];
                    MX_pixelArray1[buffer[1]][pxByteNum/24-1] = buffer[6];
                    MX_pixelArray1[buffer[1]][pxByteNum/24-0] = buffer[7];
                }
            }
    }

    // Check if it has finished
    if(byteNum >= totalBytes-1){
    	*ack = 0xffff;
        return True;
    }
    return False;
}
bool dat_write_section(uchar data, uchar *buffer, uint byteNum, uint *ack){
    // buffer 0 = precoded flag
    // buffer 1 = X coordinate
    // buffer 2 = X length

    static uint totalBytes = 0;
    uint pxByteNum = (byteNum-3)>>1; // Don't count bytes for the other matrix

	printDebug("Write section\r\n");

    switch(byteNum){
        case 0:
            // Get the PRECODED flag
            buffer[0] = data & PRECODED;
            return False;
        case 1:
            // Get the X coordinate
            buffer[1] = data;
            return False;
        case 2:
            // Get the X length
            buffer[2] = data;

            // Calculate the total number of bytes to receive,
            // and save it in a static var
            if(buffer[0]){
                // Encoded, $bit_depth bits per color
                // Height * columns * colors * bit depth / bits per byte + token
                totalBytes = MX_height *buffer[2] *3 *MX_depth /8 +3;
            }
            else {
                // Non-encoded, 1 byte per color
                // Height * columns * colors + token
                totalBytes = MX_height *buffer[2] *3 +3;
            }

            return False;
        default:
            // Ignore the data meant for the other side
            if (!(byteNum%2) != !ID)
                return False;

            // Processing order differs between encoded and non encoded tx
            if(buffer[0]){
                // If encoded buffer the input and process 3 bytes at a time
                // of each depth-bit
                if(MX_depth == 1){
                    // 1 bit-depth, buffer 3 bytes
                    buffer[3+pxByteNum%3] = data;
                    if(pxByteNum%3 < 2)
                        return False;

                    // Save the data
                    // Ignore the column-array boundaries,
                    // so we don't have to do any costly calculations...
                    MX_pixelArray0[buffer[1]][pxByteNum-2] = buffer[3];
                    MX_pixelArray0[buffer[1]][pxByteNum-1] = buffer[4];
                    MX_pixelArray0[buffer[1]][pxByteNum-0] = buffer[5];
                }
                else{
                    // 2 bit-depth, buffer 6 bytes
                    buffer[3+pxByteNum%6] = data;
                    if(pxByteNum%6 < 5)
                        return False;

                    // Save the data
                    // Ignore the column-array boundaries,
                    // so we don't have to do any costly calculations...
                    MX_pixelArray0[buffer[1]][pxByteNum-2] = buffer[3];
                    MX_pixelArray0[buffer[1]][pxByteNum-1] = buffer[4];
                    MX_pixelArray0[buffer[1]][pxByteNum-0] = buffer[5];
                    MX_pixelArray1[buffer[1]][pxByteNum-2] = buffer[6];
                    MX_pixelArray1[buffer[1]][pxByteNum-1] = buffer[7];
                    MX_pixelArray1[buffer[1]][pxByteNum-0] = buffer[8];
                }
            }
            else{
                // If not encoded, buffer 24 bytes (R,G,B 8b per color)
                // process both color depth arrays at the same time
                buffer[3+pxByteNum %24] = data;
                if(pxByteNum %24 < 23)
                    return False;

                //
                encodeBytesSerie(buffer+3,MX_depth);

                // Write  array both arrays at the same time
                // Ignore the column-array boundaries,
                // so we don't have to do any costly calculations...
                MX_pixelArray0[buffer[1]][pxByteNum/24-2] = buffer[3];
                MX_pixelArray0[buffer[1]][pxByteNum/24-1] = buffer[4];
                MX_pixelArray0[buffer[1]][pxByteNum/24-0] = buffer[5];
                if(MX_depth != 1){
                    MX_pixelArray1[buffer[1]][pxByteNum/24-2] = buffer[6];
                    MX_pixelArray1[buffer[1]][pxByteNum/24-1] = buffer[7];
                    MX_pixelArray1[buffer[1]][pxByteNum/24-0] = buffer[8];
                }
            }
    }

    // Check if it has finished
    if(byteNum >= totalBytes-1){
    	*ack = 0xffff;
        return True;
    }
    return False;
}
bool dat_burst(uchar data, uchar *buffer, uint byteNum, uint *ack){
    // buffer 0 = precoded flag

    static uint totalBytes = 0;
    uint pxByteNum = (byteNum-1)>>1; // Don't count bytes for the other matrix
    // For optimisation purposes
    static uchar colBytes;

	printDebug("Burst\r\n");

    switch(byteNum){
        case 0:
            // Get the PRECODED flag
            buffer[0] = data & PRECODED;

            // Calculate the total number of bytes to receive,
            // and save it in a static var
            if(buffer[0]){
                // Encoded, $bit_depth bits per color
                // Height * columns * colors * bit depth / bits per byte + token
                totalBytes = MX_height *MX_width *3 *MX_depth /8 +1;
            }
            else {
                // Non-encoded, 1 byte per color
                // Height * columns * colors + token
                totalBytes = MX_height *MX_width *3 +1;
            }
            // For optimisation purposes
            colBytes = MX_height >= 64 ? 4*3 : 2*3;
            
            return False;
        default:
            // Ignore the data meant for the other side
            if (!(byteNum%2) != !ID)
                return False;

            // Processing order differs between encoded and non encoded tx
            if(buffer[0]){
                // If encoded buffer the input and process 3 bytes at a time
                // of each depth-bit
                if(MX_depth == 1){
                    // 1 bit-depth, buffer 3 bytes
                    buffer[1+pxByteNum%3] = data;
                    
                    if(pxByteNum%3 < 2)
                        return False;
                    

                    // Save the data
                    // Calculate the divider at the beginning of the reception
                    MX_pixelArray0[pxByteNum/colBytes][pxByteNum%colBytes-2] = buffer[1];
                    MX_pixelArray0[pxByteNum/colBytes][pxByteNum%colBytes-1] = buffer[2];
                    MX_pixelArray0[pxByteNum/colBytes][pxByteNum%colBytes-0] = buffer[3];
                }
                else{
                    // 2 bit-depth, buffer 6 bytes
                    buffer[1+pxByteNum%6] = data;
                    if(pxByteNum%6 < 5)
                        return False;

                    // Save the data
                    // Calculate the divider at the beginning of the reception
                    MX_pixelArray0[pxByteNum/colBytes][pxByteNum%colBytes-2] = buffer[1];
                    MX_pixelArray0[pxByteNum/colBytes][pxByteNum%colBytes-1] = buffer[2];
                    MX_pixelArray0[pxByteNum/colBytes][pxByteNum%colBytes-0] = buffer[3];
                    MX_pixelArray1[pxByteNum/colBytes][pxByteNum%colBytes-2] = buffer[4];
                    MX_pixelArray1[pxByteNum/colBytes][pxByteNum%colBytes-1] = buffer[5];
                    MX_pixelArray1[pxByteNum/colBytes][pxByteNum%colBytes-0] = buffer[6];
                }
            }
            else{
                // If not encoded, buffer 24 bytes (R,G,B 8b per color)
                // process both color depth arrays at the same time
                buffer[1+pxByteNum %24] = data;
                if(pxByteNum %24 < 23)
                    return False;

                //
                encodeBytesSerie(buffer+1,MX_depth);

                // Write  array both arrays at the same time
                // Ignore the column-array boundaries,
                // so we don't have to do any costly calculations...
                MX_pixelArray0[0][pxByteNum/24-2] = buffer[1];
                MX_pixelArray0[0][pxByteNum/24-1] = buffer[2];
                MX_pixelArray0[0][pxByteNum/24-0] = buffer[3];
                if(MX_depth != 1){
                    MX_pixelArray1[0][pxByteNum/24-2] = buffer[4];
                    MX_pixelArray1[0][pxByteNum/24-1] = buffer[5];
                    MX_pixelArray1[0][pxByteNum/24-0] = buffer[6];
                }
            }
    }

    // Check if it has finished
    if(byteNum >= totalBytes-1){
        *ack = 0xffff;
        return True;
    }
    return False;
}
bool dat_interlaced_burst(uchar data, uchar *buffer, uint byteNum, uint *ack){
	// todo
	return True;
}


void encodeBytesSerie(uchar* buff, uchar depth){
    // Encode 24 bytes of a RGB sequence (8b per color),
    // preparing them to be sent over SPI
    register uint tempByte0;
    register uint tempByte1;
    uchar i;

    tempByte0 = 0;
    tempByte1 = 0;

    if(depth == 1){
        for(i=0;i<24;i++){
            tempByte0 <<= 1;
            tempByte0 |= buff[i] & 0x80;
            if((i+1) %8 == 0){
                tempByte0 >>= 7;
                tempByte0 &= 0xFF;
                buff[i/8] = (uchar)tempByte0;
                tempByte0 = 0;
            }
        }
    }
    else{
        for(i=0;i<24;i++){

            tempByte0 <<= 1;
            tempByte1 <<= 1;

            tempByte0 |= buff[i] & 0x80;
            tempByte1 |= buff[i] & 0x40;

            if((i+1) %8 == 0){
                tempByte0 >>= 7;
                tempByte0 &= 0xFF;

                tempByte1 >>= 6;
                tempByte1 &= 0xFF;

                buff[i/4-1] = (uchar)tempByte0;
                buff[i/4] = (uchar)tempByte1;

                tempByte0 = 0;
                tempByte1 = 0;
            }
        }
    }
}
