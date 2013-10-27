#include "snippets.h"

// Debuger
// todo: buscar un mejor lugar para esto
uchar DEBUG;

// 8b Integer to cstring
uchar* SNP_ctoa(uchar number, uchar *buff){
    uchar *ptr;

    ptr = buff+3;
    *ptr = '\0';
    if (!number)
        *(--ptr) = '0';
    for (;number;number/=10)
        *(--ptr) = (number % 10) + '0';
    return ptr;
}
// 8b Integer to cstring, hexa, 2 digits
uchar* SNP_ctoh(uchar number, uchar *buff){
    uchar *ptr;
    uchar i;

    ptr = buff+2;
    *ptr = '\0';
    if (!number){
        *(--ptr) = '0';
        *(--ptr) = '0';
    }
    else{
        for(i=2;i-->0;number /= 16){
            if (number % 16 < 10)
                *(--ptr) = (number % 16) + '0';
            else
                *(--ptr) = number % 16 - 10 + 'a';
        }
    }

    return ptr+2;
}
// 8b Integer to cstring, d chars
uchar* SNP_ctoaN(uchar number, uchar *buff, uchar d){
    uchar *ptr;

    ptr = buff+d;
    *ptr = '\0';
    for (;ptr != buff;number/=10)
        *(--ptr) = (number % 10) + '0';
    return ptr;
}
// 16b Integer to cstring
uchar* SNP_itoa(uint number, uchar *buff){
    uchar *ptr;

    ptr = buff+5;
    *ptr = '\0';
    if (!number)
        *(--ptr) = '0';
    for (;number;number/=10)
        *(--ptr) = (number % 10) + '0';
    return ptr;
}
// 16b Integer to cstring, hexa, 4 digits
uchar* SNP_itoh(uint number, uchar *buff){
    uchar *ptr;
    uchar i;

    ptr = buff+4;
    *ptr = '\0';
    if (!number){
        *(--ptr) = '0';
        *(--ptr) = '0';
    }
    else{
        for(i=4;i-->0;number /= 16){
            if (number % 16 < 10)
                *(--ptr) = (number % 16) + '0';
            else
                *(--ptr) = number % 16 - 10 + 'a';
        }
    }

    return ptr+4;
}
// 16b Integer to cstring, d decimals
uchar* SNP_itoaD(uint number, uchar *buff, uchar d){
    uchar *ptr;

    ptr = buff+6;
    *ptr = '\0';
    if (!number)
        *(--ptr) = '0';
    for (;number;number/=10){
        *(--ptr) = (number % 10) + '0';
        if (d)
            if(!--d)
                *(--ptr) = '.';
    }
    return ptr;
}
// 16b Integer to cstring, d chars
uchar* SNP_itoaN(uint number, uchar *buff, uchar d){
    uchar *ptr;

    ptr = buff+d;
    *ptr = '\0';
    for (;ptr != buff;number/=10)
        *(--ptr) = (number % 10) + '0';
    return ptr;
}



// Copies one str to another,
// returns a pointer to the last char of the 2nd str
uchar* SNP_strCopy(uchar* ptr0,uchar* ptr1){
    for(;*ptr0;ptr0++)
        *(ptr1++) = *ptr0;
    *ptr1 = '\0';
    return ptr1;
}

// Returns the str len, excluding the null char
uchar SNP_strLen(uchar* str){
    uchar i = 0;
    for(;*(str+i) && i<=255; i++);
    return i;
}

// Fills the string with the given char
uchar* SNP_strFill(uchar* ptr,uchar* end, uchar ch){
    for (;ptr < end;ptr++)
        *ptr = ch;
    *ptr = '\0';
    return ptr;
}

// Calculates the log2 of a number
uchar LOG2_l(unsigned long x){
    return x ? (LOG2_i(x&0xFFFF) + LOG2_l(x>>16)) : 0;
}
