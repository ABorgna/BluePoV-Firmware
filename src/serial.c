#include "serial.h"

void serial_update(void);
void encodeBytes(uchar* buff, uchar depth);

// Command functions,
// receive the byte number, and a reference to the acknowledge byte
// return True if there are no more bytes to receive
bool fn_ping(uchar data, uchar *buffer, uint byteNum, uint &ack);
bool fn_fps(uchar data, uchar *buffer, uint byteNum, uint &ack);

bool fn_clean(uchar data, uchar *buffer, uint byteNum, uint &ack);
bool fn_store(uchar data, uchar *buffer, uint byteNum, uint &ack);

bool get_height(uchar data, uchar *buffer, uint byteNum, uint &ack);
bool get_width(uchar data, uchar *buffer, uint byteNum, uint &ack);
bool get_depth(uchar data, uchar *buffer, uint byteNum, uint &ack);
bool get_total_width(uchar data, uchar *buffer, uint byteNum, uint &ack);

bool set_height(uchar data, uchar *buffer, uint byteNum, uint &ack);
bool set_width(uchar data, uchar *buffer, uint byteNum, uint &ack);
bool set_depth(uchar data, uchar *buffer, uint byteNum, uint &ack);
bool set_total_width(uchar data, uchar *buffer, uint byteNum, uint &ack);

bool dat_write_column(uchar data, uchar *buffer, uint byteNum, uint &ack);
bool dat_write_column(uchar data, uchar *buffer, uint byteNum, uint &ack);
bool dat_burst(uchar data, uchar *buffer, uint byteNum, uint ack);
bool dat_interlaced_burst(uchar data, uchar *buffer, uint byteNum, uint &ack);

// An array with pointers to every command function
bool (*commands)(uchar,uchar,uint,uint&) = {
    fn_ping,
    fn_fps,
    fn_clean,
    fn_store,
    get_height,
    get_width,
    get_depth,
    get_totalWidth,
    set_height,
    set_width,
    set_depth,
    set_totalWidth,
    dat_write_column,
    dat_write_column,
    dat_burst,
    dat_interlaced_burst
}




void serial_init(void){
    SCI_init(SERIAL_BAUDS,NONE);
}

void serial_update(void){
    static bool waiting_tkn = True;
    static uchar fnNum = 0;
    static uint byteNum;
    static uchar buffer[32];
    uint ack;

    // Check if there is something in the reception buffer,
    // and return if not
    if(!SCI_rxPoll)
        return;

    // If this is the first byte, translate the command number to
    // its correspondent position in the commands array
    if(waiting_tkn){
        byteNum = 0;
        waiting_tkn = False;
        fnNum = 0;

        switch(SCI_read){
            case DATA & INTERLACED_BURST:               fnNum++;
            case DATA & BURST:                          fnNum++;
            case DATA & WRITE_SECTION:                  fnNum++;
            case DATA & WRITE_COLUMN:                   fnNum++;
            case COMMAND & SET & TOTAL_WIDTH:           fnNum++;
            case COMMAND & SET & DEPTH:                 fnNum++;
            case COMMAND & SET & WIDTH:                 fnNum++;
            case COMMAND & SET & HEIGHT:                fnNum++;
            case COMMAND & GET & TOTAL_WIDTH:           fnNum++;
            case COMMAND & GET & DEPTH:                 fnNum++;
            case COMMAND & GET & WIDTH:                 fnNum++;
            case COMMAND & GET & HEIGHT:                fnNum++;
            case COMMAND & SET & STORE:                 fnNum++;
            case COMMAND & SET & CLEAN:                 fnNum++;
            case COMMAND & GET & FPS:                   fnNum++;
            case COMMAND & GET & PING:                  break;

            default:
                waiting_tkn = True;
        }
    }
    // Call the function if there's any selected
    // and send the response if it has finished
    if (!waiting_tkn &&
        waiting_tkn = commands[fnNum](SCI_read,buffer,byteNum++,ack)){
        SCI_write(ack>>8);
        SCI_write(ack&0xFF);
    }
}


// Command functions

bool fn_ping(uchar data, uchar *buffer, uint byteNum, uint &ack){
    if (byteNum == 0)
        return False;
    ack = data;
    return True;
}
bool fn_fps(uchar data, uchar *buffer, uint byteNum, uint &ack){
    ack = FPS_clockFreq/FPS_clockMod;
    return True;
}

