#!/usr/bin/env python3

from xboxpy import *

import os
import datetime
import subprocess
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
import watchdog.events

dma_state = 0xFD003228
dma_put_addr = 0xFD003240
dma_get_addr = 0xFD003244
dma_subroutine = 0xFD00324C

put_addr = 0xFD003210
put_state = 0xFD003220
get_addr = 0xFD003270
get_state = 0xFD003250

#FIXME: This needs to be more granular:
# if C code changes: recompile
# if tool changes: rerun
# if data changes: redraw
# if python script changes: rerun
watchlist = ["generate_pb.c"]

# Hack to pretend we have a better API in xboxpy
class Xbox:
  def __init__(self):
    self.read_u32 = read_u32
    self.read = read
    self.write = write
    self.write_u32 = write_u32
    self.ke = ke
xbox = Xbox()

def wait_until_pusher_idle():
  while(xbox.read_u32(put_state) & (1 << 4)):
    pass

def pause_fifo_pusher():
  s1 = xbox.read_u32(put_state)
  xbox.write_u32(put_state, s1 & 0xFFFFFFFE)

def resume_fifo_pusher():
  s2 = xbox.read_u32(put_state)
  xbox.write_u32(put_state, (s2 & 0xFFFFFFFE) | 1) # Recover pusher state

def wait_until_puller_idle():
  while(xbox.read_u32(get_state) & (1 << 4)):
    pass

def pause_fifo_puller():
  s1 = xbox.read_u32(get_state)
  xbox.write_u32(get_state, s1 & 0xFFFFFFFE)

def resume_fifo_puller():
  s2 = xbox.read_u32(get_state)
  xbox.write_u32(get_state, (s2 & 0xFFFFFFFE) | 1) # Recover pusher state

def clear_output():
  os.system('cls' if os.name == 'nt' else 'clear')

def show_xbox_front():
  req = interface.if_nxdk_rdt.Request()
  req.type = interface.if_nxdk_rdt.Request.SHOW_FRONT_SCREEN
  return interface.if_nxdk_rdt._send_simple_request(req)


def do_task():

  # Warn user about new output
  #clear_output()
  print("Updating output %s!" % datetime.datetime.now())
  print()

  # Run compiler
  process = subprocess.Popen(["clang","generate_pb.c","-I./env","-I./../../lib/","-g","-O0","-o","generate_pb"])
  stdoutdata, stderrdata = process.communicate()
  print()

  # Check compiler output
  print(process.returncode)
  if process.returncode:
    print("Compiler generator failed!")
    return

  #FIXME: Upload resources?

  # Run compiled tool
  process = subprocess.Popen(["./generate_pb"])
  stdoutdata, stderrdata = process.communicate()
  print()

  # Check tool output
  print(process.returncode)
  if process.returncode:
    print("Generating pushbuffer failed!")
    return

  # Load pushbuffer
  pb = open("pb.bin", "rb").read()
  assert(len(pb) % 4 == 0)

  #FIXME: Upload new pushbuffer
  pb_addr = xbox.ke.MmAllocateContiguousMemory(len(pb))
  xbox.write(pb_addr, pb)
  print("pb at 0x%08X" % pb_addr)

  # Pause pushbuffer
  pause_fifo_pusher()
  pause_fifo_puller()

  # Redirect Xbox to pushbuffer
  print("Xbox was at 0x%08X / 0x%08X" % (xbox.read_u32(dma_get_addr), xbox.read_u32(dma_put_addr)))
  xbox.write_u32(dma_get_addr, (pb_addr +       0) & 0x7FFFFFFF)
  xbox.write_u32(dma_put_addr, (pb_addr + len(pb)) & 0x7FFFFFFF)
  print("Xbox was at 0x%08X / 0x%08X" % (xbox.read_u32(dma_get_addr), xbox.read_u32(dma_put_addr)))

  # Resume pushbuffer
  resume_fifo_pusher()
  resume_fifo_puller()

  # Wait for pushbuffer to finish
  wait_until_pusher_idle()
  wait_until_puller_idle()

  # Pause pushbuffer
  pause_fifo_pusher()
  pause_fifo_puller()

  # Check if we reached our goal
  print("Xbox was at 0x%08X / 0x%08X" % (xbox.read_u32(dma_get_addr), xbox.read_u32(dma_put_addr)))

  # Free pushbuffer
  xbox.ke.MmFreeContiguousMemory(pb_addr)

  # Report success
  print("Success!")


def handle_modification(path):
  print("modification in %s" % path)

class MyHandler(watchdog.events.FileSystemEventHandler):
  def on_any_event(self, event):
    print(f'event type: {event.event_type}  path : {event.src_path}')
    handle_modification(event.src_path)
    if isinstance(event, watchdog.events.FileMovedEvent):
      handle_modification(event.dest_path)


if __name__ == "__main__":

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
    show_xbox_front()
    fb_addr = xbox.read_u32(0xFD600800)
    print("fb was at 0x%08X" % fb_addr)
    xbox.write_u32(0xFD600800, fb_addr + 1 * 640 * 480 * 4)

  # Run task
  do_task()

  event_handler = MyHandler()
  observer = Observer()
  observer.schedule(event_handler, path=".", recursive=False)
  observer.start()

  while True:
    pass
