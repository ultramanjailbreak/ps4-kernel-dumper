#include "ps4.h"

#define LOADER_PORT 6767
#define MAX_PAYLOAD_SIZE (10 * 1024 * 1024) // 10MB Maximum Payload Buffer

int _main(struct thread *td) {
  UNUSED(td);

  // Initialize essential system libraries
  initKernel();
  initLibc();
  initNetwork();
  initSysUtil();

  // Escape sandbox to allow network socket operations and custom code execution
  jailbreak();

  // Resolve local PS4 IP address
  struct in_addr ip_addr;
  char ip_string[32] = {0};
  
  if (sceNetGetIovec != NULL) { // Verify networking subsystem stability
     struct sceNetInAddr local_ip;
     // Retrieve default interface information
     sceNetGetMacAddress(NULL, &local_ip); 
     unsigned char *ip = (unsigned char *)&local_ip.s_addr;
     snprintf(ip_string, sizeof(ip_string), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  } else {
     snprintf(ip_string, sizeof(ip_string), "PS4 IP");
  }

  // Broadcast Active Listener Status notification
  char notify_msg[256];
  snprintf(notify_msg, sizeof(notify_msg), "BINLOADER ACTIVE AT %s:%d\nAwaiting payload file...", ip_string, LOADER_PORT);
  printf_notification(notify_msg);

  // Setup Server Socket
  struct sockaddr_in server, client;
  unsigned int client_len = sizeof(client);

  int server_fd = sceNetSocket("binloader_srv", AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
      printf_notification("Failed to create network socket!");
      return -1;
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = sceNetHtons(LOADER_PORT);

  if (sceNetBind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
      printf_notification("Bind error! Port 6767 busy.");
      sceNetSocketClose(server_fd);
      return -1;
  }

  sceNetListen(server_fd, 1);

  // Await Incoming Client Injection Connections
  int client_fd = sceNetAccept(server_fd, (struct sockaddr *)&client, &client_len);
  if (client_fd < 0) {
      printf_notification("Accept connection failed.");
      sceNetSocketClose(server_fd);
      return -1;
  }

  printf_notification("Payload received! Processing injection...");

  // Allocate read-write execution block for payloads (.bin / .elf payload space)
  void *payload_buffer = mmap(NULL, MAX_PAYLOAD_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (payload_buffer == MAP_FAILED) {
      printf_notification("Failed memory allocation for payload!");
      sceNetSocketClose(client_fd);
      sceNetSocketClose(server_fd);
      return -1;
  }

  size_t total_received = 0;
  int read_bytes = 0;
  char *ptr = (char *)payload_buffer;

  // Stream data over socket until end-of-file (EOF)
  while ((read_bytes = sceNetRecv(client_fd, ptr + total_received, 4096, 0)) > 0) {
      total_received += read_bytes;
      if (total_received >= MAX_PAYLOAD_SIZE) {
          printf_notification("Payload is oversized! Dropping socket.");
          break;
      }
  }

  // Housekeeping socket termination
  sceNetSocketClose(client_fd);
  sceNetSocketClose(server_fd);

  if (total_received > 0) {
      printf_notification("Executing payload injection!");
      
      // Inline execution branch shift over to our loaded assembly stream
      void (*payload_entry)(struct thread *) = (void (*)(struct thread *))payload_buffer;
      payload_entry(td); 
  }

  // Cleanup dynamic address allocation blocks
  munmap(payload_buffer, MAX_PAYLOAD_SIZE);
  return 0;
}
