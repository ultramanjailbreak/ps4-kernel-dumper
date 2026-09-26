#include <ps4.h>

// Configuration parameters
#define CURRENT_FW    "13.52"
#define TARGET_TEXT   "FBI TEST"

// Helper function to find a byte pattern in memory
static uint8_t *find_pattern(uint8_t *start, size_t length, const uint8_t *pattern, size_t pattern_len) {
    if (pattern_len > length) return NULL;
    for (size_t i = 0; i <= length - pattern_len; i++) {
        if (memcmp(start + i, pattern, pattern_len) == 0) {
            return start + i;
        }
    }
    return NULL;
}

// The standard entry point function for the ps4-payload-sdk
int payload_main(void *payload_args) {
    // 1. Initialize system library wrappers
    initKernel();
    initLibc();

    // 2. 10-second visual countdown loop using the PS4 native kernel sleep function
    // (1,000,000 microseconds = 1 second)
    for (int i = 10; i > 0; i--) {
        sceKernelUsleep(1000000); 
    }

    // 3. Format the search and replace strings into UTF-16LE format (used by PS4 ShellUI)
    char old_str[32];
    char new_str[32];
    snprintf(old_str, sizeof(old_str), "HEN %s", CURRENT_FW);
    snprintf(new_str, sizeof(new_str), "%s", TARGET_TEXT);

    uint8_t old_utf16[64] = {0};
    uint8_t new_utf16[64] = {0};
    size_t old_len = 0;
    size_t new_len = 0;

    for (int i = 0; old_str[i] != '\0' && old_len < 62; i++) {
        old_utf16[old_len++] = old_str[i];
        old_utf16[old_len++] = 0;
    }
    for (int i = 0; new_str[i] != '\0' && new_len < 62; i++) {
        new_utf16[new_len++] = new_str[i];
        new_utf16[new_len++] = 0;
    }

    // 4. Memory scanning and runtime patching
    // Scans common userland allocation ranges where SceShellCore maps string tables
    uint8_t *scan_start = (uint8_t *)0x800000000; 
    size_t scan_size = 0x20000000; // 512MB scan range chunk
    int patch_count = 0;

    uint8_t *match = find_pattern(scan_start, scan_size, old_utf16, old_len);
    if (match != NULL) {
        // If string lengths match, perform a direct inline replacement
        if (old_len == new_len) {
            memcpy(match, new_utf16, new_len);
            patch_count++;
        } 
        // If length varies, handle string size descriptor adjustments safely
        else {
            uint32_t *len_prefix = (uint32_t *)(match - 4);
            // Verify if a 4-byte length descriptor precedes the string
            if (*len_prefix == (uint32_t)strlen(old_str)) {
                *len_prefix = (uint32_t)strlen(new_str);
                memcpy(match, new_utf16, new_len);
                patch_count++;
            }
        }
    }

    // 5. Provide UI confirmation to the user on screen
    char notification_msg[128];
    if (patch_count > 0) {
        snprintf(notification_msg, sizeof(notification_msg), "Spoofed to: %s\nRe-open System Info!", TARGET_TEXT);
    } else {
        snprintf(notification_msg, sizeof(notification_msg), "Spoof failed: Target string not found.");
    }
    
    printf_notification(notification_msg);

    return 0;
}
