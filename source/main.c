#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include "payload.h"

// Define the mount locations checked by your platform deployment layout
const char *usb_mounts[] = {
    "/mnt/usb0",
    "/mnt/usb1",
    "/mnt/usb2"
};

int verify_package_version(const char *filepath, uint32_t *out_version) {
    if (!filepath || !out_version) {
        return -1;
    }

    // Open file block under low-level bare metal or basic POSIX layout handles
    int fd = open(filepath, O_RDONLY, 0);
    if (fd < 0) {
        return -2; // File unavailable
    }

    SystemPackageHeader header;
    ssize_t bytes_read = read(fd, &header, sizeof(SystemPackageHeader));
    close(fd);

    if (bytes_read < (ssize_t)sizeof(SystemPackageHeader)) {
        return -3; // Fragmented or malformed header structure
    }

    // Assign the discovered framework version to output parameters
    *out_version = header.target_fw;
    return 0; 
}

int scan_storage_for_updates(char *out_path, size_t max_len, uint32_t *detected_fw) {
    if (!out_path || max_len == 0 || !detected_fw) {
        return 0;
    }

    char target_buffer[512];

    for (size_t i = 0; i < sizeof(usb_mounts) / sizeof(usb_mounts[0]); i++) {
        // Build the target checking string layout pointing to standard payload conventions
        snprintf(target_buffer, sizeof(target_buffer), "%s/PS4/UPDATE/PS4UPDATE.PUP", usb_mounts[i]);
        
        // Ensure standard accessibility before verification reads
        if (access(target_buffer, F_OK) == 0) {
            uint32_t fw_ver = 0;
            if (verify_package_version(target_buffer, &fw_ver) == 0) {
                strncpy(out_path, target_buffer, max_len - 1);
                out_path[max_len - 1] = '\0'; // Enforce null termination safety
                *detected_fw = fw_ver;
                return 1; // Struct matches safely
            }
        }
    }
    return 0; 
}

// System payload standard entry point initialization routine
int _main(void) {
    char package_path[512];
    uint32_t package_fw = 0;

    // Run the detection scanning pipeline
    int package_detected = scan_storage_for_updates(package_path, sizeof(package_path), &package_fw);
    
    if (package_detected) {
        // Add safe console UI print strings or frame handler integrations below
        // Example: your_ui_log_function("Update Detected!", package_fw);
    }

    return 0;
}
