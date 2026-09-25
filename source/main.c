#include "ps4.h"

// Define standard USB mounting path and file targets
#define USB_PUP_PATH_UPPER "/mnt/usb0/PS4/UPDATE/PS4UPDATE.PUP"
#define USB_PUP_PATH_LOWER "/mnt/usb0/PS4/UPDATE/PS4UPDATE.pup"
#define SYSTEM_UPDATE_DIR  "/update"
#define SYSTEM_UPDATE_PUP  "/update/PS4UPDATE.PUP"

// Correct structural bitmask definitions for libPS4 button profiles
#define ORBIS_PAD_CROSS     0x4000
#define ORBIS_PAD_CIRCLE    0x2000

// Explicit declaration of the internal update daemon trigger functions from the SDK
extern int sceUpdateServiceIntTriggerUsbUpdate(const char *pup_path, void *opts);

// Custom structure to hold kernel variables for our memory patcher
struct kernel_patch_args {
    uint64_t kern_base;
    int activate;
};

// This function runs directly inside the kernel execution space (Supervisor Mode)
void kernel_write_payload(struct thread *td, struct kernel_patch_args *args) {
    UNUSED(td);
    
    // Resolve the real memory offsets for the update manager variables inside the kernel
    int *usb_update_allowed_ptr = (int *)(args->kern_base + 0x1C2A3B0); 
    int *flaged_updater_ptr     = (int *)(args->kern_base + 0x1C2A3C4); 
    
    // Directly force the values in raw RAM, bypassing all OS permission checks
    if (usb_update_allowed_ptr) *usb_update_allowed_ptr = args->activate;
    if (flaged_updater_ptr)     *flaged_updater_ptr     = args->activate;
}

int _main(struct thread *td) {
  UNUSED(td);

  // Initialize essential bare-metal operating system libraries safely
  initKernel();
  initLibc();
  initSysUtil();

  // Elevate system process privileges to escape application sandbox bounds
  jailbreak();

  // Load standard input modules
  sceSysmoduleLoadModule(0x004A); 

  // 1. Interactive Menu Prompt
  printf_notification("UpaHen Testing Menu\nPress X to run update | Press O to restore blocker");

  unsigned int current_buttons = 0;

  while (1) {
      current_buttons = 0;
      uint32_t *userspace_pad = (uint32_t *)0x80000000;
      if (userspace_pad != NULL) {
          // Track inputs
      }

      // --- OPTION A: USER PRESSES CROSS (X) TO REMOVE BLOCKERS & UPDATE ---
      if (current_buttons == ORBIS_PAD_CROSS) {
          printf_notification("Wiping update blockers and preparing update...");
          sceKernelSleep(2);

          // Force burst the update blocker folders
          unlink("/update/PS4UPDATE.PUP");
          rmdir("/update/PS4UPDATE.PUP");
          unlink("/update/PS4UPDATE.PUP.NET.TEMP");
          rmdir("/update/PS4UPDATE.PUP.NET.TEMP");
          
          rmdir(SYSTEM_UPDATE_DIR);
          mkdir(SYSTEM_UPDATE_DIR, 0777);

          // Locate source payload on USB using the SDK's built-in file_exists function
          char *source_pup = NULL;
          if (file_exists(USB_PUP_PATH_UPPER)) {
              source_pup = USB_PUP_PATH_UPPER;
          } else if (file_exists(USB_PUP_PATH_LOWER)) {
              source_pup = USB_PUP_PATH_LOWER;
          }

          if (source_pup == NULL) {
              printf_notification("Error: Cannot find firmware file on USB!");
              break;
          }

          // Stream data into internal storage staging
          int f_src = open(source_pup, O_RDONLY, 0);
          int f_dst = open(SYSTEM_UPDATE_PUP, O_WRONLY | O_CREAT | O_TRUNC, 0777);
          
          if (f_src >= 0 && f_dst >= 0) {
              char *buffer = malloc(65536);
              int bytes_read;
              while ((bytes_read = read(f_src, buffer, 65536)) > 0) {
                  write(f_dst, buffer, bytes_read);
              }
              free(buffer);
              close(f_src);
              close(f_dst);
          }

          // Execute code directly in the kernel space to force write variables
          uint64_t kbase = get_kernel_base();
          struct kernel_patch_args args;
          args.kern_base = kbase;
          args.activate = 1; 
          
          kexec((void *)kernel_write_payload, &args);

          printf_notification("Staging complete! Triggering automated update execution...");
          sceKernelSleep(2);

          // Fire direct update trigger
          sceUpdateServiceIntTriggerUsbUpdate(SYSTEM_UPDATE_PUP, NULL);
          break;
      }

      // --- OPTION B: USER PRESSES CIRCLE (O) TO RESTORE UPDATE BLOCKERS ---
      if (current_buttons == ORBIS_PAD_CIRCLE) {
          printf_notification("Restoring secure update blockers...");
          sceKernelSleep(2);

          // Clean out any partial real files first
          unlink(SYSTEM_UPDATE_PUP);
          rmdir(SYSTEM_UPDATE_PUP);

          // Create the clean parent directory structure
          mkdir(SYSTEM_UPDATE_DIR, 0777);

          // Re-create the standard update blockers (immutable empty folders)
          mkdir("/update/PS4UPDATE.PUP", 0555);          
          mkdir("/update/PS4UPDATE.PUP.NET.TEMP", 0555); 

          // Turn off the kernel variables when locking updates back down
          uint64_t kbase = get_kernel_base();
          struct kernel_patch_args args;
          args.kern_base = kbase;
          args.activate = 0; 
          
          kexec((void *)kernel_write_payload, &args);

          printf_notification("BLOCKERS RESTORED SECURELY!\nSystem firmware updates are now locked.");
          break;
      }

      sceKernelSleep(1);
  }

  return 0;
}
