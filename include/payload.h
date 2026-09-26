#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <stdint.h>
#include <stddef.h>

// Force exact data layout mapping without compiler padding
#pragma pack(push, 1)

/**
 * @brief Standardized mockup structure for reading binary package headers.
 */
typedef struct {
    char magic[4];          // Explicitly 4 bytes for magic signatures (e.g., "OSUP")
    uint32_t target_fw;     // The targeted firmware version integer 
    uint32_t package_flags; // Flags indicating package metadata properties
    uint8_t hash[32];       // Storage block for basic signature tracking placeholders
} SystemPackageHeader;

#pragma pack(pop)

/**
 * @brief Parses an update package file to verify its embedded firmware header.
 * 
 * @param filepath Path to the update file on storage.
 * @param out_version Output pointer to store the retrieved firmware integer.
 * @return int 0 on success, negative values represent parsing errors.
 */
int verify_package_version(const char *filepath, uint32_t *out_version);

/**
 * @brief Scans active system mount paths for standard update storage folder directories.
 * 
 * @param out_path Output buffer to store the valid file path string if found.
 * @param max_len Maximum capacity size of the output string buffer.
 * @param detected_fw Output pointer to hold the confirmed firmware version.
 * @return int Returns 1 if a target package is matched and validated, 0 otherwise.
 */
int scan_storage_for_updates(char *out_path, size_t max_len, uint32_t *detected_fw);

#endif // PAYLOAD_H
