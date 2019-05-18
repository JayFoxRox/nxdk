// This is a debug extension to be used with XBDM on a debug Xbox kernel
//
// Can be installed in E:/dxt and will be auto-loaded at startup.
// You can communicate with this DXT through telnet.
// Connect to your Xbox debug IP on port 731.
// It will respond to "hello!" commands via XBDM.
//
// Warning:
//
// - Raw and Virtual address in binary must be the same.
// - Must have an import table.
//   - If an import fails, loading fails.
//   - If an import has name and ordinal, the name is used (will likely fail).
// - This doesn't use the CRT0, so constructors won't run.
// - TLS (Thread local storage) won't work.
//


#include <assert.h>
#include <string.h>

#include <xboxkrnl/xboxkrnl.h>
#include <winapi/winbase.h>
#include <xbdm/xbdm.h>

// Dummy, so the CRT0 doesn't bail
// We use this opportunity to force at least 1 import
int main() { (void)KeTickCount; return 0; }

static HRESULT __stdcall command_processor(LPCSTR szCommand, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc) {
#if 0
  // Echo the command as response
  assert(cchResp >= (strlen(szCommand) + 1));
  strcpy(szResp, szCommand);

  //FIXME: Show how to use pdmcc

#endif
szResp[0] = 'X';
szResp[1] = '\0';

  // Valid return values include:
  // - 0
  // - E_FAIL
  // - XBDM_DEDICATED
  // - XBDM_NOERR
  // - XBDM_INVALIDCMD
  return XBDM_NOERR;
}

void DxtEntry(ULONG *pfUnload) {

  OutputDebugString("Hello World via debugger!");

  // Send a startup message
  HRESULT ret = DmSendNotificationString("Hello World via notification!");
  //assert(ret == 0); //FIXME: Might give XBDM_NOERR?

  // Set up a command processor
  DmRegisterCommandProcessor("HELLO", command_processor);

}