bool fn_clean(uchar data, uchar *buffer, uint byteNum, uint &ack){
    uint x,y;
    for(x=MX_MAX_WIDTH;x-->0;){
        for(y=MX_MAX_HEIGHT*3/2/8;y-->0;){
            MX_pixelArray0[x][y] = 0;
            MX_pixelArray1[x][y] = 0;
        }
    }
    ack = 0xffff;
    return True;
}
bool fn_store(uchar data, uchar *buffer, uint byteNum, uint &ack);


bool get_height(uchar data, uchar *buffer, uint byteNum, uint &ack){
    ack = MX_height;
    return True;
}
bool get_width(uchar data, uchar *buffer, uint byteNum, uint &ack){
    ack = MX_width;
    return True;
}
bool get_depth(uchar data, uchar *buffer, uint byteNum, uint &ack){
    ack = MX_depth;
    return True;
}
bool get_totalWidth(uchar data, uchar *buffer, uint byteNum, uint &ack){
    ack = MX_totalWidth;
    return True;
}

bool set_height(uchar data, uchar *buffer, uint byteNum, uint &ack){
    switch(byteNum){
        case 0:
            return False;
        case 1:
            buffer[0] = data;
            return False;
        default:
            MX_height = buffer[0]<<8 & data;
            ack = 0xffff;
            return True;
    }
}
bool set_width(uchar data, uchar *buffer, uint byteNum, uint &ack){
    switch(byteNum){
        case 0:
            return False;
        case 1:
            buffer[0] = data;
            return False;
        default:
            MX_width = buffer[0]<<8 & data;
            ack = 0xffff;
            return True;
    }
}
bool set_depth(uchar data, uchar *buffer, uint byteNum, uint &ack){
    if (byteNum == 0)
        return False;
    ack = 0xffff;
    return True;
}
bool set_totalWidth(uchar data, uchar *buffer, uint byteNum, uint &ack){
    switch(byteNum){
        case 0:
            return False;
        case 1:
            buffer[0] = data;
            return False;
        default:
            MX_totalWidth = buffer[0]<<8 & data;
            ack = 0xffff;
            return True;
    }
}

