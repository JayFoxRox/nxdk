#pragma once

#include <stdlib.h>

#include <xboxrt/debug.h>


#include <hal/fileio.h>
#include <string.h>
#include <stdlib.h>

#define SEEK_CUR 0
#define SEEK_SET 1
#define SEEK_END 2

typedef struct {
  int handle;
} FILE;

static int fwrite(const void* buffer, int chunk_count, int chunk_size, FILE* f) {
  unsigned int numberOfBytesWritten;
  XWriteFile(f->handle, (void*)buffer, chunk_count * chunk_size, &numberOfBytesWritten);
  return numberOfBytesWritten / chunk_size;
}

static long ftell(FILE *stream) {
  int newFilePointer;
  int r = XSetFilePointer(stream->handle, 0, &newFilePointer,	FILE_CURRENT);
  return newFilePointer;
}

static int fseek(FILE* f, int offset, int whence) {
  int moveMethod;
  switch(whence) {
  case SEEK_SET:
    break;
  case SEEK_CUR:
    offset += ftell(f);
    break;
  case SEEK_END: {
    // We can't use FILE_END from XSetFilePointer due to bugs; do our own thing
    unsigned int filesize;
    NTSTATUS status = XGetFileSize(f->handle, &filesize);
    if (status != TRUE) {
      debugPrint("Unable to get file-size for fseek\n");
      while(1);
    }
    offset += filesize;
    break;
  }
  default:
    debugPrint("Bad whence: %d\n", whence);
    while(1);
  }

  int newFilePointer;
  XSetFilePointer(f->handle, offset, &newFilePointer, FILE_BEGIN);

  return 0;
}

static FILE* fopen(char* path, char* mode) {
  int handle;

  int create;
  int access;
  int whence;
  if (!strcmp(mode, "rb")) {
    create = OPEN_EXISTING;
    access = GENERIC_READ;
    whence = SEEK_SET;
  } else if (!strcmp(mode, "wb")) {
    create = CREATE_ALWAYS;
    access = GENERIC_WRITE;
    whence = SEEK_SET;
  } else if (!strcmp(mode, "ab")) {
    create = OPEN_ALWAYS;
    access = GENERIC_READ | GENERIC_WRITE;
    whence = SEEK_END;
  } else {
    return NULL;
  }

  NTSTATUS status = XCreateFile(&handle, path, access, 0, create, 0);

  // This is technically not an error for any modes we support
  if (status == ERROR_ALREADY_EXISTS) {
    status = STATUS_SUCCESS;
  }

  // Error out if no file was loaded
  if (status != STATUS_SUCCESS) {
    return NULL;
  }

  // Create a FILE object
  FILE* f = malloc(sizeof(FILE));
  f->handle = handle;

  // Go to the intended location within file
  fseek(f, 0, whence);

  return f;
}

static int fclose(FILE* f) {
  XCloseHandle(f->handle);
  free(f);
  return 0;
}

static int vprintf(const char *format, va_list ap) { //FIXME: Should be in stdarg.h
  char buf[4096];
  //sprintf(buf, "%s", format);
  int r = vsprintf(buf, format, ap);
  //debugPrint(buf);
  FILE* f = fopen("log.txt", "ab");
  fwrite(buf, strlen(buf), 1, f);
  fclose(f);
  return r;
}

static int printf(const char *format, ...) {
  va_list args;
  va_start (args, format);
  int r = vprintf(format, args);
  va_end (args);
  return r;
}

static int fread(void* buffer, int chunk_size, int chunk_count, FILE* f) {
  unsigned int numberOfBytesRead = 1337;
  int r = XReadFile(f->handle, buffer, chunk_count * chunk_size, &numberOfBytesRead);
  if (r != TRUE) {
    // Assume 0xC0000011, thanks obama!
    printf("Read failed %d / %d\n", numberOfBytesRead, chunk_count * chunk_size);
    return 0;
  }
  if (numberOfBytesRead != chunk_count * chunk_size) {
    printf("Read too few bytes %d / %d\n", numberOfBytesRead,  chunk_count * chunk_size);
    //while(1);
  }
  return numberOfBytesRead / chunk_size;
}

static int fprintf(FILE *stream, const char *format, ...) {
  char buf[4096];
  va_list args;
  va_start (args, format);
  int r = vsprintf(buf, format, args);
  va_end (args);
  fwrite(buf, strlen(buf), 1, stream);
  return r;
}

static FILE* stderr = NULL;
static FILE* stdout = NULL;

static void perror(const char *s) {
}


