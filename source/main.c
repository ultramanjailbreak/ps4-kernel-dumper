#define DEBUG_IP "192.168.2.2"
#define DEBUG_PORT 9023

// Include standard Scene-Collective PS4 library headers
#include <ps4.h>

#define SPOOF 0x82

#ifdef DEBUG_SOCKET
int DEBUG_SOCK;
#endif

int _main(struct thread *td) {
  UNUSED(td);

  // Scene-Collective initialization functions mapping
  initKernel();
  initLibc();

#ifdef DEBUG_SOCKET
  initNetwork();
  struct sockaddr_in server;
  server.sin_len = sizeof(server);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = sceNetHtonl(IP(192, 168, 2, 2)); // Or parse DEBUG_IP
  server.sin_port = sceNetHtons(DEBUG_PORT);
  DEBUG_SOCK = sceNetSocket("debug_sock", AF_INET, SOCK_STREAM, 0);
  sceNetConnect(DEBUG_SOCK, (struct sockaddr *)&server, sizeof(server));
#endif

  // Kernel modification execution
  jailbreak();
  
  // Note: Ensure your custom platform offsets/functions for 'spoof_target_id' 
  // are included or accessible if they are missing from your base SDK version.
  spoof_target_id(SPOOF);

  initSysUtil();

  // Displays native notification box on the PS4 UI
  notify("Spoofing Target ID: 0x%02x!", SPOOF);

#ifdef DEBUG_SOCKET
  sceNetSocketClose(DEBUG_SOCK);
#endif

  return 0;
}
