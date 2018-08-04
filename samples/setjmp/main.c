// This code has been taken from https://stackoverflow.com/questions/14685406/practical-usage-of-setjmp-and-longjmp-in-c

#include <xboxrt/debug.h>
#include <pbkit/pbkit.h>
#include <hal/xbox.h>

#include "stdio.h"
#include <setjmp.h>

jmp_buf bufferA, bufferB;

void routineB(); // forward declaration 

void routineA()
{
    int r ;

    debugPrint("(A1)\n");

    r = setjmp(bufferA);
    if (r == 0) routineB();

    debugPrint("(A2) r=%d\n",r);

    r = setjmp(bufferA);
    if (r == 0) longjmp(bufferB, 20001);

    debugPrint("(A3) r=%d\n",r);

    r = setjmp(bufferA);
    if (r == 0) longjmp(bufferB, 20002);

    debugPrint("(A4) r=%d\n",r);
}

void routineB()
{
    int r;

    debugPrint("(B1)\n");

    r = setjmp(bufferB);
    if (r == 0) longjmp(bufferA, 10001);

    debugPrint("(B2) r=%d\n", r);

    r = setjmp(bufferB);
    if (r == 0) longjmp(bufferA, 10002);

    debugPrint("(B3) r=%d\n", r);

    r = setjmp(bufferB);
    if (r == 0) longjmp(bufferA, 10003);
}

void main(void)
{
    int i;

    switch(pb_init())
    {
        case 0: break;
        default:
            XSleep(2000);
            XReboot();
            return;
    }

    pb_show_debug_screen();

    while(1) {
        routineA();
        XSleep(2000);
    }

    pb_kill();
    XReboot();
}
