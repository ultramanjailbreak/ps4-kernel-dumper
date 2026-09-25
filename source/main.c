#include "ps4.h"

#define BUTTON_COMBINATION (PAD_OPTIONS | PAD_CROSS)
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

  // Initialize the PS4 controller handle subsystem
  // Assumes user is using controller handle ID 0 (primary pad)
  int pad_handle = scePadOpen(0, 0, 0, NULL);

  unsigned int current_buttons = 0;
  int held_duration = 0;

  // Background listening loop for button triggers
  while (1) {
    // Read current state data from the controller
    ScePadData pad_data;
    if (scePadReadState(pad_handle, &pad_data) == 0) {
        current_buttons = pad_data.buttons;
    }

    // Check if both Options and X buttons are currently held down together
    if ((current_buttons & BUTTON_COMBINATION) == BUTTON_COMBINATION) {
        held_duration += CHECK_INTERVAL_MS;

        // If the combo has been held continuously for 3000ms (3 seconds)
        if (held_duration >= HOLD_TIME_MS) {
            
            // 2. Play the custom hello.mp3 file via native system media services
            // This system command tells the media shell to play the file globally in the background
            system("orbis-player /data/self/system/common/hello.mp3 &");
            
            // Visual validation popup confirming audio execution
            printf_notification("Playing hello.mp3");

            // Reset loop hold tracker and pause briefly to avoid playing multiple times back-to-back
            held_duration = 0;
            sceKernelUsleep(2000000); // 2-second cooldown sleep period
        }
    } else {
        // Reset counter immediately if the user releases either button early
        held_duration = 0;
    }

    // Synchronize loop cycles to manage hardware resource consumption
    sceKernelUsleep(CHECK_INTERVAL_MS * 1000);
  }

  // Safe resource termination clean up on exit
  if (pad_handle >= 0) {
      scePadClose(pad_handle);
  }

  return 0;
}
