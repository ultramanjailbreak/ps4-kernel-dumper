#include "ps4.h"

int _main(struct thread *td) {
  UNUSED(td);

  // Initialize essential core system wrappers safely
  initKernel();
  initLibc();
  initSysUtil();

  // 1. Elevate process credentials and break out of the application sandbox context
  jailbreak();

  // 2. Broadcast the live initialization confirmation banner
  printf_notification("Activating Production Debug Settings...");

  // 3. Define the actual boolean activation value (1 = Enabled)
  int activate = 1;
  size_t size = sizeof(activate);

  // 4. Update the real Kernel System Control parameters (sysctl)
  // These calls directly modify the active Registry Manager subsystem flags
  
  // Set internal development node mode to active
  sysctlbyname("machdep.rcmgr.intdev", NULL, NULL, &activate, size);
  
  // Set system debugger permissions to active
  sysctlbyname("machdep.rcmgr.sl_debugger", NULL, NULL, &activate, size);
  
  // Force the main user interface shell to populate the real Debug Options menu
  sysctlbyname("machdep.rcmgr.debug_menu", NULL, NULL, &activate, size);
  
  // Unshackle internal package installation permissions (Allows PKG installation)
  sysctlbyname("machdep.rcmgr.flaged_updater", NULL, NULL, &activate, size);

  // 5. Broadcast success message completion banner
  printf_notification("Debug Settings Successfully Activated!\nCheck the bottom of System Settings.");

  return 0;
}
