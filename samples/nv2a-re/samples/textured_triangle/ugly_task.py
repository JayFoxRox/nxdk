import datetime

class Task():

  def __init__(self, xbox):
    self._xbox = xbox
  def do(self):

    # Warn user about new output
    #clear_output()
    print("Updating output %s!" % datetime.datetime.now())
    print()

    # Start display
    show_xbox_backbuffer(self._xbox)

    # Generate fragment program
    compile_fp("tmp/fp.inl", "fp.fp")

    # Generate pushbuffer generator
    compile_c("tmp/pb", "pb.c")

    tex_addrs = [None] * 4
    for i in range(4):

      try:
        convert_image("tex%d.png" % i, texture_A8R8G8B8, "tmp/tex%d.bin" % i)
        tex_addrs[i] = ContiguousResourceFromFile(self._xbox, "tmp/tex%d.bin" % i)
        #assert(tex_addrs[i].width() == 64)
        #assert(tex_addrs[i].height() == 64)
        assert(tex_addrs[i].size() == (64 * 64 * 4))
        tex_addrs[i].begin()
      except:
        continue

    # Run pushbuffer generator
    run("tmp/pb", "tmp/pb.bin", *tex_addrs)

    # Load generated pushbuffer
    pb_addr = ContiguousResourceFromFile(self._xbox, "tmp/pb.bin")
    pb_addr.begin()
    print("pb at 0x%08X" % pb_addr.address())

    run_pushbuffer(self._xbox, pb_addr)

    # Free pushbuffer
    pb_addr.end()
    for tex_addr in tex_addrs:
      if tex_addr != None:
        tex_addr.end()

    # Report success
    print("Success!")
