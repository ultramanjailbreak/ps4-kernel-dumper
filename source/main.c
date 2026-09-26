#include <stdint.h>
#include <stddef.h>

// Include your custom header
#include "payload.h"

// Explicitly declare system call wrappers or symbols provided by libPS4 if they 
// are not available in standard headers. In libPS4, system call interfaces 
// are often mapped via direct function signatures.
extern int open(const char *path, int flags, ...);
extern int close(int fd);
extern int read(int fd, void *buf, size_t nbyte);

// Basic inline string layout builder to avoid referencing external snprintf/__snprintf_chk
void custom_build_path(char *dst, const char *usb, const char *suffix) {
    while (*usb) {
        *dst++ = *usb++;
    }
    while (*suffix) {
        *dst++ = *suffix++;
    }
    *dst = '\0';
}

const char *usb_mounts[] = {
    "/mnt/usb0",
    "/mnt/usb1",
    "/mnt/usb2"
};

int verify_package_version(const char *filepath, uint32_t *out_version) {
    if (!filepath || !out_version) {
        return -1;
    }

    // O_RDONLY is typically 0 in POSIX/Orbis environments
    int fd = open(filepath, 0, 0);
    if (fd < 0) {
        return -2; 
    }

    SystemPackageHeader header;
    int bytes_read = read(fd, &header, sizeof(SystemPackageHeader));
    close(fd);

    if (bytes_read < (int)sizeof(SystemPackageHeader)) {
        return -3; 
    }

    *out_version = header.target_fw;
    return 0; 
}

int scan_storage_for_updates(char *out_path, size_t max_len, uint32_t *detected_fw) {
    if (!out_path || max_len == 0 || !detected_fw) {
        return 0;
    }

    char target_buffer[256];
    const char *update_suffix = "/PS4/UPDATE/PS4UPDATE.PUP";

    for (size_t i = 0; i < sizeof(usb_mounts) / sizeof(usb_mounts[0]); i++) {
        custom_build_path(target_buffer, usb_mounts[i], update_suffix);
        
        // Use a standard open check to verify file presence instead of 'access'
        int fd = open(target_buffer, 0, 0);
        if (fd >= 0) {
            close(fd);
            uint32_t fw_ver = 0;
            if (verify_package_version(target_buffer, &fw_ver) == 0) {
                // Manual safe copy loop to avoid strncpy / buffer checks
                size_t j = 0;
                while (j < max_len - 1 && target_buffer[j] != '\0') {
                    out_path[j] = target_buffer[j];
                    j++;
                }
                out_path[j] = '\0';
                *detected_fw = fw_ver;
                return 1; 
            }
        }
    }
    return 0; 
}

int _main(void) {
    char package_path[256];
    uint32_t package_fw = 0;

    scan_storage_for_updates(package_path, sizeof(package_path), &package_fw);

    return 0;
}
