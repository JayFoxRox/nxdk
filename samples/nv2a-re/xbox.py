# Hack to pretend we have a better API in xboxpy

import os

class Xbox():
  def __init__(self, host):

    os.environ["XBOX_IF"] = "nxdk-rdt"
    os.environ["XBOX"] = "%s:%d" % (host[0], host[1])
    import xboxpy

    self.read_u32 = xboxpy.read_u32
    self.read = xboxpy.read
    self.write = xboxpy.write
    self.write_u32 = xboxpy.write_u32
    self.ke = xboxpy.ke
    self.nxdk_rdt = xboxpy.interface.if_nxdk_rdt
