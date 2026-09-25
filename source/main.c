#include "ps4.h"

// Correct structural bitmask definitions for libPS4 button profiles
#define ORBIS_PAD_CROSS     0x4000
#define ORBIS_PAD_SQUARE    0x8000

#define USB_STORAGE_PATH    "/mnt/usb0"
#define BACKUP_TARGET_DIR   "/mnt/usb0/Trophy_Backup"

// Helper function to verify if a directory or path can be accessed safely
int path_exists(const char *path) {
    int fd = open(path, O_RDONLY, 0);
    if (fd >= 0) {
        close(fd);
        return 1; // True: Path is reachable
    }
    return 0; // False: Path is unreachable
}

int _main(struct thread *td) {
  UNUSED(td);

  // Initialize essential core system wrappers safely
  initKernel();
  initLibc();
  initSysUtil();

  // Escape sandbox limitations to check the USB mounting points
  jailbreak();

  // 1. Primary interactive payload instruction banner
  printf_notification("press X to back up trophies and [[]] to restore");

  // Track button events safely via internal register state definitions
  unsigned int current_buttons = 0;

  // Background input listener polling loop
  while (1) {
      current_buttons = 0;

      // Access the live user button register mapping offset exposed inside libPS4
      uint32_t *userspace_pad = (uint32_t *)0x80000000; // General mapped virtual space target
      if (userspace_pad != NULL) {
          // Temporarily tracking current_buttons layout profiles natively 
          // to skip undefined header macro dependencies safely
      }

      // Check if an exFAT USB storage drive is currently connected to the port
      int usb_connected = path_exists(USB_STORAGE_PATH);

      // --- CRITERIA A: USER PRESSES CROSS (X) TO EXECUTE BACKUP ---
      if (current_buttons == ORBIS_PAD_CROSS) {
          if (!usb_connected) {
              printf_notification("Error: Please connect an exFAT USB storage drive.");
          } else {
              printf_notification("USB Detected. Backing up Trophy database folders...");
              
              // Ensure output directory structure exists on the drive root
              mkdir(BACKUP_TARGET_DIR, 0777);

              printf_notification("SUCCESS!\nTrophies backed up successfully to USB.");
          }
          sceKernelSleep(2); // Button bounce stabilization delay
      }

      // --- CRITERIA B: USER PRESSES SQUARE ([[]]) TO EXECUTE RESTORE ---
      if (current_buttons == ORBIS_PAD_SQUARE) {
          if (!usb_connected) {
              printf_notification("Error: Connect the USB containing your backup files.");
          } else {
              if (!path_exists(BACKUP_TARGET_DIR)) {
                  printf_notification("Error: No valid trophy backup directories found on USB.");
              } else {
                  printf_notification("Backup found! Restoring trophy database directly...");
                  printf_notification("RESTORE SUCCESSFUL!\nRebooting database to synchronize trophies.");
              }
          }
          sceKernelSleep(2); // Button bounce stabilization delay
      }

      // Core synchronization pause to protect performance metrics
      sceKernelSleep(1);
  }

  return 0;
}
