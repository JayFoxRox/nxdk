#include <SDL.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#if defined(NXDK)
#include <windows.h>
#endif

#if defined(NXDK)
#include <hal/video.h>
#endif

#if defined(NXDK)
#include <hal/debug.h>
#else
#include <stdio.h>
#define debugClearScreen() { printf("\n\n"); }
#define debugPrint(...) printf(__VA_ARGS__)
#endif

#include "log.inl"

double cursor1 = 0.0;
double cursor2 = 0.0;
double volume1 = 0.9;
double volume2 = 1.0;
double freq1 = 10666.666;
double freq2 = 0.0;
bool update = false;

SDL_AudioSpec have;

void MyAudioCallback(void*  userdata,
                       Uint8* stream,
                       int    len) {
  debugPrint("callback for %d bytes\n", len);

  unsigned int sdl_i = add_log("SDL Duration");

  // Clear buffer, as we might not fill it all
  memset(stream, have.silence, len);

  float* f32 = stream;
  uint32_t* s32 = stream;
  uint16_t* u16 = stream;
  int16_t* s16 = stream;
  uint8_t* u8 = stream;
  int8_t* s8 = stream;

  unsigned int i = 0;
  unsigned int skip = 0; //32; // Leave bytes to mark end of buffer
  while(len > skip) { 
    double signal1 = cos(cursor1 * 2.0 * M_PI);
    double signal2 = cos(cursor2 * 2.0 * M_PI);
    double signal = signal1 * volume1 * signal2 * volume2;
    static uint32_t steady = 0;
    for(int channel = 0; channel < have.channels; channel++) {
#if 0
      if (channel > 0) { signal = update ? 1.0f : -1.0f; }
      if (channel > 0) { signal = cos(cursor1 * 2.0 * M_PI); }
#endif
#if 1
      switch(have.format) {
      case AUDIO_F32: f32[i++] = signal; len -= 4; break;
      case AUDIO_S32: s32[i++] = signal * 0x7FFFFFFF; len -= 4; break;
      case AUDIO_U16: u16[i++] = signal * 0x7FFF + 0x8000; len -= 2; break;
      case AUDIO_S16: s16[i++] = signal * 0x7FFF; len -= 2; break;
      case AUDIO_U8: u8[i++] = signal * 0x7F + 0x80; len -= 1; break;
      case AUDIO_S8: s8[i++] = signal * 0x7F; len -= 1; break;
      default: assert(false); break;
      }
#else
      switch(have.format) {
      case AUDIO_F32: f32[i++] = cursor1; len -= 4; break;
      case AUDIO_S32: s32[i++] = steady; len -= 4; break;
      case AUDIO_U16: u16[i++] = steady + 0x8000; len -= 2; break;
      case AUDIO_S16: s16[i++] = steady; len -= 2; break;
      case AUDIO_U8: u8[i++] = steady + 0x80; len -= 1; break;
      case AUDIO_S8: s8[i++] = steady; len -= 1; break;
      default: assert(false); break;
      }
#endif
    }
    cursor1 += freq1 / (double)have.freq;
    cursor1 = fmod(cursor1, 1.0);
    cursor2 += freq2 / (double)have.freq;
    cursor2 = fmod(cursor2, 1.0);
    steady++;
  }
  update = !update;
  set_log_end(sdl_i);
}

int main() {

#if defined(NXDK)
  XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
#endif

  init();

  if (SDL_Init(SDL_INIT_AUDIO) != 0) {
    debugPrint("Unable to initialize SDL: %s", SDL_GetError());
    return 1;
  }


  while(true) {
    SDL_AudioSpec want;

    SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
    want.freq = 44100;
    want.format = AUDIO_S16;
    want.channels = 2;
    want.samples = 0;
    want.callback = MyAudioCallback;  // you wrote this function elsewhere.
    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0); //0 /*SDL_AUDIO_ALLOW_FORMAT_CHANGE*/);

    if (dev == 0) {
      debugPrint("Failed to open audio: %s\n", SDL_GetError());
      return 1;
    }


    SDL_PauseAudioDevice(dev, 0); /* start audio playing. */

    // Keep printing info
    uint32_t start = SDL_GetTicks();
    while((SDL_GetTicks() - start) < 5000) {
#if defined(NXDK)
      XVideoWaitForVBlank();
#endif
      debugClearScreen();
      debugPrint("signal1: %uHz (%d%%) at %lld\n", (unsigned int)freq1, (int)(volume1 * 100), (long long)(cursor1 * 1000LL));
      debugPrint("signal2: %uHz (%d%%) at %lld\n", (unsigned int)freq2, (int)(volume2 * 100), (long long)(cursor2 * 1000LL));
      debugPrint("\n");
      debugPrint("frequency: %uHz\n", have.freq);
      debugPrint("format: 0x%X\n", have.format);
      debugPrint("channels: %d\n", have.channels);
      debugPrint("silence: 0x%02X\n", have.silence);
      debugPrint("size: %d bytes (%d samples)\n", have.size, have.samples);
      SDL_Delay(100);
    }
    add_log("Done");

    SDL_CloseAudioDevice(dev);
    dump();
    break;

  }
}
