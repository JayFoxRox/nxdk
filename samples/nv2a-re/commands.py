import os
import subprocess
import PIL.Image

import globals

from resources import Args

def texture_A8R8G8B8(image):
  return image.tobytes()

def convert_image(in_path, converter, out_path):
  image = PIL.Image.open(globals.BASE_PATH + "/" + in_path)
  image = image.convert("RGBA")
  data = converter(image)
  open(globals.BASE_PATH + "/" + out_path, "wb").write(data)
  return

def run(program_path, *args):

  run_args = []
  for arg in args:
    if isinstance(arg, Args):
      run_args += arg.args()
    elif arg == None:
      run_args += [""]
    elif isinstance(arg, str):
      run_args += [arg]
    else:
      print(arg)
      assert(False)

  print("Running %s:" % program_path, run_args)

  process = subprocess.Popen([program_path, *run_args], cwd=globals.BASE_PATH)
  stdoutdata, stderrdata = process.communicate()
  print()

  if process.returncode:
    print("Running failed!")
    raise Exception()
  return Args([])

def compile_c(out_path, *in_args):
  # Run compiler
  print(in_args)
  process = subprocess.Popen(["clang", *in_args, "../../env/pbkit/pbkit.c","-I../../env","-I./../../../../lib/","-g","-O0","-o", out_path], cwd=globals.BASE_PATH)
  stdoutdata, stderrdata = process.communicate()
  if process.returncode:
    print("C compiler failed!")
    raise Exception()
  return Args([])

def compile_fp(out_path, in_path):
  process = subprocess.Popen([os.environ['NXDK_DIR'] + "/tools/fp20compiler/fp20compiler", globals.BASE_PATH + "/" + in_path], stdout=subprocess.PIPE)
  stdoutdata, stderrdata = process.communicate()
  if process.returncode:
    print("fp20compiler failed!")
    raise Exception()
  open(globals.BASE_PATH + "/" + out_path, "wb").write(stdoutdata)
  return Args([out_path])
