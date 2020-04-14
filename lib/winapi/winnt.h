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

#endif
