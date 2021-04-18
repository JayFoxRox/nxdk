#include <hal/debug.h>
#include <hal/video.h>
#include "igr.h"
#include <assert.h>

#include <SDL.h>

void synchronizeIgrToSdl(IGRPadState* state, const SDL_GameController* controller) {
  memset(state, 0x00, sizeof(IGRPadState));

  state->digital_dpad_up = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP);
  state->digital_dpad_down = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
  state->digital_dpad_left = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
  state->digital_dpad_right = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
  state->digital_start = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START);
  state->digital_back = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_BACK);
  state->digital_left_thumb = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_LEFTSTICK);
  state->digital_right_thumb = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_RIGHTSTICK);

  state->analog_a = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A) ? 0xFF : 0x00;
  state->analog_b = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B) ? 0xFF : 0x00;
  state->analog_x = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X) ? 0xFF : 0x00;
  state->analog_y = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y) ? 0xFF : 0x00;
  state->analog_black = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) ? 0xFF : 0x00;
  state->analog_white = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) ? 0xFF : 0x00;
  state->analog_left_trigger = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) >> 8;
  state->analog_right_trigger = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) >> 8;

  state->axis_left_x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
  state->axis_left_y = ~SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
  state->axis_right_x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
  state->axis_right_y = ~SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
}

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

    debugPrint("Seed 0x%08X\n", seed);

    IGRPadState state;
    synchronizeIgrToSdl(&state, controller);

    for(int i = 0; i < 4; i++) {
      debugPrint("Synchronizing IGR for pad %d\n", i);

      // In a real application you can also use `SDL_GameControllerGetPlayerIndex(controller)` to get the port
      unsigned int port = i;

      // In a real application you can also use `SDL_GameControllerGetAttached(controller)` to get the connection status
      bool connected = true;

      // Note that the IGR might modify the state
      notifyIGR(port, &state, connected);
    }
  }

  return;
}
