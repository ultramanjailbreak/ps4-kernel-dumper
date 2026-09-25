#include "ps4.h"

int _main(struct thread *td) {
  UNUSED(td);

  // Initialize essential bare-metal operating system libraries safely
  initKernel();
  initLibc();
  initSysUtil();

  // Elevate system process privileges to escape application sandbox bounds
  jailbreak();

  // 1. Broadcast the requested initial configuration notification banner
  printf_notification("Loading UpaHen");
  sceKernelSleep(2); // Brief pacing delay to allow the notification to render completely

  // 2. Overwrite System Software Update Parameter States (sysctl)
  // These variables alter how the native "System Software Update" app operates in Settings
  int activate = 1;
  size_t size = sizeof(activate);

  // Instructs the OS updater daemon to enable local offline package sourcing
  sysctlbyname("machdep.rcmgr.usb_update_allowed", NULL, NULL, &activate, size);
  
  // Sets the internal installation mode configuration flag to read external hardware keys
  sysctlbyname("machdep.rcmgr.flaged_updater", NULL, NULL, &activate, size);

  // 3. Broadcast execution success completion banner
  printf_notification(
      "UpaHen Activated!\n"
      "Go to Settings > System Software Update to select USB .PUP"
  );

  return 0;
}
