#include <hal/debug.h>
#include <hal/video.h>
#include "igr.h"
#include <assert.h>

#include <SDL.h>

void main() {
  XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

  SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

  SDL_Init(SDL_INIT_GAMECONTROLLER);

  SDL_GameController* controller = SDL_GameControllerOpen(0);
  assert(controller != NULL);

  uint32_t seed = KeQueryPerformanceCounter() & 0xFFFFFFFF;
  while(true) {
    XVideoWaitForVBlank();
    debugClearScreen();

    SDL_GameControllerUpdate();
    int is_a = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A);
    int is_b = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B);

    uint32_t esp;
    asm("mov %%esp, %0" : "=r"(esp));
    debugPrint("ESP 0x%08X\n", esp);
    debugPrint("Seed 0x%08X\n", seed);
    debugPrint("Alternative: %d; Trigger: %d\n", is_a, is_b);
    if (is_b) {
      //FIXME: What's a common index?
      IGRPadState state = {0};
      state.analog_left_trigger = 0xFF;
      state.analog_right_trigger = 0xFF;
      if (is_a) {
        state.digital_buttons |= 0x20; // Back
      } else {
        state.analog_black = 0xFF;
      }
      state.digital_buttons |= 0x10; // Start
      for(int i = 0; i <= 4; i++) {
        debugPrint("Triggering IGR for pad %d\n", i);
        notifyIGR(i, &state, true);
      }
    }
  }

  return;
}