bool dat_write_column(uchar data, uchar *buffer, uint byteNum, uint &ack){
    // buffer 0 = precoded flag
    // buffer 1 = X coordinate
    switch(byteNum){
        case 0:
            // Get the PRECODED flag
            buffer[0] = data & PRECODED;
            return False;
        case 1:
            // Get the X coordinate
            buffer[1] = data;
            return False;
        default:
            // If not precoded, encode it
            // (prepare the data to be sent over SPI)

            // Ignore the data meant for the other side
            if (byteNum%2 != !ID)
                return False;

            // Processing order differs between encoded and not encoded tx
            if(buffer[0]){
                // If encoded buffer the input and process 3 bytes at a time
                buffer[2+((byteNum-2)%6)/2]
                if((byteNum-2)%6/2 < 2)
                    return False;

                // Write array 0 first
                // and later array 1 if the color depth > 1
                if(byteNum-2 < MX_height*3/8){
                    MX_pixelArray0[buffer[1]][0+((byteNum-2)/2] = buffer[2];
                    MX_pixelArray0[buffer[1]][1+((byteNum-2)/2] = buffer[3];
                    MX_pixelArray0[buffer[1]][2+((byteNum-2)/2] = buffer[4];
                }
                else{
                    MX_pixelArray1[buffer[1]][0+((byteNum-2-MX_height*3/8)/2] = buffer[2];
                    MX_pixelArray1[buffer[1]][1+((byteNum-2-MX_height*3/8)/2] = buffer[3];
                    MX_pixelArray1[buffer[1]][2+((byteNum-2-MX_height*3/8)/2] = buffer[4];
                }

                // Check if all the bytes have been received
                if((   byteNum-2 >= (MX_height*3/8 -1) && MX_depth == 1)
                    || byteNum-2 >= (MX_height*3*2/8 -1)
                    ){
                    ack = 0xffff;
                    return True;
                }
            }
            else{
                // If not encoded, buffer 24 bytes (R,G,B 8b per color)
                // process both color depth arrays at the same time
                buffer[2+((byteNum-2)%48)/2]
                if((byteNum-2)%48/2 < 2)
                    return False;

                //
                encodeBytes(buffer+2,MX_depth);

                // Write  array both arrays at the same time
                MX_pixelArray0[buffer[1]][0+((byteNum-2)/48] = buffer[2];
                MX_pixelArray0[buffer[1]][1+((byteNum-2)/48] = buffer[3];
                MX_pixelArray0[buffer[1]][2+((byteNum-2)/48] = buffer[4];
                if(MX_depth != 1){
                    MX_pixelArray1[buffer[1]][0+((byteNum-2)/48] = buffer[5];
                    MX_pixelArray1[buffer[1]][1+((byteNum-2)/48] = buffer[6];
                    MX_pixelArray1[buffer[1]][2+((byteNum-2)/48] = buffer[7];
                }

                // Check if all the bytes have been received
                if((byteNum-2 >= MX_height*3 -1){
                    ack = 0xffff;
                    return True;
                }
            }
    }
    return False;
}
bool dat_write_column(uchar data, uchar *buffer, uint byteNum, uint &ack){
    * PC manda col0b0 col0b1 col1b0 ... ? *
    // buffer 0 = precoded flag
    // buffer 1 = X coordinate
    // buffer 2 = X length
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
            return False;
        default:
            // If not precoded, encode it
            // (prepare the data to be sent over SPI)

            // Ignore the data meant for the other side
            if (byteNum%2 != !ID)
                return False;

            // Processing order differs between encoded and not encoded tx
            if(buffer[0]){
                // If encoded buffer the input and process 3 bytes at a time
                buffer[2+((byteNum-2)%6)/2]
                if((byteNum-2)%6/2 < 2)
                    return False;

                // Write array 0 first
                // and later array 1 if the color depth > 1
                if(byteNum-2 < MX_height*3/8){
                    MX_pixelArray0[buffer[1]][0+((byteNum-2)/2] = buffer[2];
                    MX_pixelArray0[buffer[1]][1+((byteNum-2)/2] = buffer[3];
                    MX_pixelArray0[buffer[1]][2+((byteNum-2)/2] = buffer[4];
                }
                else{
                    MX_pixelArray1[buffer[1]][0+((byteNum-2-MX_height*3/8)/2] = buffer[2];
                    MX_pixelArray1[buffer[1]][1+((byteNum-2-MX_height*3/8)/2] = buffer[3];
                    MX_pixelArray1[buffer[1]][2+((byteNum-2-MX_height*3/8)/2] = buffer[4];
                }

                // Check if all the bytes have been received
                if((   byteNum-2 >= (MX_height*3/8 -1) && MX_depth == 1)
                    || byteNum-2 >= (MX_height*3*2/8 -1)
                    ){
                    ack = 0xffff;
                    return True;
                }
            }
            else{
                // If not encoded, buffer 24 bytes (R,G,B 8b per color)
                // process both color depth arrays at the same time
                buffer[2+((byteNum-2)%48)/2]
                if((byteNum-2)%48/2 < 2)
                    return False;

                //
                encodeBytes(buffer+2,MX_depth);

                // Write  array both arrays at the same time
                MX_pixelArray0[buffer[1]][0+((byteNum-2)/48] = buffer[2];
                MX_pixelArray0[buffer[1]][1+((byteNum-2)/48] = buffer[3];
                MX_pixelArray0[buffer[1]][2+((byteNum-2)/48] = buffer[4];
                if(MX_depth != 1){
                    MX_pixelArray1[buffer[1]][0+((byteNum-2)/48] = buffer[5];
                    MX_pixelArray1[buffer[1]][1+((byteNum-2)/48] = buffer[6];
                    MX_pixelArray1[buffer[1]][2+((byteNum-2)/48] = buffer[7];
                }

                // Check if all the bytes have been received
                if((byteNum-2 >= MX_height*3 -1){
                    ack = 0xffff;
                    return True;
                }
            }
    }
    return False;
}
bool dat_burst(uchar data, uchar *buffer, uint byteNum, uint ack);
bool dat_interlaced_burst(uchar data, uchar *buffer, uint byteNum, uint &ack);



void encodeBytes(uchar* buff, uchar depth){
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
                buff[i/8] = tempByte0;
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

                buff[i/4-1] = tempByte0;
                buff[i/4] = tempByte1;

                tempByte0 = 0;
                tempByte1 = 0;
            }
        }
    }
}
