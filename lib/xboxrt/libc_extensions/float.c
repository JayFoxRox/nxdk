#include <float.h>
#include <stdlib.h>

// Check architecture
// Warning: This code also assumes a lack of SSE2+
#if (!defined(__i386__))
#error Unsupported architecture
#endif

#define _SW_INEXACT 0x00000001
#define _SW_UNDERFLOW 0x00000002
#define _SW_OVERFLOW 0x00000004
#define _SW_ZERODIVIDE 0x00000008
#define _SW_INVALID 0x00000010
#define _SW_DENORMAL 0x00080000

unsigned int _clear87(void)
{
    unsigned long fpword;
#if defined(__GNUC__) || defined(__clang__)
    __asm__ __volatile__( "fnstsw %0; fnclex" : "=m" (fpword) );
#else
#error Unsupported compiler
#endif

    unsigned int flags = 0;
    if (fpword & 0x1)  flags |= _SW_INVALID;
    if (fpword & 0x2)  flags |= _SW_DENORMAL;
    if (fpword & 0x4)  flags |= _SW_ZERODIVIDE;
    if (fpword & 0x8)  flags |= _SW_OVERFLOW;
    if (fpword & 0x10) flags |= _SW_UNDERFLOW;
    if (fpword & 0x20) flags |= _SW_INEXACT;

    return flags;
}

unsigned int _clearfp(void)
{
    return _clear87();
}

unsigned int _control87(unsigned int new, unsigned int mask)
{
    unsigned int x86_cw;
    __control87_2(new, mask, &x86_cw, NULL);
    return x86_cw;
}

unsigned int _controlfp(unsigned int new, unsigned int mask)
{
    return _control87(new, mask);
}

int __control87_2(unsigned int new, unsigned int mask, unsigned int* x86_cw, unsigned int* sse2_cw)
{
    if (x86_cw) {
        unsigned long fpword;
#if defined(__GNUC__) || defined(__clang__)
        __asm__ __volatile__( "fstcw %0" : "=m" (fpword) );
#else
#error Unsupported compiler
#endif

        /* Convert into mask constants */
        unsigned int flags = 0;
        if (fpword & 0x1)  flags |= _EM_INVALID;
        if (fpword & 0x2)  flags |= _EM_DENORMAL;
        if (fpword & 0x4)  flags |= _EM_ZERODIVIDE;
        if (fpword & 0x8)  flags |= _EM_OVERFLOW;
        if (fpword & 0x10) flags |= _EM_UNDERFLOW;
        if (fpword & 0x20) flags |= _EM_INEXACT;

        switch (fpword & 0xc00) {
        case 0xc00: flags |= _RC_UP|_RC_DOWN; break;
        case 0x800: flags |= _RC_UP; break;
        case 0x400: flags |= _RC_DOWN; break;
        }

        switch (fpword & 0x300) {
        case 0x0:   flags |= _PC_24; break;
        case 0x200: flags |= _PC_53; break;
        case 0x300: flags |= _PC_64; break;
        }

        if (fpword & 0x1000) flags |= _IC_AFFINE;

        if (mask) {
            flags = (flags & ~mask) | (new & mask);

            /* Convert (masked) value back to fp word */
            fpword = 0;
            if (flags & _EM_INVALID)    fpword |= 0x1;
            if (flags & _EM_DENORMAL)   fpword |= 0x2;
            if (flags & _EM_ZERODIVIDE) fpword |= 0x4;
            if (flags & _EM_OVERFLOW)   fpword |= 0x8;
            if (flags & _EM_UNDERFLOW)  fpword |= 0x10;
            if (flags & _EM_INEXACT)    fpword |= 0x20;

            switch (flags &  _MCW_RC) {
            case _RC_UP|_RC_DOWN: fpword |= 0xc00; break;
            case _RC_UP:          fpword |= 0x800; break;
            case _RC_DOWN:        fpword |= 0x400; break;
            }

            switch (flags &  _MCW_PC) {
            case _PC_64: fpword |= 0x300; break;
            case _PC_53: fpword |= 0x200; break;
            case _PC_24: fpword |= 0x0; break;
            }

            if (flags & _IC_AFFINE) fpword |= 0x1000;

#if defined(__GNUC__) || defined(__clang__)
            __asm__ __volatile__( "fldcw %0" : : "m" (fpword) );
#else
#error Unsupported compiler
#endif
        }

        *x86_cw = flags;
    }

    if (sse2_cw) {
        *sse2_cw = 0;
    }

    return 1;
}

unsigned int _status87(void)
{
    unsigned int x86_sw;
    _statusfp2(&x86_sw, NULL);
    return x86_sw;
}

unsigned int _statusfp(void)
{
    return _status87();
}

void _statusfp2(unsigned int *px86, unsigned int *pSSE2)
{
    if (px86) {
        unsigned long fpword;
#if defined(__GNUC__) || defined(__clang__)
        __asm__ __volatile__( "fstsw %0" : "=m" (fpword) );
#else
#error Unsupported compiler
#endif

        unsigned int flags = 0;
        if (fpword & 0x1)  flags |= _SW_INVALID;
        if (fpword & 0x2)  flags |= _SW_DENORMAL;
        if (fpword & 0x4)  flags |= _SW_ZERODIVIDE;
        if (fpword & 0x8)  flags |= _SW_OVERFLOW;
        if (fpword & 0x10) flags |= _SW_UNDERFLOW;
        if (fpword & 0x20) flags |= _SW_INEXACT;

        *px86 = flags;
    }

    if (pSSE2) {
        *pSSE2 = 0;
    }
}
