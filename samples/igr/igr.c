#include <assert.h>
#include <xboxkrnl/xboxkrnl.h>

#include "igr.h"

// `connected` parameter was added manually (expected in KeRaiseIrqlToDpcLevel)
static uint32_t emulateXInputGetState(void* device, IGRPadState* state, uint32_t return_value) {

  // Access to the pattern that we generate to fool IGR implementations
  extern uint8_t pattern_values[];

  // Pattern supported by nkpatcher 10:
  // - Detects if XInputGetState is caller.
  // - Checked in KeRaiseIrqlToDpcLevel.
  // - Caller of KeRaiseIrqlToDpcLevel must be below address 0x80000000.
  // - KeRaiseIrqlToDpcLevel installs hook.
  // - Hook is triggered when XInputGetState returns.
  // - Return value of XInputGetState must be zero, or IGR is skipped.
  uint8_t pattern_mask[]      = { 0xFF,0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF };
  uint8_t pattern_reference[] = { 0x8B,   0,0x24,0x0C,   0,   0,0xA3,0x00,0x00,0x00 };
  bool nkpatcher = true;
  for(int i = 0; i < sizeof(pattern_mask); i++) {
    nkpatcher &= ((pattern_values[i] & pattern_mask[i]) == pattern_reference[i]);
  }

  // Patterns supported by iND BIOS v5003:
  // - Detects if XInputGetState is caller.
  // - Checked in KeRaiseIrqlToDpcLevel.
  // - IGR part of KeRaiseIrqlToDpcLevel.
  uint8_t pattern_reference_a[] = { 0x8B,0x54,0x24,0x0C,0x8B,0x8A,0xA3,0x00 };
  uint8_t pattern_reference_b[] = { 0x8B,0x54,0x24,0x0C,0x80,0xBA,0xA3,0x00 };
  bool ind_bios_variant_a = true;
  for(int i = 0; i < sizeof(pattern_reference_a); i++) {
    ind_bios_variant_a &= (pattern_values[i] == pattern_reference_a[i]);
  }
  bool ind_bios_variant_b = true;
  for(int i = 0; i < sizeof(pattern_reference_b); i++) {
    ind_bios_variant_b &= (pattern_values[i] == pattern_reference_b[i]);
  }
  bool ind_bios = ind_bios_variant_a || ind_bios_variant_b;

  // Ensure that each IGR is supported
  assert(nkpatcher && ind_bios);

  // KeRaiseIrqlToDpcLevel is called and the caller must have a specific pattern of bytes.
  // This assembly block matches that.
  uint8_t irql;
  asm volatile(// Push fake XInputGetState arguments
               "push %[state];\n"
               "push %[device];\n"

               // Push fake XInputGetState return address (equivalent to call)
               "push cleanup;\n"

               // Create fake XInputGetState stack frame
               "push $0;\n"
               "push $0;\n"

               // This will be detected by IGR code
               "call _KeRaiseIrqlToDpcLevel@0;\n"
               ".globl _pattern_values\n"
               "_pattern_values:\n"
               "movl 0xC(%%esp), %%edx;\n"
               "movl 0xA3(%%edx), %%ecx;\n"

               // Remove the stack frame, return address and arguments
               "cleanup:\n"
               "add $8, %%esp;\n"
               "add $4, %%esp;\n"
               "add $8, %%esp;\n"
               : "=a"(irql)
               : [device]"r"(device), [state]"r"(state)
               : "edx", "ecx");
  KfLowerIrql(irql);

  // When this returns, nkpatcher will jump to the IGR hook
  return return_value;
}

void notifyIGR(unsigned int port, IGRPadState* state, bool connected) {
  
  // Setup unk_b
  struct UnkB {
    uint8_t unk0[20]; // +0
    uint32_t port; // +20
    //FIXME: Fields missing. Extend to proper size?
  } __attribute__((packed)) unk_b = {0};
  unk_b.port = port;

  // Setup unk_a
  struct UnkA {
    struct UnkB* unk_b; // +0
    //FIXME: Fields missing. Extend to proper size?
  } __attribute__((packed)) unk_a = {0};
  unk_a.unk_b = &unk_b;

  // Setup device
  struct {
    struct UnkA* unk_a; // +0
    uint32_t unk4; // +4
    uint32_t unk8; // +8
    uint32_t unk12; // +12
    uint32_t unk16; // +16
    uint8_t report[30]; // +20
    //FIXME: Fields missing. Extend to proper size?
  } __attribute__((packed)) device = {0};
  device.unk_a = &unk_a;

  // Some IGR will hook the input code and read the internal state, so we prepare that
  assert(sizeof(device.report) == (sizeof(state) - 4));
  memcpy(device.report, (uintptr_t)state + 4, sizeof(device.report));

  //FIXME: We don't hint wether the gamepad is connected or not in internal structures

  // Trigger IGR by going into our fake function
  emulateXInputGetState(&device, state, connected ? 0x00000000 : 0x0000048F);
}
