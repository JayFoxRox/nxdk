/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// Simple Decoder
// ==============
//
// This is an example of a simple decoder loop. It takes an input file
// containing the compressed data (in IVF format), passes it through the
// decoder, and writes the decompressed frames to disk. Other decoder
// examples build upon this one.
//
// The details of the IVF format have been elided from this example for
// simplicity of presentation, as IVF files will not generally be used by
// your application. In general, an IVF file consists of a file header,
// followed by a variable number of frames. Each frame consists of a frame
// header followed by a variable length payload. The length of the payload
// is specified in the first four bytes of the frame header. The payload is
// the raw compressed data.
//
// Standard Includes
// -----------------
// For decoders, you only have to include `vpx_decoder.h` and then any
// header files for the specific codecs you use. In this case, we're using
// vp8.
//
// Initializing The Codec
// ----------------------
// The libvpx decoder is initialized by the call to vpx_codec_dec_init().
// Determining the codec interface to use is handled by VpxVideoReader and the
// functions prefixed with vpx_video_reader_. Discussion of those functions is
// beyond the scope of this example, but the main gist is to open the input file
// and parse just enough of it to determine if it's a VPx file and which VPx
// codec is contained within the file.
// Note the NULL pointer passed to vpx_codec_dec_init(). We do that in this
// example because we want the algorithm to determine the stream configuration
// (width/height) and allocate memory automatically.
//
// Decoding A Frame
// ----------------
// Once the frame has been read into memory, it is decoded using the
// `vpx_codec_decode` function. The call takes a pointer to the data
// (`frame`) and the length of the data (`frame_size`). No application data
// is associated with the frame in this example, so the `user_priv`
// parameter is NULL. The `deadline` parameter is left at zero for this
// example. This parameter is generally only used when doing adaptive post
// processing.
//
// Codecs may produce a variable number of output frames for every call to
// `vpx_codec_decode`. These frames are retrieved by the
// `vpx_codec_get_frame` iterator function. The iterator variable `iter` is
// initialized to NULL each time `vpx_codec_decode` is called.
// `vpx_codec_get_frame` is called in a loop, returning a pointer to a
// decoded image or NULL to indicate the end of list.
//
// Processing The Decoded Data
// ---------------------------
// In this example, we simply write the encoded data to disk. It is
// important to honor the image's `stride` values.
//
// Cleanup
// -------
// The `vpx_codec_destroy` call frees any memory allocated by the codec.
//
// Error Handling
// --------------
// This example does not special case any error return codes. If there was
// an error, a descriptive message is printed and the program exits. With
// few exceptions, vpx_codec functions return an enumerated error status,
// with the value `0` indicating success.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>



#include "video_reader.h"

#include <vpx/vpx_decoder.h>
#include <vpx_config.h>
#include <vpx/vp8dx.h>

#include <hal/video.h>
//#include <SDL.h>
#include "libyuv_c.h"

#include <windows.h>

static const int SCREEN_WIDTH = 640;
static const int SCREEN_HEIGHT = 480;

#include <stdarg.h>

static const VpxInterface vpx_decoders[] = {
#if CONFIG_VP8_DECODER
  {"vp8", VP8_FOURCC, &vpx_codec_vp8_dx},
#endif

#if CONFIG_VP9_DECODER
  {"vp9", VP9_FOURCC, &vpx_codec_vp9_dx},
#endif
};

int get_vpx_decoder_count(void) {
  return sizeof(vpx_decoders) / sizeof(vpx_decoders[0]);
}

const VpxInterface *get_vpx_decoder_by_index(int i) {
  return &vpx_decoders[i];
}

#include <hal/debug.h>

// tools_common.c
#define LOG_ERROR(label) do {\
  const char *l = label;\
  va_list ap;\
  va_start(ap, fmt);\
  if (l)\
    fprintf(stderr, "%s: ", l);\
  vfprintf(stderr, fmt, ap);\
  fprintf(stderr, "\n");\
  va_end(ap);\
} while (0)

// tools_common.c
const VpxInterface *get_vpx_decoder_by_fourcc(uint32_t fourcc) {
  int i;

  for (i = 0; i < get_vpx_decoder_count(); ++i) {
    const VpxInterface *const decoder = get_vpx_decoder_by_index(i);
    if (decoder->fourcc == fourcc)
      return decoder;
  }

  return NULL;
}

// tools_common.c
void die(const char *fmt, ...) {
  LOG_ERROR(NULL);
  usage_exit();
}

// tools_common.c
void warn(const char *fmt, ...) {
  LOG_ERROR("Warning");
}

// tools_common.c
void die_codec(vpx_codec_ctx_t *ctx, const char *s) {
  const char *detail = vpx_codec_error_detail(ctx);

  printf("%s: %s\n", s, vpx_codec_error(ctx));
  if (detail)
    printf("    %s\n", detail);
  exit(EXIT_FAILURE);
}

extern "C" void usage_exit(void) {
  assert(false);
}

