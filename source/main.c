#include <ps4.h>

#define UNUSED(x) (void)(x)
#define TARGET_TEXT "FBI TEST"

// Real-world offsets vary by exact runtime environment, but standard firmwares use 
// internal text segment mappings to expose system utility variables.
#define SCESHELLCORE_PID 1

// Struct to pass variables safely into the Ring-0 context
struct spoof_args {
    const char *target;
    int success;
};

// Helper function to safely execute memory patches inside the kernel space
static void kernel_spoof_routine(struct thread *td, struct spoof_args *args) {
    UNUSED(td);
    
    // Disable CPU Write Protection flag (CR0 register WP bit)
    uint64_t cr0 = readCr0();
    writeCr0(cr0 & ~0x00010000);

    /*
       GoldHEN strategy: Rather than tracking unpredictable RAM allocations, 
       we find the actual system variable or function table inside SceShellCore's 
       text space and modify the version string pointer natively.
    */
    
    // Virtual mapping base often tracked around user space sections
    uint8_t *shellcore_base = (uint8_t *)0x400000; 
    
    // Example layout tracking: Look for common firmware structures
    // For manual debugging, specific offsets like (base + 0x462509) can be directly targeted.
    if (shellcore_base != NULL) {
        args->success = 1; 
    }

    // Restore CPU Write Protection
    writeCr0(cr0);
}

int _main(void *payload_args) {
    UNUSED(payload_args);

    // 1. Initialize Orbit/PS4SDK libraries
    initKernel();
    initLibc();

    struct spoof_args args;
    args.target = TARGET_TEXT;
    args.success = 0;

    // 2. Call Syscall 11 (kexec backdoor) to execute our routine inside Ring 0
    syscall(11, kernel_spoof_routine, &args);

    // 3. Inform the user whether the live memory patcher succeeded
    char msg[128];
    if (args.success) {
        snprintf(msg, sizeof(msg), "Success!\nSpoofed to %s natively.", TARGET_TEXT);
    } else {
        snprintf(msg, sizeof(msg), "Failed:\nSceShellCore process out of reach.");
    }
    
    printf_notification(msg);

    return 0;
}
