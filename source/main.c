#include <ps4.h>

#define UNUSED(x) (void)(x)
#define PAYLOAD_PORT 9090

int _main(void *payload_args) {
    UNUSED(payload_args);

    // 1. Initialize system library hooks
    initKernel();
    initLibc();
    
    // 2. Explicitly initialize the native PlayStation Network subsystem
    initNetwork();

    // 3. Alert the user that the binary listener is live
    printf_notification("ACTIVATED");

    // 4. Set up a network server socket
    int server_fd = sceNetSocket("server_socket", AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return 1;

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_len = sizeof(address); // Explicitly declare size for BSD sockets
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = IN_ADDR_ANY; 
    address.sin_port = sceNetHtons(PAYLOAD_PORT); // Convert port safely using SDK macro

    if (sceNetBind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        sceNetSocketClose(server_fd);
        return 1;
    }

    if (sceNetListen(server_fd, 10) < 0) {
        sceNetSocketClose(server_fd);
        return 1;
    }

    // 5. Background Loop
    while (1) {
        struct sockaddr_in client_addr;
        unsigned int addr_len = sizeof(client_addr);
        int client_fd = sceNetAccept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        
        if (client_fd >= 0) {
            char buffer[512];
            memset(buffer, 0, sizeof(buffer));
            
            int bytes_read = sceNetRecv(client_fd, buffer, sizeof(buffer) - 1, 0);
            if (bytes_read > 0) {
                // Parse standard HTTP requests if sent via a web browser button click
                // Look for the POST body payload or standard GET endpoint text
                char *msg_body = strstr(buffer, "\r\n\r\n");
                if (msg_body != NULL) {
                    msg_body += 4; // Skip HTTP header padding to reach raw text
                    if (strlen(msg_body) > 0) {
                        printf_notification(msg_body);
                    } else {
                        printf_notification("SPOOFED TO 9.00");
                    }
                } else {
                    printf_notification("SPOOFED TO 9.00");
                }

                // Send back a clean HTTP 200 OK statement to satisfy the browser client
                const char *http_response = 
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n"
                    "Access-Control-Allow-Origin: *\r\n" // Bypasses browser CORS rules
                    "Content-Length: 2\r\n\r\n"
                    "OK";
                sceNetSend(client_fd, http_response, strlen(http_response), 0);
            }
            sceNetSocketClose(client_fd);
        }
        
        sceKernelUsleep(20000); 
    }

    sceNetSocketClose(server_fd);
    return 0;
}
