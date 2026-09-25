#include "ps4.h"

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

  // Directly call the underlying OS notification function to supply our custom icon and text
  // Parameter 1: Notification type index (0 = standard text notice with icon)
  // Parameter 2: Context text message string
  sceSysUtilSendSystemNotificationWithText(0, "file:///tmp/icon.png\nLoading GayHelper 6.9");

  return 0;
}
