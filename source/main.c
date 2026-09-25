#include "ps4.h"

// Configuration constants for the interface window
#define WINDOW_WIDTH  550
#define WINDOW_HEIGHT 150
#define COLOR_PINK    0x99B469FF  // RGBA: Red, Green, Blue, Alpha (0x99 alpha gives approx 40% transparency)
#define COLOR_WHITE   0xFFFFFFFF  // RGBA: Solid White for legible text

int _main(struct thread *td) {
  UNUSED(td);

  // Initialize essential core system wrappers
  initKernel();
  initLibc();
  initSysUtil();

  // 1. Broadcast the initial system notification
  printf_notification("Loading GayHelper 6.9");

  // Escaping sandbox limitations to request hardware access for the video layer
  jailbreak();

  // 2. Initialize the PS4 2D Graphics Engine context
  initGraphics();

  // Fetch frame buffer specifications directly from the active engine state
  int canvas_width, canvas_height;
  get_screen_size(&canvas_width, &canvas_height);
  
  // Display overlay loop counter (runs for roughly 5 seconds at 60fps)
  int frames = 300; 
  while(frames > 0) {
      
      // Retrieve the current memory address pointer for the active draw canvas frame
      uint32_t *frame_buffer = (uint32_t *)get_buffer_address();
      
      // Render Logic: Draw the window box container in the upper left corner
      for (int y = 50; y < (50 + WINDOW_HEIGHT); y++) {
          for (int x = 50; x < (50 + WINDOW_WIDTH); x++) {
              // Map 2D coordinates directly onto the linear 1D memory array of the active frame buffer
              frame_buffer[y * canvas_width + x] = COLOR_PINK;
          }
      }

      // Font Engine Logic: Draw active strings natively into the display layer
      draw_string(70, 80, "THIS PS4 IS GAY", COLOR_WHITE, COLOR_PINK);
      draw_string(70, 110, "expiration date: never bc ur ps4 is gay now", COLOR_WHITE, COLOR_PINK);

      // Flip/update screen frames to push raw pixels into display visibility 
      flipGraphics();

      // Synchronize video output timing with the display refresh clock (V-Blank)
      sceKernelUsleep(16666); 
      frames--;
  }

  // Graceful cleanup: Release the graphics hardware hooks back to the operating system
  endGraphics();

  return 0;
}
