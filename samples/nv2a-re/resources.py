import globals

import filewatch

#FIXME: Remove this type?
class Args():
  def __init__(self, args):
    self._args = args
  def args(self):
    return self._args

class BaseContiguousResource(Args):
  def address(self):
    return self._memory
  def size(self):
    return self._size
  def args(self):
    return ["%d" % self._memory]

class ContiguousResourceFromBytes(BaseContiguousResource):
  def __init__(self, xbox, data):
    self._xbox = xbox
    self._data = data
    self._size = len(self._data)
    self._memory = None
  def _begin(self, new_data):
    new_size = len(new_data)
    #FIXME: Only relocate if old memory too small or non-existing
    self._memory = self._xbox.ke.MmAllocateContiguousMemoryEx(new_size, 0x00000000, 0x7FFFFFFF, 0x1000, self._xbox.ke.PAGE_READWRITE | self._xbox.ke.PAGE_NOCACHE)
    if (self._memory == 0):
      self._memory = None
      assert(False)
    #FIXME: Only reupload if old memory is different
    self._xbox.write(self._memory, new_data)
    self._data = new_data
    self._size = new_size
  def _free(self):
    if self._memory != None:
      self._xbox.ke.MmFreeContiguousMemory(self._memory)
    self._data = bytes([])
    self._size = 0
  def begin(self):
    self._begin(self._data)
  def end(self):
    self._free()
  def update(self, data):
    self._free()
    self._begin(data)

class ContiguousResourceFromFile(BaseContiguousResource):
  def _contents(self):
    return open(globals.BASE_PATH + "/" + self._path, "rb").read()
  def __init__(self, xbox, path):
    self._xbox = xbox
    self._path = path
    self._resource = ContiguousResourceFromBytes(xbox, self._contents())
    self._handler = None
  def _update(self):
    self._resource.update(self._contents())
  def begin(self):
    self._resource.begin()
    self._handler = filewatch.register_handler(globals.BASE_PATH + "/" + self._path, self._update)
  def end(self):
    filewatch.unregister_handler(self._handler)
    self._resource.end()
  @property
  def _memory(self):
    return self._resource._memory
  @property
  def _size(self):
    return self._resource._size


class BaseTask():

  def __init__(self):
    self.resources = []

  def register(self, resource):
    self.resources += [resource]
    return resource

  def end(self):
    for resource in resources:
      resource.end()
  def begin(self):
    for resource in resources:
      resource.begin()
  def force_update(self):
    for resource in resources:
      resource.force_update()  
