#include "ps4.h"

// Hexadecimal macro mappings for target firmware revisions
#define CURRENT_FW_HEX   0x13520000  // Firmware 13.52 representation
#define TARGET_FW_HEX    0x13000000  // Spoofed target 13.00 representation

int _main(struct thread *td) {
  UNUSED(td);

  // Initialize essential bare-metal operating system libraries safely
  initKernel();
  initLibc();
  initSysUtil();

  // 1. Elevate process credentials and escape application sandbox bounds
  jailbreak();

  // 2. Broadcast the live initialization confirmation banner
  printf_notification("Initializing Firmware Spoof Tool...");

  // 3. Inject Version Overwrite Into Active System Control Tables
  // Assign target version values to the active kernel system registry space
  uint32_t spoof_version = TARGET_FW_HEX;
  size_t size = sizeof(spoof_version);

  // Intercept and overwrite the standard SDK / Kernel execution identifier parameters
  int res1 = sysctlbyname("kern.sdk_version", NULL, NULL, &spoof_version, size);
  int res2 = sysctlbyname("machdep.rcmgr.fw_version", NULL, NULL, &spoof_version, size);

  // 4. Verification Check and Reporting
  if (res1 >= 0 || res2 >= 0) {
      printf_notification(
          "SPOOF SUCCESSFUL!\n"
          "Firmware changed from 13.52 -> 13.00"
      );
  } else {
      printf_notification("Error: Failed to write to kernel system tables.");
  }

  return 0;
}
