#!/usr/bin/env python3

import os
import datetime

import globals

from xbox import *

xbox = Xbox(("127.0.0.1", 9269))

from nv2a_helper import *
from filewatch import *
from resources import *
from commands import *

globals.BASE_PATH = "samples/textured_triangle"


def clear_output():
  os.system('cls' if os.name == 'nt' else 'clear')

def run_pushbuffer(pb_addr):
  # Pause pushbuffer
  pause_fifo_pusher(xbox)
  pause_fifo_puller(xbox)

  # Redirect Xbox to pushbuffer
  print("Xbox was at 0x%08X / 0x%08X" % (xbox.read_u32(dma_get_addr), xbox.read_u32(dma_put_addr)))
  xbox.write_u32(dma_get_addr, (pb_addr.address() +              0) & 0x7FFFFFFF)
  xbox.write_u32(dma_put_addr, (pb_addr.address() + pb_addr.size()) & 0x7FFFFFFF)
  print("Xbox was at 0x%08X / 0x%08X" % (xbox.read_u32(dma_get_addr), xbox.read_u32(dma_put_addr)))

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

def do_task():

  # Warn user about new output
  #clear_output()
  print("Updating output %s!" % datetime.datetime.now())
  print()

  # Start display
  show_xbox_backbuffer(xbox)

  # Generate fragment program
  compile_fp("tmp/fp.inl", "fp.fp")

  # Generate pushbuffer generator
  compile_c("tmp/pb", "pb.c")

  tex_addrs = [None] * 4
  for i in range(4):

    try:
      convert_image("tex%d.png" % i, texture_A8R8G8B8, "tmp/tex%d.bin" % i)
      tex_addrs[i] = ContiguousResourceFromFile(xbox, "tmp/tex%d.bin" % i)
      #assert(tex_addrs[i].width() == 64)
      #assert(tex_addrs[i].height() == 64)
      assert(tex_addrs[i].size() == (64 * 64 * 4))
      tex_addrs[i].begin()
    except:
      continue

  # Run pushbuffer generator
  args = [
    tex_addrs[0],
    tex_addrs[1],
    tex_addrs[2],
    tex_addrs[3],
  ]
  run("tmp/pb", "tmp/pb.bin", *args)

  # Load generated pushbuffer
  pb_addr = ContiguousResourceFromFile(xbox, "tmp/pb.bin")
  pb_addr.begin()
  print("pb at 0x%08X" % pb_addr.address())

  run_pushbuffer(pb_addr)

  # Free pushbuffer
  pb_addr.end()
  for tex_addr in tex_addrs:
    if tex_addr != None:
      tex_addr.end()

  # Report success
  print("Success!")


if __name__ == "__main__":

  # Run task
  do_task()

  # Start filewatch.py stuff
  event_handler = MyHandler()
  observer = Observer()
  observer.schedule(event_handler, path=".", recursive=False)
  observer.start()

  while True:
    pass
