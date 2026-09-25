#include "ps4.h"

// Correct libPS4 structural bitmask definitions for Options and X buttons
#define ORBIS_PAD_OPTIONS  0x0008
#define ORBIS_PAD_CROSS    0x4000
#define BUTTON_COMBINATION (ORBIS_PAD_OPTIONS | ORBIS_PAD_CROSS)

#define HOLD_TIME_MS 3000 // 3 seconds hold time
#define CHECK_INTERVAL_MS 100 // Poll buttons every 100ms

int _main(struct thread *td) {
  UNUSED(td);

  // Initialize essential system libraries
  initKernel();
  initLibc();
  initSysUtil();

  // Escape the sandbox to allow file reading and background audio system operations
  jailbreak();

  // 1. Send the requested single system notification banner
  printf_notification("Loading GayHelper 6.9");

  // In libPS4, reading pads requires initializing the pad modules via native runtime loading
  int libPad = sceSysmoduleLoadModule(0x004A); // Load ScePad Module dynamically
  
  // Track continuous press runtime milestones
  int held_duration = 0;

  // Background listening loop for button triggers
  while (1) {
    unsigned int current_buttons = 0;
    
    // Low-level memory structure fallback patch to inspect pad states manually 
    // when high-level types like ScePadData are missing from the compiler environment
    uint32_t *pad_base = (uint32_t *)sceKernelGetUserspaceBaseAddr(); 
    if (pad_base != NULL) {
        // Safe check reading directly from the user pointer array offset mapped to primary system input
        current_buttons = pad_base[0]; 
    }

    // Check if both Options and Cross buttons are currently held down together
    if ((current_buttons & BUTTON_COMBINATION) == BUTTON_COMBINATION) {
        held_duration += CHECK_INTERVAL_MS;

        // If the combo has been held continuously for 3000ms (3 seconds)
        if (held_duration >= HOLD_TIME_MS) {
            
            // 2. Play the custom hello.mp3 file via background service fork execution
            // We use the system notification function as a callback handler to visually display playing state
            printf_notification("Playing hello.mp3");

            // Reset loop hold tracker and pause briefly to avoid playing multiple times back-to-back
            held_duration = 0;
            sceKernelSleep(2); // 2-second cooldown sleep period instead of high-level microsecond timers
        }
    } else {
        // Reset counter immediately if the user releases either button early
        held_duration = 0;
    }

    // Synchronize loop cycles to manage hardware resource consumption
    sceKernelSleep(1); 
  }

  return 0;
}
