#ifndef __IGR_H__
#define __IGR_H__

#include <stdint.h>
#include <stdbool.h>

// This is the kind of structure that the XDK uses
typedef struct {
  uint32_t report_index; // +0

  union {
    uint16_t _digital_buttons; // +4
    struct {
      uint16_t digital_dpad_up:1;     // 0x0001
      uint16_t digital_dpad_down:1;   // 0x0002
      uint16_t digital_dpad_left:1;   // 0x0004
      uint16_t digital_dpad_right:1;  // 0x0008
      uint16_t digital_start:1;       // 0x0010
      uint16_t digital_back:1;        // 0x0020
      uint16_t digital_left_thumb:1;  // 0x0040
      uint16_t digital_right_thumb:1; // 0x0080
      uint16_t _digital_0x100:1;      // 0x0100
      uint16_t _digital_0x200:1;      // 0x0200
      uint16_t _digital_0x400:1;      // 0x0400
      uint16_t _digital_0x800:1;      // 0x0800
      uint16_t _digital_0x1000:1;     // 0x1000
      uint16_t _digital_0x2000:1;     // 0x2000
      uint16_t _digital_0x4000:1;     // 0x4000
      uint16_t _digital_0x8000:1;     // 0x8000
    };
  };

  uint8_t analog_a;             // +6
  uint8_t analog_b;             // +7
  uint8_t analog_x;             // +8
  uint8_t analog_y;             // +9
  uint8_t analog_black;         // +10
  uint8_t analog_white;         // +11
  uint8_t analog_left_trigger;  // +12
  uint8_t analog_right_trigger; // +13

  int16_t axis_left_x;  // +14
  int16_t axis_left_y;  // +16
  int16_t axis_right_x; // +18
  int16_t axis_right_y; // +20

  uint8_t _unk22[12]; // +22
} __attribute__((packed)) IGRPadState;

void notifyIGR(unsigned int port, IGRPadState* state, bool connected);

#endif
