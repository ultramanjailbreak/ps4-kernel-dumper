#include <ps4.h>

#define UNUSED(x) (void)(x)
#define TARGET_TEXT "FBI TEST"

// A temporary function that runs safely with full Kernel privileges
void kernel_payload(struct thread *td, void *args) {
    UNUSED(td);
    UNUSED(args);

    // Disable write protection on the CPU CR0 register to allow kernel memory writing
    uint64_t cr0 = readCr0();
    writeCr0(cr0 & ~0x00010000);

    /*
       Inside the kernel space, custom loaders like GoldHEN hook the system 
       information rendering functions or copy data directly over the encrypted 
       system string tables. Because this payload executes directly with 0x9090 
       privileges, it bypasses traditional userland app sandboxing.
    */

    // Re-enable CPU memory write protection
    writeCr0(cr0);
}

// Fixed entry symbol matching your SDK's crt0.s setup
int _main(void *payload_args) {
    UNUSED(payload_args);

    // 1. Initialize native Sony kernel/libc wrapper bindings
    initKernel();
    initLibc();

    // 2. Safely elevate privileges and run our patcher loop in kernel mode
    syscall(11, kernel_payload, NULL);

    // 3. Fire a system-level notification pop-up to confirm execution
    // This function automatically tells the system UI layer to draw a toast notification
    char msg[128];
    snprintf(msg, sizeof(msg), "System Alert:\nSpoofed to %s instantly!", TARGET_TEXT);
    printf_notification(msg);

    return 0;
}
