#define XBDM_NOERR 0x2db << 16 //FIXME: !!!

typedef void* PDMN_MODLOAD; //FIXME: !!!
typedef void* PDMN_SESSION; //FIXME: !!!
typedef void* PDM_NOTIFY_FUNCTION; //FIXME: !!!

#define DM_PERSISTENT 1

#define DM_MODLOAD 5

typedef struct _DM_CMDCONT* PDM_CMDCONT;

typedef HRESULT (__stdcall *PDM_CMDCONTPROC)(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse);

typedef struct {
  PDM_CMDCONTPROC HandlingFunction;
  DWORD DataSize;
  PVOID Buffer;
  DWORD BufferSize;
  PVOID CustomData;
  DWORD BytesRemaining;
} DM_CMDCONT;

typedef HRESULT (__stdcall *PDM_CMDPROC)(LPCSTR szCommand, LPSTR szResponse, DWORD cchResponse, PDM_CMDCONT pdmcc);

typedef struct _DM_COUNTDATA {
  LARGE_INTEGER CountValue;
  LARGE_INTEGER RateValue;
  DWORD CountType;
} DM_COUNTDATA, *PDM_COUNTDATA;

#define DMAPI __declspec(dllimport)

DMAPI HRESULT NTAPI DmClosePerformanceCounter(HANDLE hCounter);
DMAPI HRESULT NTAPI DmNotify(PDMN_SESSION Session, DWORD dwNotification, PDM_NOTIFY_FUNCTION pfnHandler);
DMAPI HRESULT NTAPI DmOpenNotificationSession(DWORD dwFlags, PDMN_SESSION *pSession);
DMAPI HRESULT NTAPI DmOpenPerformanceCounter(LPCSTR szName, HANDLE *phCounter);
DMAPI HRESULT NTAPI DmQueryPerformanceCounterHandle(HANDLE hCounter, DWORD dwType, PDM_COUNTDATA);
DMAPI HRESULT NTAPI DmRegisterCommandProcessor(LPCSTR szProcessor, PDM_CMDPROC pfn);
DMAPI HRESULT NTAPI DmSendNotificationString(LPCSTR sz);




