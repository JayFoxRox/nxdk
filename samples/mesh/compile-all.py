#!/usr/bin/env python3

import subprocess
from pathlib import Path
import time

i = 1
for path in sorted(Path('.').glob('*-ps.inl.*')):
  if path.name[-4:].lower() == ".iso":
    continue
  with open("ps.inl", "wb") as f:
    subprocess.run(["../../tools/fp20compiler/fp20compiler", path.resolve()], stdout=f)
  subprocess.run(["make", "XBE_TITLE=%03d.\\ %s" % (i, path.name)])
  Path('./ps.inl').unlink()
  output_folder = Path("./bin/%s/" % (path.name))
  if not output_folder.is_dir():
    output_folder.mkdir()
  output_file = Path(output_folder, "default.xbe")
  if output_file.exists():
    output_file.unlink()
  Path('./bin/default.xbe').rename(output_file)
  i += 1

