#ifndef __WINNT_H__
#define __WINNT_H__

#include <xboxkrnl/xboxnt.h>

typedef CHAR *LPSTR;

#ifdef UNICODE
typedef LPCWSTR LPCTSTR; 
#else
typedef LPCSTR LPCTSTR;
#endif

#endif
