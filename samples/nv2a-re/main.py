#!/usr/bin/env python3

import os
import sys
import time
import datetime

import globals

from xbox import *
from nv2a_helper import *
from filewatch import *
from resources import *
from commands import *

globals.BASE_PATH = sys.argv[1] #"samples/textured_triangle"


def clear_output():
  os.system('cls' if os.name == 'nt' else 'clear')


if __name__ == "__main__":


  xbox_list = []
  for arg in sys.argv[2:]:
    host = arg
    port = 9269 #FIXME: Also accept port from arg
    print("Connecting to %s:%d" % (host, port))
    xbox_list += [Xbox((host, port))]

  #FIXME: Start filewatch.py stuff?

  task_code = open(globals.BASE_PATH + "/ugly_task.py").read()
  exec(task_code)

  # Create task
  for xbox in xbox_list:
    task = Task(xbox)
    task.begin()

  # Run forever
  while True:
    print("Doing task")
    for xbox in xbox_list:
      task.do()
    time.sleep(1.0)

  for xbox in xbox_list:
    task.end()
