typedef unsigned int DWORD;

#define GetTickCount() 0


#define NtClose(...) 0
#define NtCreateEvent(...) 0
#define NtSetEvent(...) 0
#define NtResetEvent(...) 0

#define STATUS_SUCCESS 0

typedef struct {
  long long QuadPart;
} LARGE_INTEGER;

typedef DWORD NTSTATUS;
typedef DWORD HANDLE;

typedef struct {
  NTSTATUS Status;
  int Information;
} IO_STATUS_BLOCK;
