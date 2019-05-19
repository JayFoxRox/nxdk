#ifndef __XBOXNT_H__
#define __XBOXNT_H__

typedef struct _STRING {
    USHORT Length;
    USHORT MaximumLength;
    PCHAR  Buffer;
} ANSI_STRING, *PANSI_STRING;

#endif
