#include <ps4.h>

#define UNUSED(x) (void)(x)
#define PAYLOAD_PORT 9090

int _main(void *payload_args) {
    UNUSED(payload_args);

    // 1. Initialize system library hooks
    initKernel();
    initLibc();

    // 2. Alert the user that the binary listener is live
    printf_notification("ACTIVATED");

    // 3. Set up a network server socket to listen for incoming HTML commands
    // Note: The PS4 SDK wraps standard socket descriptors via internal sceNet calls
    int server_fd = sceNetSocket("server_socket", AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return 1;

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = IN_ADDR_ANY; // Fixed to use your SDK's exact naming token
    address.sin_port = htons(PAYLOAD_PORT);

    if (sceNetBind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        sceNetSocketClose(server_fd);
        return 1;
    }

    if (sceNetListen(server_fd, 3) < 0) {
        sceNetSocketClose(server_fd);
        return 1;
    }

    // 4. Infinite loop: Keep running in the background waiting for your HTML button presses
    while (1) {
        struct sockaddr_in client_addr;
        unsigned int addr_len = sizeof(client_addr);
        int client_fd = sceNetAccept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        
        if (client_fd >= 0) {
            char buffer[128];
            memset(buffer, 0, sizeof(buffer));
            
            // Read incoming text string transmitted from the web browser
            int bytes_read = sceNetRecv(client_fd, buffer, sizeof(buffer) - 1, 0);
            if (bytes_read > 0) {
                // Trigger the notification display instantly with the exact incoming HTML text
                printf_notification(buffer);
            }
            sceNetSocketClose(client_fd);
        }
        
        // Minor rest context prevents CPU core locking
        sceKernelUsleep(50000); 
    }

    sceNetSocketClose(server_fd);
    return 0;
}
