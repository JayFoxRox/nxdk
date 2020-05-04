#include <windows.h>

BOOL IsBadWritePtr(LPVOID lp, UINT_PTR ucb)
{
  //FIXME: Run NtQueryVirtualMemory for the pages?

  /* We disallow all access for now, because memory is potentially unsafe */
  return TRUE;
}
