#include <ps4.h>

#define CURRENT_FW    "13.52"
#define TARGET_TEXT   "FBI TEST"

// Subtly quiet the unused parameter warning for compilation rules
#define UNUSED(x) (void)(x)

// Helper function to scan memory chunks
static uint8_t *find_pattern(uint8_t *start, size_t length, const uint8_t *pattern, size_t pattern_len) {
    if (pattern_len > length) return NULL;
    for (size_t i = 0; i <= length - pattern_len; i++) {
        if (memcmp(start + i, pattern, pattern_len) == 0) {
            return start + i;
        }
    }
    return NULL;
}

// Fixed entry symbol name to match your SDK's exact crt0.s expectation (_main)
int _main(void *payload_args) {
    // Suppress the unused parameter warning cleanly
    UNUSED(payload_args);

    // Initialize core system wrappers natively
    initKernel();
    initLibc();

    // Setup string tables matching your targets
    char old_str[64];
    char new_str[64];
    snprintf(old_str, sizeof(old_str), "HEN %s", CURRENT_FW);
    snprintf(new_str, sizeof(new_str), "%s", TARGET_TEXT);

    uint8_t old_utf16[128];
    uint8_t new_utf16[128];
    size_t old_len = 0;
    size_t new_len = 0;

    // Convert strings cleanly to UTF-16 Little Endian
    for (int i = 0; old_str[i] != '\0' && old_len < 120; i++) {
        old_utf16[old_len++] = old_str[i];
        old_utf16[old_len++] = 0;
    }
    for (int i = 0; new_str[i] != '\0' && new_len < 120; i++) {
        new_utf16[new_len++] = new_str[i];
        new_utf16[new_len++] = 0;
    }

    // Direct active scan area for background shell UI layers
    uint8_t *scan_start = (uint8_t *)0x800000000; 
    size_t scan_size = 0x30000000; 
    int patches_applied = 0;

    uint8_t *match = find_pattern(scan_start, scan_size, old_utf16, old_len);
    if (match != NULL) {
        if (old_len == new_len) {
            memcpy(match, new_utf16, new_len);
            patches_applied++;
        } else {
            uint32_t *len_prefix = (uint32_t *)(match - 4);
            if (*len_prefix == (uint32_t)strlen(old_str)) {
                *len_prefix = (uint32_t)strlen(new_str);
                memcpy(match, new_utf16, new_len);
                patches_applied++;
            }
        }
    }

    // Trigger instant global UI update text confirmation window
    char popup_msg[256];
    if (patches_applied > 0) {
        snprintf(popup_msg, sizeof(popup_msg), "System Updated: %s applied instantly!", TARGET_TEXT);
    } else {
        snprintf(popup_msg, sizeof(popup_msg), "Instant spoof failed: target memory string not located.");
    }
    
    printf_notification(popup_msg);

    return 0;
}
