#ifndef __WINNT_H__
#define __WINNT_H__

#include <xboxkrnl/xboxdef.h>

typedef LONG HRESULT;
typedef signed __int64 LONG64, *PLONG64;

typedef CHAR *LPSTR;

#ifdef UNICODE
typedef LPCWSTR LPCTSTR;
#else
typedef LPCSTR LPCTSTR;
#endif

static LONG64 InterlockedExchange64(LONG64 volatile *Target, LONG64 Value)
{
    return __atomic_exchange_n(Target, Value, __ATOMIC_ACQ_REL);
}

static PVOID InterlockedExchangePointer(PVOID volatile *Target, PVOID Value)
{
    return __atomic_exchange_n(Target, Value, __ATOMIC_ACQ_REL);
}

static PVOID InterlockedCompareExchangePointer(PVOID volatile *Destination, PVOID Exchange, PVOID Comperand)
{
  PVOID hack = *Destination; //FIXME: This isn't atomic
  int result = __atomic_compare_exchange_n (Destination, Comperand, Exchange, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
  return result ? Comperand : hack;
}


#endif
