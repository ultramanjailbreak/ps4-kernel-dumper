#include "ps4.h"

int _main(struct thread *td) {
  UNUSED(td);

  initKernel();
  initLibc();
  initSysUtil();

  printf_notification("Hello im here");

  return 0;
}
