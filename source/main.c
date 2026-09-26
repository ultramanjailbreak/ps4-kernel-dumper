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
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return 1;

    // Allow quick reuse of the port local socket mapping bindings
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Listen on all network adapters
    address.sin_port = htons(PAYLOAD_PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 3) < 0) {
        close(server_fd);
        return 1;
    }

    // 4. Infinite loop: Keep running in the background waiting for your HTML button presses
    while (1) {
        struct sockaddr_in client_addr;
        int addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t*)&addr_len);
        
        if (client_fd >= 0) {
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            
            // Read incoming text string transmitted from the web browser
            int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                // Trigger the notification display instantly with the exact incoming HTML text
                printf_notification(buffer);
            }
            close(client_fd);
        }
        
        // Minor rest context prevents CPU core locking
        sceKernelUsleep(50000); 
    }

    close(server_fd);
    return 0;
}
