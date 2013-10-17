#ifndef SNIPPETS_H_GUARD
#define SNIPPETS_H_GUARD

// Debuger
// todo: buscar un mejor lugar para esto
#define DEBUG_ENABLED
extern uchar DEBUG;
#define DEBUG_MODES 3
#define DEBUG_DISPLAY 1
#define DEBUG_USB 2
#define DEBUG_ZERO 3

// 8b Integer to cstring
uchar* SNP_ctoa(uchar, uchar *buff);
// 8b Integer to cstring, hexa, 2 digits
uchar* SNP_ctoh(uchar, uchar *buff);
// 8b Integer to cstring, d chars
uchar* SNP_ctoaN(uchar, uchar *buff, uchar d);
// 16b Integer to cstring
uchar* SNP_itoa(uint, uchar *buff);
// 16b Integer to cstring, hexa, 4 digits
uchar* SNP_itoh(uint, uchar *buff);
// 16b Integer to cstring, d decimals
uchar* SNP_itoaD(uint, uchar *buf, uchar d);
// 16b Integer to cstring, d chars
uchar* SNP_itoaN(uint, uchar *buff, uchar d);



// Writes part of the variable
//void SNP_maskWrite(uchar* var, uchar data, uchar mask);
#define SNP_maskWrite(var,data,mask) {*(var) = (*(var) & ~(mask)) | ((data) & (mask));}

// Reads part of the variable
//uchar SNP_maskRead(uchar* var,uchar offset, uchar mask);
#define SNP_maskRead(var,offset,mask) ((*(var)&(mask))>>(offset))


// Copies one str to another,
// returns a pointer to the last char of the 2nd str
uchar* SNP_strCopy(uchar* from,uchar* to);

// Returns the str len, excluding the null char
uchar SNP_strLen(uchar* str);

// Fills the string with the given char
uchar* SNP_strFill(uchar* start,uchar* end, uchar ch);



// Calculates the log2 of a number
#define LOG2_c(x) ((  \
                ((x) &   0x80) ?  7 : \
                ((x) &   0x40) ?  6 : \
                ((x) &   0x20) ?  5 : \
                ((x) &   0x10) ?  4 : \
                ((x) &   0x08) ?  3 : \
                ((x) &   0x04) ?  2 : \
                ((x) &   0x02) ?  1 : \
                0                 ))
#define LOG2_i(x) ((  \
                ((x) & 0x8000) ? 15 : \
                ((x) & 0x4000) ? 14 : \
                ((x) & 0x2000) ? 13 : \
                ((x) & 0x1000) ? 12 : \
                ((x) & 0x0800) ? 11 : \
                ((x) & 0x0400) ? 10 : \
                ((x) & 0x0200) ?  9 : \
                ((x) & 0x0100) ?  8 : \
                LOG2_c((x))       ))
uchar LOG2_l(unsigned long x);

#endif // SNIPPETS_H_GUARD
