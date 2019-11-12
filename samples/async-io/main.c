#include <xboxkrnl/xboxkrnl.h>
#include <hal/debug.h>
#include <hal/fileio.h>
#include <hal/video.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#define BUFFER_SIZE 20*1024*1024

 __attribute__((section("pad")))
struct {
  int start;
  char x[BUFFER_SIZE];
  int end;
} data = {1, {}, 1};

int main(void) {

  FscSetCacheSize(1); // 1 Page

  //FIXME: Just mark section as non-preload
  //XeUnloadSection(FindSection("pad"));

  // We consume a lot of memory, so we need to claim the framebuffer
  XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
  size_t fb_size = 640 * 480 * 4;
  extern char* _fb;
  _fb = MmAllocateContiguousMemoryEx(fb_size, 0, 0xFFFFFFFF, 0x1000, PAGE_READWRITE | PAGE_WRITECOMBINE);
  memset(_fb, 0x00, fb_size);
#define PCRTC_START				0xFD600800
  *(unsigned int*)(PCRTC_START) = (unsigned int)_fb & 0x03FFFFFF;
  debugPrint("FB: 0x%X\n", _fb);

  // Copy temp file to disk
  FILE* f = fopen("C:/tmp.bin", "wb");
  assert(f != NULL);
  fseek(f, BUFFER_SIZE, SEEK_SET);
  fputc('X', f);
  fclose(f);
  debugPrint("Done!\n");
 

  HANDLE file;
  ANSI_STRING path;
  OBJECT_ATTRIBUTES attributes;
  IO_STATUS_BLOCK ioStatusBlock;
  NTSTATUS status;

#if 0
  // PENDING = Okay

  RtlInitAnsiString(&path, "\\Device\\Harddisk0\\Partition0");
  InitializeObjectAttributes(&attributes, &path, OBJ_INHERIT, NULL, NULL);
#endif
#if 0
  // PENDING = Okay

  RtlInitAnsiString(&path, "\\Device\\CdRom0");
  InitializeObjectAttributes(&attributes, &path, OBJ_INHERIT, NULL, NULL);
#endif
#if 1
  // Blocking SUCCESS [WHY?!]

  char tmp[MAX_PATH];
  int rc = XConvertDOSFilenameToXBOX("C:/tmp.bin", tmp);
  if (rc != STATUS_SUCCESS)
    return NULL;

  assert(strlen(tmp) < MAX_PATH);
  RtlInitAnsiString(&path, tmp);
  debugPrint("-> '%s'\n", tmp);

  InitializeObjectAttributes(&attributes, &path, OBJ_CASE_INSENSITIVE, NULL, NULL);
#endif
#if 0
  // Blocking SUCCESS [WHY?!]

  char tmp[MAX_PATH];
  int rc = XConvertDOSFilenameToXBOX("default.xbe", tmp);
  if (rc != STATUS_SUCCESS)
    return NULL;

  assert(strlen(tmp) < MAX_PATH);
  RtlInitAnsiString(&path, tmp);
  debugPrint("-> '%s'\n", tmp);

  InitializeObjectAttributes(&attributes, &path, OBJ_CASE_INSENSITIVE, NULL, NULL);
#endif
#if 0
  // Blocking SUCCESS [WHY?!]

  OBJECT_STRING s = RTL_CONSTANT_STRING("\\??\\D:");
  OBJECT_STRING d = RTL_CONSTANT_STRING("\\Device\\CdRom0");
  status = IoCreateSymbolicLink(&s, &d);
  debugPrint("0x%08X\n", status);
  assert(status == STATUS_SUCCESS);

  RtlInitAnsiString(&path, "D:\\default.xbe");
  InitializeObjectAttributes(&attributes, &path, OBJ_CASE_INSENSITIVE, ObDosDevicesDirectory(), NULL);
#endif

  PHANDLE            FileHandle = &file;
  ACCESS_MASK        DesiredAccess = SYNCHRONIZE | FILE_READ_ATTRIBUTES | FILE_READ_DATA;
  POBJECT_ATTRIBUTES ObjectAttributes = &attributes;
  PIO_STATUS_BLOCK   IoStatusBlock = &ioStatusBlock;
  ULONG              ShareAccess = 0; //FILE_SHARE_READ;

  // Async won't work without `FILE_NO_INTERMEDIATE_BUFFERING`.
  // See https://support.microsoft.com/en-us/help/156932/asynchronous-disk-i-o-appears-as-synchronous-on-windows
  // However, that forces offset / size alignment of `alignment.AlignmentRequirement`:
  //
  //   FILE_ALIGNMENT_INFORMATION alignment;
  //   status = NtQueryInformationFile(device, &ioStatusBlock, &alignment, sizeof(alignment), FileAlignmentInformation);
  //   assert(status == STATUS_SUCCESS);
  //   assert(ioStatusBlock.Status == STATUS_SUCCESS);
  //   assert(ioStatusBlock.Information == sizeof(alignment));
  //
  // For DVDs, this should be 2048 bytes.
  // For HDDs, this should be 512 bytes.
  //
  // For devices / volumes, `FILE_NO_INTERMEDIATE_BUFFERING` is always set implicitly.

  ULONG              CreateOptions = FILE_NON_DIRECTORY_FILE | FILE_NO_INTERMEDIATE_BUFFERING;

#if 1
  PLARGE_INTEGER     AllocationSize = NULL; // XXX
  ULONG              FileAttributes = 0; // XXX
  ULONG              CreateDisposition = FILE_OPEN; // XXX

  status = NtCreateFile(
                FileHandle,
                DesiredAccess,
                ObjectAttributes,
                IoStatusBlock,
                AllocationSize,
                FileAttributes,
                ShareAccess,
                CreateDisposition,
                CreateOptions
                );
#else
  status = NtOpenFile(FileHandle,
                      DesiredAccess,
                      ObjectAttributes,
                      IoStatusBlock,
                      ShareAccess,
                      CreateOptions);
#endif

  debugPrint("0x%08X\n", status);
  assert(status == STATUS_SUCCESS);

  if (ioStatusBlock.Information != FILE_OPENED) {
    assert(0);
    return 1;  // Can't open file
  }

  // Read much data
  char*  buffer = malloc(BUFFER_SIZE);

  LARGE_INTEGER offset;
  offset.QuadPart = 0;

  debugPrint("Starting read\n");
  DWORD begin = GetTickCount();


  // Note: Will not signal `file` if event is passed!
  status = NtReadFile(file, NULL, NULL, NULL, &ioStatusBlock,
                      buffer, BUFFER_SIZE, &offset);


  DWORD end = GetTickCount();
  int read_duration_ms = end - begin;

  debugPrint("STATUS?! 0x%08X / 0x%08X / %d\n", status, ioStatusBlock.Status, ioStatusBlock.Information);

  int wait_duration_ms = 0;
  if (status == STATUS_SUCCESS) {
    debugPrint("SUCCESS?!\n");

  } else if (status == STATUS_PENDING) {
    debugPrint("PENDING?!\n");

    DWORD begin = GetTickCount();
    status = NtWaitForSingleObject(file, FALSE, NULL);
    DWORD end = GetTickCount();
    wait_duration_ms = end - begin;

    debugPrint("0x%08X\n", status);
    assert(status == STATUS_SUCCESS);

  } else {
    debugPrint("UNKNOWN?!\n");
  }

  debugPrint("-- 0x%08X / %d bytes [%d + %d ms]\n", ioStatusBlock.Status, ioStatusBlock.Information, read_duration_ms, wait_duration_ms);

  if ((ioStatusBlock.Status != STATUS_SUCCESS) || (ioStatusBlock.Information != BUFFER_SIZE)) {

    // Can't read file
    assert(0);

    return 1;
  }

  // Don't block more than 5ms
  assert(read_duration_ms < 5);

  while(1);

  return 0;
  
}
