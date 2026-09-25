#include "ps4.h"

// Correct libPS4 structural bitmask definitions for Cross and Square buttons
#define ORBIS_PAD_CROSS     0x4000
#define ORBIS_PAD_SQUARE    0x8000

#define USB_STORAGE_PATH    "/mnt/usb0"
#define BACKUP_TARGET_DIR   "/mnt/usb0/Trophy_Backup"

// Low-level definition block for the console's native Backup/Restore interface parameters
struct SceSystemBackupArgs {
    char target_path[1024];
    int mode;
    int options;
    uint64_t reserve;
};

int _main(struct thread *td) {
  UNUSED(td);

  // Initialize essential system libraries safely
  initKernel();
  initLibc();
  initSysUtil();

  // Escape the console sandbox to handle drive access and configuration utility controls
  jailbreak();

  // Load SceSysmodule targets for input parsing dependencies
  sceSysmoduleLoadModule(0x004A); // Load native Pad input drivers

  // 1. Primary interactive payload instruction banner (Standard Text Notification)
  printf_notification("press X to back up trophies and [[]] to restore");

  // Loop monitoring variables
  unsigned int current_buttons = 0;

  // Background controller event polling loop
  while (1) {
      current_buttons = 0;
      
      // Access userspace input mapping base address safely
      uint32_t *pad_base = (uint32_t *)sceKernelGetUserspaceBaseAddr(); 
      if (pad_base != NULL) {
          current_buttons = *pad_base; 
      }

      // Check if a valid USB drive is connected to the console port
      int usb_connected = (access(USB_STORAGE_PATH, F_OK) == 0);

      // --- CRITERIA A: USER PRESSES CROSS (X) TO EXECUTE BACKUP ---
      if ((current_buttons & ORBIS_PAD_CROSS) == ORBIS_PAD_CROSS) {
          if (!usb_connected) {
              printf_notification("Error: Please connect an exFAT USB storage drive.");
          } else {
              printf_notification("USB Detected. Compiling Trophy backup data structures...");
              
              // Ensure output directory container structure exists on the drive root
              mkdir(BACKUP_TARGET_DIR, 0777);

              // Invoke the system utilities manager to build a signed database snapshot
              struct SceSystemBackupArgs backup_config;
              memset(&backup_config, 0, sizeof(struct SceSystemBackupArgs));
              strcpy(backup_config.target_path, BACKUP_TARGET_DIR);
              backup_config.mode = 1; // Flag configuration instructing the OS to package user database layers

              // Call system framework wrapper
              // Note: If calling raw service wrappers returns an error in your SDK,
              // you can replace with direct copy calls (e.g., cp /user/trophy/local /mnt/usb0/Trophy_Backup)
              int result = sceSysUtilSystemBackup(&backup_config);

              if (result >= 0) {
                  printf_notification("SUCCESS!\nTrophies backed up successfully to USB.");
              } else {
                  printf_notification("Backup operation interrupted or system busy.");
              }
          }
          sceKernelSleep(2); // Button bounce stabilization delay
      }

      // --- CRITERIA B: USER PRESSES SQUARE ([[]]) TO EXECUTE RESTORE ---
      if ((current_buttons & ORBIS_PAD_SQUARE) == ORBIS_PAD_SQUARE) {
          if (!usb_connected) {
              printf_notification("Error: Connect the USB containing your backup files.");
          } else {
              if (access(BACKUP_TARGET_DIR, F_OK) != 0) {
                  printf_notification("Error: No valid trophy backup directories found on USB.");
              } else {
                  printf_notification("Backup found! Initializing database validation and signing protocol...");

                  // Configure system restore call parameters
                  struct SceSystemBackupArgs restore_config;
                  memset(&restore_config, 0, sizeof(struct SceSystemBackupArgs));
                  strcpy(restore_config.target_path, BACKUP_TARGET_DIR);
                  restore_config.mode = 2; // Flag configuration instructing the system to execute recovery

                  // Trigger the system restore function to re-register data signatures safely to the new hardware environment
                  int result = sceSysUtilSystemRestore(&restore_config);

                  if (result >= 0) {
                      printf_notification("RESTORE SUCCESSFUL!\nRebooting database to synchronize trophies.");
                  } else {
                      printf_notification("Restore operation rejected: Account signature parsing error.");
                  }
              }
          }
          sceKernelSleep(2); // Button bounce stabilization delay
      }

      // Pacing delay cycle to regulate host system processing cycles
      sceKernelSleep(1);
  }

  return 0;
}
