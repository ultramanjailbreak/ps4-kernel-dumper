#include "ps4.h"

// This links to the binary data generated automatically by your compiler tools
extern unsigned char icon_png[];
extern unsigned int icon_png_len;

int _main(struct thread *td) {
  UNUSED(td);

  // Initialize essential system libraries
  initKernel();
  initLibc();
  initSysUtil();

  // Escape the console sandbox environment
  jailbreak();

  // Save the baked-in image into temporary system RAM so the notification system can read it
  int fd = open("/tmp/icon.png", O_WRONLY | O_CREAT | O_TRUNC, 0777);
  if (fd >= 0) {
      write(fd, icon_png, icon_png_len);
      close(fd);
  }

  // Display the notification using the temporary local file path
  // If your SDK version doesn't support the 'notify' function name, 
  // you can change this line back to: printf_notification("Loading GayHelper 6.9");
  notify("file:///tmp/icon.png", "Loading GayHelper 6.9");

  return 0;
}
