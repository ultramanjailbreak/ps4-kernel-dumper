#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>  // Required for Sleep() on Windows
    #pragma comment(lib, "ws2_32.lib")
    #define sleep_ms(ms) Sleep(ms)
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #define sleep_ms(ms) usleep((ms) * 1000)
#endif

// Configuration parameters
#define PS4_IP        "192.168.10.36"
#define PS4_PORT      744
#define CURRENT_FW    "13.52"
#define TARGET_TEXT   "FBI TEST"  // Updated text string to spoof into memory

void close_socket(int sock) {
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}

int main() {
    printf("shitspoof C-Edition 0.0001: never update edition\n");
    printf("============================================================\n");
    printf("[*] Preparation: Please get ready to navigate on your PS4.\n");
    printf("============================================================\n\n");

    // 10-second visual countdown delay
    for (int i = 10; i > 0; i--) {
        printf("[!] Waiting %d seconds... Open 'System Information' on your PS4 now!\n", i);
        fflush(stdout); // Forces immediate print to the terminal screen
        sleep_ms(1000); 
    }
    printf("\n[*] Time's up! Starting spoof configuration: HEN %s -> %s\n", CURRENT_FW, TARGET_TEXT);

#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("[!] Winsock initialization failed.\n");
        return 1;
    }
#endif

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("[!] Failed to create socket.\n");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PS4_PORT);
    inet_pton(AF_INET, PS4_IP, &server_addr.sin_addr);

    printf("[*] Connecting to ps4debug on %s:%d...\n", PS4_IP, PS4_PORT);
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("[!] Connection failed. Is ps4debug running on your console?\n");
        close_socket(sock);
        return 1;
    }
    printf("[+] Connected successfully!\n");

    // Format target buffers
    char old_text[64];
    char new_text[64];
    snprintf(old_text, sizeof(old_text), "HEN %s", CURRENT_FW);
    snprintf(new_text, sizeof(new_text), "%s", TARGET_TEXT);

    printf("[*] Scanning memory architecture sections for '%s'...\n", old_text);
    printf("[*] Replacing matches with raw text layout: '%s'\n", new_text);
    printf("[!] Mock execution completed over socket framework.\n");

    close_socket(sock);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
