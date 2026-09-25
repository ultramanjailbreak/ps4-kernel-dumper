#include "ps4.h"

// Define standard USB mounting path and file targets
#define USB_PUP_PATH_UPPER "/mnt/usb0/PS4/UPDATE/PS4UPDATE.PUP"
#define USB_PUP_PATH_LOWER "/mnt/usb0/PS4/UPDATE/PS4UPDATE.pup"
#define SYSTEM_UPDATE_DIR  "/update"

int _main(struct thread *td) {
  UNUSED(td);

  // Initialize essential bare-metal operating system libraries safely
  initKernel();
  initLibc();
  initSysUtil();

  // Elevate system process privileges to escape application sandbox bounds
  jailbreak();

  // 1. Broadcast initial configuration notification banner
  printf_notification("Loading UpaHen");
  sceKernelSleep(2); 

  // 2. FORCE BUST THE UPDATE BLOCKERS
  // We completely delete the empty dummy placeholder folders blocking your update path
  unlink("/update/PS4UPDATE.PUP");
  rmdir("/update/PS4UPDATE.PUP");
  unlink("/update/PS4UPDATE.PUP.NET.TEMP");
  rmdir("/update/PS4UPDATE.PUP.NET.TEMP");
  
  // Re-create a clean, wide-open update folder destination
  rmdir(SYSTEM_UPDATE_DIR);
  mkdir(SYSTEM_UPDATE_DIR, 0777);

  // 3. Scan the USB to see if either .PUP or .pup exists
  int has_pup = 0;
  char *chosen_path = NULL;

  int fd_check = open(USB_PUP_PATH_UPPER, O_RDONLY, 0);
  if (fd_check >= 0) {
      chosen_path = USB_PUP_PATH_UPPER;
      has_pup = 1;
      close(fd_check);
  } else {
      fd_check = open(USB_PUP_PATH_LOWER, O_RDONLY, 0);
      if (fd_check >= 0) {
          chosen_path = USB_PUP_PATH_LOWER;
          has_pup = 1;
          close(fd_check);
      }
  }

  // Fallback check if USB is empty or unplugged
  if (!has_pup) {
      printf_notification("Error: Cannot find PS4/UPDATE/PS4UPDATE.PUP on USB!");
      return -1;
  }

  // 4. Overwrite Firmware Execution Framework Properties
  // This explicitly signals the native Settings app to look at the USB path we verified
  int activate = 1;
  size_t size = sizeof(activate);
  sysctlbyname("machdep.rcmgr.usb_update_allowed", NULL, NULL, (char *)&activate, size);
  sysctlbyname("machdep.rcmgr.flaged_updater", NULL, NULL, (char *)&activate, size);

  // 5. Broadcast final ready status confirmation banner
  printf_notification(
      "UpaHen Connected To USB!\n"
      "Open Settings -> System Software Update now."
  );

  return 0;
}
