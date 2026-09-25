#include "ps4.h"

// Explicit declaration to fix the compiler warning in older libPS4 versions
extern void printf_notification(const char* fmt, ...);

// Links to the binary data generated automatically by your Makefile tools
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

  // Display the notification using the temporary local file path protocol
  // Passing the path inside brackets allows printf_notification to render the image asset
  printf_notification("file:///tmp/icon.png\nLoading GayHelper 6.9");

  return 0;
}
