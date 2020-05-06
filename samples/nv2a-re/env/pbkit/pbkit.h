#ifndef __PBKIT_H__
#define __PBKIT_H__

#include "pbkit/nv_regs.h"
#include "pbkit/outer.h"

#include <stdint.h>

typedef uint32_t DWORD;

#define SUBCH_3D                0

uint32_t* pb_begin();
void pb_end(uint32_t* p);

#define EncodeMethod(subchannel,command,nparam) ((nparam<<18)+(subchannel<<13)+command)

static void pb_push_to(DWORD subchannel, uint32_t *p, DWORD command, DWORD nparam)
{
    *(p+0)=EncodeMethod(subchannel,command,nparam);
}

static void pb_push(uint32_t *p, DWORD command, DWORD nparam)
{
    pb_push_to(SUBCH_3D,p,command,nparam);
}

static uint32_t *pb_push1_to(DWORD subchannel, uint32_t *p, DWORD command, DWORD param1)
{
    pb_push_to(subchannel,p,command,1);
    *(p+1)=param1;
    return p+2;
}

static uint32_t *pb_push1(uint32_t *p, DWORD command, DWORD param1)
{
    return pb_push1_to(SUBCH_3D,p,command,param1);
}

// Hook for the user
void _pb_emit(void* data, size_t size);


#endif
