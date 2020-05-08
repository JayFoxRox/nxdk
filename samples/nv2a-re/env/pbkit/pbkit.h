#ifndef __PBKIT_H__
#define __PBKIT_H__

#include "pbkit/nv_regs.h"
#include "pbkit/outer.h"

#include <stdint.h>
#include <string.h>

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





// Compatibility warnings
// #FIXME: Warn for each of these

//FIXME: These functions are just stubs
#define pb_busy() 0
#define pb_finished() 0
#define pb_init() 0
#define pb_kill()
#define pb_wait_for_vbl(...)
#define pb_reset()

//FIXME: These functions will likely be removed from pbkit
#define pb_back_buffer_width() 640
#define pb_back_buffer_height() 480
#define pb_show_front_screen()
#define pb_target_back_buffer()
#define pb_show_debug_screen()

//FIXME: These functions will likely be removed from pbkit, but need replacements
#define pb_erase_depth_stencil_buffer(...) //FIXME: Do manually
#define pb_fill(...) //FIXME: Do manually






#endif
