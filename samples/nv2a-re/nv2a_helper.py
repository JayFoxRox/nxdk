# Mostly stolen from nv2a-trace helper.py (in some instances modified!)

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
  s1 = xbox.read_u32(put_state)
  xbox.write_u32(put_state, s1 & 0xFFFFFFFE)

def resume_fifo_pusher(xbox):
  s2 = xbox.read_u32(put_state)
  xbox.write_u32(put_state, (s2 & 0xFFFFFFFE) | 1) # Recover pusher state

def wait_until_puller_idle(xbox):
  while(xbox.read_u32(get_state) & (1 << 4)):
    pass

def pause_fifo_puller(xbox):
  s1 = xbox.read_u32(get_state)
  xbox.write_u32(get_state, s1 & 0xFFFFFFFE)

def resume_fifo_puller(xbox):
  s2 = xbox.read_u32(get_state)
  xbox.write_u32(get_state, (s2 & 0xFFFFFFFE) | 1) # Recover pusher state

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

