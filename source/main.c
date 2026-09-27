#define DEBUG_IP "192.168.2.2"
#define DEBUG_PORT 9090  // Synchronized to standard GoldHEN BinLoader port

#include <ps4.h>

#define SPOOF 0x82

#ifdef DEBUG_SOCKET
int DEBUG_SOCK;
#endif

int _main(struct thread *td) {
  UNUSED(td);

  initKernel();
  initLibc();

#ifdef DEBUG_SOCKET
  initNetwork();
  struct sockaddr_in server;
  server.sin_len = sizeof(server);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = sceNetHtonl(IP(192, 168, 2, 2)); 
  server.sin_port = sceNetHtons(DEBUG_PORT);
  DEBUG_SOCK = sceNetSocket("debug_sock", AF_INET, SOCK_STREAM, 0);
  sceNetConnect(DEBUG_SOCK, (struct sockaddr *)&server, sizeof(server));
#endif

  // Escalates processing runtime authorization flags
  jailbreak();
  
  // Custom execution offset logic mapping 
  spoof_target_id(SPOOF);

  initSysUtil();

  // Throws an on-screen display notification to the user interface
  notify("Spoofing Target ID: 0x%02x!", SPOOF);

#ifdef DEBUG_SOCKET
  sceNetSocketClose(DEBUG_SOCK);
#endif

  return 0;
}