extern "C" int main(void) {
  int frame_cnt = 0;
  FILE *outfile = NULL;
  vpx_codec_ctx_t codec;
  VpxVideoReader *reader = NULL;
  const VpxInterface *decoder = NULL;
  const VpxVideoInfo *info = NULL;
  const char* input_path = "input.ivf";
#if 0
  const char* output_path = "output.bin";
#endif

  // Open log file for performance info
  freopen("log.txt", "wb", stdout);

  // We consume a lot of memory, so we need to claim the framebuffer
  XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
  size_t fb_size = 640 * 480 * 4;
  extern uint8_t* _fb;
  _fb = (uint8_t*)MmAllocateContiguousMemoryEx(fb_size, 0, 0xFFFFFFFF, 0x1000, PAGE_READWRITE | PAGE_WRITECOMBINE);
  memset(_fb, 0x00, fb_size);
#define PCRTC_START				0xFD600800
  *(unsigned int*)(PCRTC_START) = (unsigned int)_fb & 0x03FFFFFF;
  debugPrint("FB: 0x%X\n", _fb);

  reader = vpx_video_reader_open(input_path);
  if (!reader) die("Failed to open %s for reading.", input_path);

#if 0
  if (!(outfile = fopen(output_path, "wb")))
    die("Failed to open %s for writing.", output_path);
#endif

  info = vpx_video_reader_get_info(reader);

  decoder = get_vpx_decoder_by_fourcc(info->codec_fourcc);
  if (!decoder) die("Unknown input codec.");

  printf("Using %s\n", vpx_codec_iface_name(decoder->codec_interface()));

  if (vpx_codec_dec_init(&codec, decoder->codec_interface(), NULL, 0))
    die_codec(&codec, "Failed to initialize decoder.");

  // Prepare buffer for SDL format
  //FIXME: This is only necessary because the SDL YUV API sucks.
  //       We might also want to convert to some GPU compatible YUV format,
  //       instead of RGB.
  uint8_t* planes = (uint8_t*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 2);

  DWORD begin;
  int delta;

  while (true) {

    begin = GetTickCount();
    int ret = vpx_video_reader_read_frame(reader);
    if (!ret) {
        break;
    }
    delta = GetTickCount() - begin;
    printf("Reading took %d milliseconds\n", delta);

    // Already start loading the follow-up frame into memory (while we decode)
    begin = GetTickCount();
    debugPrint("--manual fetch\n");
    vpx_video_reader_prefetch_frame(reader);
    delta = GetTickCount() - begin;
    printf("Fetching took %d milliseconds\n", delta);

    begin = GetTickCount();
    vpx_codec_iter_t iter = NULL;
    size_t frame_size = 0;
    const unsigned char *frame =
        vpx_video_reader_get_frame(reader, &frame_size);
debugPrint("??? %p %d\n", frame, frame_size);

    if (vpx_codec_decode(&codec, frame, (unsigned int)frame_size, NULL, 100))
      die_codec(&codec, "Failed to decode frame.");
    delta = GetTickCount() - begin;
    printf("Decoding took %d milliseconds\n", delta);

    while (true) {

      begin = GetTickCount();
      vpx_image_t *img = vpx_codec_get_frame(&codec, &iter);
      if (img == NULL) {
          break;
      }
      delta = GetTickCount() - begin;
      printf("Getting frame took %d milliseconds\n", delta);

#if 0
      vpx_img_write(img, outfile);
#endif
      ++frame_cnt;

#if 1
      begin = GetTickCount();

      assert(img->x_chroma_shift == 1);
      assert(img->y_chroma_shift == 1);
      int width = img->d_w;
      int height = img->d_h;
      if (width > SCREEN_WIDTH) { width = SCREEN_WIDTH; }
      if (height > SCREEN_HEIGHT) { height = SCREEN_HEIGHT; }

      //FIXME: Might need SDL_PIXELFORMAT_YV12 etc. otherwise + convert below
      printf("0x%X\n", img->fmt);
      assert(img->fmt == VPX_IMG_FMT_I420);
#if 0
      Uint32 src_format = SDL_PIXELFORMAT_IYUV;

      //FIXME: Use SDL_SetYUVConversionMode otherwise
      assert(img->cs == VPX_CS_UNKNOWN || img->cs == VPX_CS_BT_709);
      printf("0x%X\n", img->cs);
      printf("%d %d\n", img->w, img->h);
      printf("%d %d\n", img->d_w, img->d_h);
      printf("%d %d\n", width, height);

      // Bring data into SDL format
      //FIXME: ~11ms (!)
      size_t y_size = img->stride[VPX_PLANE_Y] * height;
      size_t u_size = img->stride[VPX_PLANE_U] * ((height + 1) / 2);
      size_t v_size = img->stride[VPX_PLANE_V] * ((height + 1) / 2);
      size_t y_offset = 0;
      size_t u_offset = y_offset + y_size;
      size_t v_offset = u_offset + u_size;
      memcpy(&planes[y_offset], img->planes[VPX_PLANE_Y], y_size);
      memcpy(&planes[u_offset], img->planes[VPX_PLANE_U], u_size);
      memcpy(&planes[v_offset], img->planes[VPX_PLANE_V], v_size);
      const void* src = planes;
      delta = GetTickCount() - begin;
      printf("Padding took %d milliseconds\n", delta);

      int src_pitch = img->stride[VPX_PLANE_Y];

      Uint32 dst_format = SDL_PIXELFORMAT_RGB565;
      void* dst = XVideoGetFB();
      int dst_pitch = SCREEN_WIDTH * 2;

      // Convert SDL format to GPU format (directly into framebuffer)
      //FIXME: ~36ms (!)
      SDL_ConvertPixels(width, height,
                        src_format, src, src_pitch,
                        dst_format, dst, dst_pitch);

#endif

      unsigned long long beginu = __builtin_ia32_rdtsc();
#if 1
      // 4-5ms
      libyuv::I420ToARGB_(img->planes[VPX_PLANE_Y], img->stride[VPX_PLANE_Y],
                         img->planes[VPX_PLANE_U], img->stride[VPX_PLANE_U],
                         img->planes[VPX_PLANE_V], img->stride[VPX_PLANE_V],
                         _fb, SCREEN_WIDTH * 4,
                         width, height);
#endif
#if 0
      // 4-5ms [same as non-flipped]
      libyuv::I420ToARGB_(img->planes[VPX_PLANE_Y], img->stride[VPX_PLANE_Y],
                         img->planes[VPX_PLANE_U], img->stride[VPX_PLANE_U],
                         img->planes[VPX_PLANE_V], img->stride[VPX_PLANE_V],
                         _fb, SCREEN_WIDTH * 4,
                         width, -height);
#endif
#if 0
      // 5-6ms
      libyuv::I420ToRGB565_(img->planes[VPX_PLANE_Y], img->stride[VPX_PLANE_Y],
                           img->planes[VPX_PLANE_U], img->stride[VPX_PLANE_U],
                           img->planes[VPX_PLANE_V], img->stride[VPX_PLANE_V],
                           _fb, SCREEN_WIDTH * 2,
                           width, height);
#endif
#if 0
      libyuv::I420ToARGB1555_(img->planes[VPX_PLANE_Y], img->stride[VPX_PLANE_Y],
                             img->planes[VPX_PLANE_U], img->stride[VPX_PLANE_U],
                             img->planes[VPX_PLANE_V], img->stride[VPX_PLANE_V],
                             _fb, SCREEN_WIDTH * 2,
                             width, height);
#endif
#if 0
      // Not as framebuffer, but acceptable speed
      libyuv::I420ToARGB4444_(img->planes[VPX_PLANE_Y], img->stride[VPX_PLANE_Y],
                              img->planes[VPX_PLANE_U], img->stride[VPX_PLANE_U],
                              img->planes[VPX_PLANE_V], img->stride[VPX_PLANE_V],
                              _fb, SCREEN_WIDTH * 2,
                              width, height);
#endif
#if 0
      // This is an Xbox GPU compatible format (less CPU intense).
      //
      // CR8YB8CB8YA8 = UYVY [420?]; 16bpp = libyuv::I420ToUYVY
      // YB8CR8YA8CB8 = YUYV [420?]; 16bpp = libyuv::I420ToYUY2
      // A8CR8CB8Y8 = AUVY [444?]; 32bpp = ???
      //
      //FIXME: Optimize these
      libyuv::I420ToYUY2(img->planes[VPX_PLANE_Y], img->stride[VPX_PLANE_Y],
                         img->planes[VPX_PLANE_U], img->stride[VPX_PLANE_U],
                         img->planes[VPX_PLANE_V], img->stride[VPX_PLANE_V],
                         gpu_buffer, width * 2,
                         width, height);
#endif
#if 0
      // Alternatively, you can also just move all 3 planes (Y,U,V) into the
      // GPU separately, then do all of the RGB conversion in register combiner
      // or texture shader.
#endif
      unsigned long long endu = __builtin_ia32_rdtsc();
      delta = GetTickCount() - begin;

      unsigned long long deltau = endu - beginu;
      unsigned long long tu = (deltau * 1000000000ULL) / 733333333ULL;
      printf("Conversion [libyuv] took %llu cycles [%llu.%06llu ms]\n", deltau, tu / 1000000UL, tu % 1000000UL);

      printf("Conversion (incl. padding) took %d milliseconds\n", delta);
#endif
    }
  }

  free(planes);

  printf("Processed %d frames.\n", frame_cnt);
  if (vpx_codec_destroy(&codec)) die_codec(&codec, "Failed to destroy codec");

#if 0
  printf("Play: ffplay -f rawvideo -pix_fmt yuv420p -s %dx%d %s\n",
         info->frame_width, info->frame_height, output_path);
#endif

  vpx_video_reader_close(reader);

#if 0
  fclose(outfile);
#endif

  return EXIT_SUCCESS;
}
