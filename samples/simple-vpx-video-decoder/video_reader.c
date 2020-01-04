/*
 *  Copyright (c) 2014 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#define SYNC 0

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <windows.h>
#include <hal/debug.h> //FIXME: Remove

#include "video_reader.h"

//FIXME: Inline
#include "vpx_ports/mem_ops.h"

static const char *const kIVFSignature = "DKIF";

#if SYNC
typedef FILE* IVF_FILE;
#else
typedef HANDLE IVF_FILE;
#endif

struct VpxVideoReaderStruct {
  VpxVideoInfo info;
  IVF_FILE file;
  size_t frame_size;

  IO_STATUS_BLOCK prefetch_state;
  LARGE_INTEGER prefetch_offset;

  uint8_t *buffer[2];
  size_t buffer_size[2];
  size_t buffer_current_size[2];
  unsigned int prefetch_idx;
};

static void prefetch(VpxVideoReader* reader, size_t frame_size) {
  assert(reader->buffer_current_size[reader->prefetch_idx] == 0);

  if (frame_size > 256 * 1024 * 1024) {
    warn("Read invalid data size (%u)\n", (unsigned int)frame_size);
    frame_size = 0;
  }

  size_t size = frame_size + IVF_FRAME_HDR_SZ;

  //FIXME: Make sure that buffer[prefetch_idx] is large enough
  if (size > reader->buffer_size[reader->prefetch_idx]) {
    size_t new_size = 2 * size;
    uint8_t *new_buffer = realloc(reader->buffer[reader->prefetch_idx], new_size);

    if (new_buffer) {
      reader->buffer[reader->prefetch_idx] = new_buffer;
      reader->buffer_size[reader->prefetch_idx] = new_size;
    } else {
      warn("Failed to allocate compressed data buffer\n");
    }
  }

  reader->buffer_current_size[reader->prefetch_idx] = size;

#if SYNC
  // Load data into buffer
  fseek(reader->file, reader->prefetch_offset.QuadPart, SEEK_SET);
  reader->prefetch_state.Information = fread(reader->buffer[reader->prefetch_idx], 1, size, reader->file);
  reader->prefetch_state.Status = STATUS_SUCCESS;
#else
  NTSTATUS status;

  // Begin streaming
  debugPrint("streaming %d bytes from %llu\n", size, reader->prefetch_offset.QuadPart);
  reader->prefetch_state.Status = (DWORD)STATUS_PENDING;
  status = NtReadFile(
    reader->file,
    NULL,
    NULL,
    NULL,
    &reader->prefetch_state,
    reader->buffer[reader->prefetch_idx],
    size,
    &reader->prefetch_offset
  );
  //FIXME: Never STATUS_PENDING?! Why?! All this work for nothing?
  assert(status == STATUS_SUCCESS);
  //Sleep(1000);
#endif
}

VpxVideoReader *vpx_video_reader_open(const char *filename) {
  uint8_t header[32];
  VpxVideoReader *reader = NULL;
#if SYNC
  IVF_FILE file = fopen(filename, "rb");
  if (!file)
    return NULL;  // Can't open file

  if (fread(header, 1, 32, file) != 32) {
    fclose(f);
    return NULL;  // Can't read file header
  }
#else
  IVF_FILE file;

  ANSI_STRING path;
  OBJECT_ATTRIBUTES attributes;

  char tmp[MAX_PATH];
  int rc = XConvertDOSFilenameToXBOX(filename, tmp);
  if (rc != STATUS_SUCCESS)
    return NULL;

  assert(strlen(tmp) < MAX_PATH);
  RtlInitAnsiString(&path, tmp);
  debugPrint("'%s' -> '%s'\n", filename, tmp);

  InitializeObjectAttributes(&attributes, &path, OBJ_CASE_INSENSITIVE, ObDosDevicesDirectory(), NULL);
  attributes.RootDirectory = NULL; //FIXME: Hack

  IO_STATUS_BLOCK ioStatusBlock;

  NTSTATUS status;

  status = NtOpenFile(&file, FILE_GENERIC_READ, &attributes, &ioStatusBlock,
                      FILE_SHARE_READ, FILE_NON_DIRECTORY_FILE);
  debugPrint("0x%08X\n", status);
  assert(status == STATUS_SUCCESS);

  if (ioStatusBlock.Information != FILE_OPENED) {
    return NULL;  // Can't open file
  }

  // Read header
  LARGE_INTEGER offset = {0};
  status = NtReadFile(file, NULL, NULL, NULL, &ioStatusBlock,
                      header, 32, &offset);
  debugPrint("0x%08X\n", status);
  assert(status == STATUS_SUCCESS);

//FIXME: Only if STATUS_PENDING
#if 0
  //FIXME: Does this need a timeout to work?
  status = NtWaitForSingleObject(reader->file, FALSE, NULL);
  debugPrint("0x%08X\n", status);
  assert(status == STATUS_SUCCESS);
#endif

  if ((ioStatusBlock.Status != STATUS_SUCCESS) || (ioStatusBlock.Information != 32)) {
    debugPrint("0x%08X / %d\n", ioStatusBlock.Status, ioStatusBlock.Information);
    assert(0);
    NtClose(file);
    return NULL;  // Can't read file header
  }
  
#endif

  if (memcmp(kIVFSignature, header, 4) != 0)
    //FIXME: Close file
    return NULL;  // Wrong IVF signature

  if (mem_get_le16(header + 4) != 0)
    //FIXME: Close file
    return NULL;  // Wrong IVF version

  reader = calloc(1, sizeof(*reader));
  if (!reader)
    //FIXME: Close file
    return NULL;  // Can't allocate VpxVideoReader

  reader->file = file;
  reader->info.codec_fourcc = mem_get_le32(header + 8);
  reader->info.frame_width = mem_get_le16(header + 12);
  reader->info.frame_height = mem_get_le16(header + 14);
  reader->info.time_base.numerator = mem_get_le32(header + 16);
  reader->info.time_base.denominator = mem_get_le32(header + 20);

  reader->frame_size = 0;

  reader->prefetch_offset.QuadPart = 32;

  reader->buffer[0] = NULL;
  reader->buffer[1] = NULL;
  reader->buffer_size[0] = 0;
  reader->buffer_size[1] = 0;
  reader->buffer_current_size[0] = 0;
  reader->buffer_current_size[1] = 0;
  reader->prefetch_idx = 0;

  // Fetch first header
  prefetch(reader, 0);

  // Parse first header
  vpx_video_reader_read_frame(reader);

  return reader;
}

void vpx_video_reader_close(VpxVideoReader *reader) {
  if (reader) {
#if SYNC
    fclose(reader->file);
#else
    //FIXME: Abort the fetch if one is still running
    NtClose(reader->file);
#endif
    free(reader->buffer[0]);
    free(reader->buffer[1]);
    free(reader);
  }
}

int vpx_video_reader_read_frame(VpxVideoReader *reader) {

  // Ensure that we have prefetched
  if (!vpx_video_reader_prefetch_frame(reader)) {
    return 0;
  }

#if SYNC
#else
  NTSTATUS status;

  //FIXME: Only if status was STATUS_PENDING !

  // Wait for prefetch result
  status = NtWaitForSingleObject(reader->file, FALSE, NULL);
  assert(status == STATUS_SUCCESS);
#endif

  // Check status
  assert(reader->prefetch_state.Status == STATUS_SUCCESS);
//FIXME: Check for this?
#if 0
  if (!feof(infile)) {
    return 0;
  }
#endif

  // Advance in file
  reader->prefetch_offset.QuadPart += reader->prefetch_state.Information;

  // Extract header data from metadata
  // (also in reader->buffer[!reader->prefetch_idx])
  assert(reader->buffer_current_size[reader->prefetch_idx] >= IVF_FRAME_HDR_SZ);
  reader->frame_size = reader->buffer_current_size[reader->prefetch_idx] - IVF_FRAME_HDR_SZ;

  // Store how much data we ended up with
  assert(reader->prefetch_state.Information <= reader->buffer_current_size[reader->prefetch_idx]);
  reader->buffer_current_size[reader->prefetch_idx] = reader->prefetch_state.Information;

  // Check frame completness
  if (reader->buffer_current_size[reader->prefetch_idx] < reader->frame_size) {
    warn("Failed to read full frame\n");
    reader->frame_size = 0; //FIXME: Breaks logic?
    reader->buffer_current_size[reader->prefetch_idx] = 0;
    return 0;
  }

  // Go to other buffer
  reader->prefetch_idx = !reader->prefetch_idx;
  reader->buffer_current_size[reader->prefetch_idx] = 0;

  return 1;
}

int vpx_video_reader_prefetch_frame(VpxVideoReader *reader) {

  // Ignore when fetching multiple times
  if (reader->buffer_current_size[reader->prefetch_idx] > 0) {
    return 1;
  }

  // Find header after current frame
  assert(reader->buffer_current_size[!reader->prefetch_idx] >= reader->frame_size);
  uint8_t* buffer = reader->buffer[!reader->prefetch_idx];
  uint8_t* raw_header = &buffer[reader->frame_size];
  if ((reader->buffer_current_size[!reader->prefetch_idx] - reader->frame_size) < IVF_FRAME_HDR_SZ) {
    warn("No header, won't prefetch\n");
    return 0;
  }

  // Load next frame and the header to follow it
  size_t frame_size = mem_get_le32(raw_header);
  prefetch(reader, frame_size);

  return 1;
}

const uint8_t *vpx_video_reader_get_frame(VpxVideoReader *reader,
                                          size_t *size) {
  if (size)
    *size = reader->frame_size;

  return reader->buffer[!reader->prefetch_idx];
}

const VpxVideoInfo *vpx_video_reader_get_info(VpxVideoReader *reader) {
  return &reader->info;
}

