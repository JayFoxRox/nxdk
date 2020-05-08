# Mostly stolen from nv2a-trace helper.py (in some instances modified!)

import time

dma_state = 0xFD003228
dma_put_addr = 0xFD003240
dma_get_addr = 0xFD003244
dma_subroutine = 0xFD00324C

put_addr = 0xFD003210
put_state = 0xFD003220
get_addr = 0xFD003270
get_state = 0xFD003250

def wait_until_pusher_idle(xbox):
  while(xbox.read_u32(put_state) & (1 << 4)):
    pass

def pause_fifo_pusher(xbox):
  xbox.write_u32(put_state, 0)

def resume_fifo_pusher(xbox):
  xbox.write_u32(put_state, 1) # Recover pusher state

def wait_until_puller_idle(xbox):
  while(xbox.read_u32(get_state) & (1 << 4)):
    pass

def pause_fifo_puller(xbox):
  xbox.write_u32(get_state, 0)

def resume_fifo_puller(xbox):
  xbox.write_u32(get_state, 1) # Recover pusher state

def show_xbox_frontbuffer(xbox):
  req = xbox.nxdk_rdt.Request()
  req.type = xbox.nxdk_rdt.Request.SHOW_FRONT_SCREEN
  return xbox.nxdk_rdt._send_simple_request(req)

def show_xbox_backbuffer(xbox):
  # Get backbuffer and display it
  if False:
    #FIXME: Does not work in XQEMU
    color_offset = xbox.read_u32(0xFD400828)
    depth_offset = xbox.read_u32(0xFD40082C)
    color_base = xbox.read_u32(0xFD400840)
    depth_base = xbox.read_u32(0xFD400844)
    print("color is at 0x%08X+0x%08X" % (color_base, color_offset))
    xbox.write_u32(0xFD600800, color_base + color_offset)
  else:
    # Get to a fixed address and advance to next buffer
    show_xbox_frontbuffer(xbox)
    fb_addr = xbox.read_u32(0xFD600800)
    print("fb was at 0x%08X" % fb_addr)
    xbox.write_u32(0xFD600800, fb_addr + 1 * 640 * 480 * 4)

def dump_cache(xbox):
  print("cache:")
  cache_put = xbox.read_u32(put_addr)
  cache_get = xbox.read_u32(get_addr)
  for i in range(128):
    c = xbox.read_u32(0xFD003800 + i * 8 + 0)
    v = xbox.read_u32(0xFD003800 + i * 8 + 4)
    s = ""
    if (i * 8) == (cache_get & ~7):
      s += " G"
      if (cache_get & 7): s += "*"
    if (i * 8) == (cache_put & ~7):
      s += " P"
      if (cache_put & 7): s += "*"
    print("  [0x%02X] 0x%08X:0x%08X %s" % (i * 8, c, v, s))

def run_pushbuffer(xbox, pb_addr):

  # Wait for pushbuffer to finish
  wait_until_pusher_idle(xbox)
  wait_until_puller_idle(xbox)

  # Pause pushbuffer
  pause_fifo_pusher(xbox)
  pause_fifo_puller(xbox)

  #xbox.write_u32(0xFD002500, 0) 

  print("PFIFO_DMA: 0x%08X" % xbox.read_u32(0xFD002508))
  #xbox.write_u32(0xFD002508, 1)
  #xbox.write_u32(0xFD002508, 0)

  # Clear methods in DMA buffer
  print("Xbox was at 0x%08X / 0x%08X [ring]" % (xbox.read_u32(get_addr), xbox.read_u32(put_addr)))
  print("Xbox state: 0x%08X" % (xbox.read_u32(dma_state)))
  print("Xbox was at 0x%08X / 0x%08X" % (xbox.read_u32(dma_get_addr), xbox.read_u32(dma_put_addr)))
  xbox.write_u32(dma_state, 0)
  print("Xbox state: 0x%08X" % (xbox.read_u32(dma_state)))

  #xbox.write_u32(0xFD002500, 1)
  #xbox.write_u32(dma_get_addr, 0)
  #xbox.write_u32(dma_put_addr, 0)
  #xbox.write_u32(0x80000000, (pb_addr.address() & 0x7FFFFFFC) | 1)

  #define NV_PFIFO_CACHE1_PUSH1					0x00003204
  #xbox.write_u32(0xFD003204, xbox.read_u32(0xFD003204) & ~0x100) # Disable DMA

  # Redirect Xbox to pushbuffer
  print("Xbox was at 0x%08X / 0x%08X" % (xbox.read_u32(dma_get_addr), xbox.read_u32(dma_put_addr)))
  xbox.write_u32(dma_get_addr, (pb_addr.address() +              0) & 0x7FFFFFFF)
  print("Xbox was at 0x%08X / 0x%08X" % (xbox.read_u32(dma_get_addr), xbox.read_u32(dma_put_addr)))
  xbox.write_u32(dma_put_addr, (pb_addr.address() + pb_addr.size()) & 0x7FFFFFFF)
  print("Xbox was at 0x%08X / 0x%08X" % (xbox.read_u32(dma_get_addr), xbox.read_u32(dma_put_addr)))
  print("Xbox state: 0x%08X" % (xbox.read_u32(dma_state)))

  #xbox.write_u32(0xFD002500, 1)

  #xbox.write_u32(0xFD003204, xbox.read_u32(0xFD003204) | 0x100) # Enable DMA

  #xbox.write_u32(put_addr, 0)
  #xbox.write_u32(get_addr, 0)



  #dump_cache(xbox)

  # Resume pushbuffer
  resume_fifo_pusher(xbox)
  resume_fifo_puller(xbox)

  # Wait for pushbuffer to finish
  wait_until_pusher_idle(xbox)
  wait_until_puller_idle(xbox)

  # Pause pushbuffer
  pause_fifo_pusher(xbox)
  pause_fifo_puller(xbox)

  # Check if we reached our goal
  print("Xbox was at 0x%08X / 0x%08X" % (xbox.read_u32(dma_get_addr), xbox.read_u32(dma_put_addr)))
  print("Xbox was at 0x%08X / 0x%08X [ring]" % (xbox.read_u32(get_addr), xbox.read_u32(put_addr)))
  print("Xbox state: 0x%08X" % (xbox.read_u32(dma_state)))

  #dump_cache(xbox)
