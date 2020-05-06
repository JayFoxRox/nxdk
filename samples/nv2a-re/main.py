#!/usr/bin/env python3

import os
import sys
import time
import datetime

import globals

from xbox import *

xbox = Xbox(("127.0.0.1", 9269))

from nv2a_helper import *
from filewatch import *
from resources import *
from commands import *

globals.BASE_PATH = sys.argv[1] #"samples/textured_triangle"


def clear_output():
  os.system('cls' if os.name == 'nt' else 'clear')


if __name__ == "__main__":

  task_code = open(globals.BASE_PATH + "/ugly_task.py").read()
  exec(task_code)


  # Create task
  task = Task(xbox)

  #FIXME: Start filewatch.py stuff?

  # Run forever
  while True:
    task.do()
    time.sleep(1.0)
