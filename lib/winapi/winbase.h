#ifndef __WINBASE_H__
#define __WINBASE_H__

#include <windef.h>
#include <winnt.h>

#ifdef __cplusplus
extern "C"
{
#endif

DWORD GetLastError (void);
void SetLastError (DWORD error);

void WINAPI OutputDebugStringA (LPCTSTR lpOutputString);

#ifndef UNICODE
#define OutputDebugString OutputDebugStringA
#else
#error nxdk does not support the Unicode API
#endif

#ifdef __cplusplus
}
#endif

#endif
