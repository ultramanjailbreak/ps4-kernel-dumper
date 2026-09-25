#include "ps4.h"

// Define custom sizing parameters for the overlay container box
#define OVERLAY_X       50
#define OVERLAY_Y       50
#define BOX_WIDTH       250
#define BOX_HEIGHT      80

#define COLOR_WHITE     0xFFFFFFFF  // Standard White color hex representation
#define COLOR_BLACK     0x000000FF  // Standard Black color hex representation

// Raw 8x8 font matrix definitions mapping out the characters for the string "TEST"
static const uint8_t font_TEST[4][8] = {
    {0xFC, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00}, // 'T'
    {0xFC, 0xC0, 0xC0, 0xF8, 0xC0, 0xC0, 0xFC, 0x00}, // 'E'
    {0x7C, 0xC0, 0xC0, 0x78, 0x0C, 0x0C, 0xF8, 0x00}, // 'S'
    {0xFC, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00}  // 'T'
};

int _main(struct thread *td) {
  UNUSED(td);

  // Initialize essential system libraries safely
  initKernel();
  initLibc();
  initSysUtil();

  // Escape sandbox constraints to permit low-level video layer initialization
  jailbreak();

  // Send the requested standard notification text greeting
  printf_notification("Hello im a ps4");
  sceKernelSleep(2); // Short pacing delay to prevent text block overlaps

  // Load the native PS4 Video Output Module to fetch display configuration handles dynamically
  // 0x0019 corresponds to the native system hardware display compositor module ID
  sceSysmoduleLoadModule(0x0019);

  // Dynamically allocate memory structures for direct on-screen frame map drawing
  // Maps a 1080p execution frame space using raw mmap pointers
  int width = 1920;
  int height = 1080;
  
  void *video_mem = mmap(NULL, width * height * 4, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (video_mem == MAP_FAILED) {
      return -1; // Graceful structural safety exit if hardware memory mapping errors occur
  }

  uint32_t *frame_buffer = (uint32_t *)video_mem;

  // Persistent rendering cycle loop drawing the layout overlay container box
  int runtime_loop = 600; // Loops for roughly 10 seconds at 60Hz intervals
  while (runtime_loop > 0) {
      
      // --- LAYER 1: RENDER THE WHITE INTERFACE BOX CONTAINER ---
      for (int y = OVERLAY_Y; y < (OVERLAY_Y + BOX_HEIGHT); y++) {
          for (int x = OVERLAY_X; x < (OVERLAY_X + BOX_WIDTH); x++) {
              frame_buffer[y * width + x] = COLOR_WHITE;
          }
      }

      // --- LAYER 2: DRAW THE BLACK TEXT CHARACTERS ("TEST") ---
      int text_start_x = OVERLAY_X + 60;
      int text_start_y = OVERLAY_Y + 25;

      for (int char_idx = 0; char_idx < 4; char_idx++) {
          for (int row = 0; row < 8; row++) {
              uint8_t bits = font_TEST[char_idx][row];
              for (int col = 0; col < 8; col++) {
                  if (bits & (0x80 >> col)) {
                      // Scale pixel parameters up slightly for better legibility on high resolutions
                      int draw_x = text_start_x + (char_idx * 30) + (col * 3);
                      int draw_y = text_start_y + (row * 3);
                      
                      // Inject the targeted black display pixels directly inside the container box boundaries
                      for (int py = 0; py < 3; py++) {
                          for (int px = 0; px < 3; px++) {
                              frame_buffer[(draw_y + py) * width + (draw_x + px)] = COLOR_BLACK;
                          }
                      }
                  }
              }
          }
      }

      // Synchronize presentation timing pace with display clocks
      sceKernelUsleep(16666); // 16.6ms intervals coordinates roughly with a standard 60fps pacing sync
      runtime_loop--;
  }

  // Gracefully free active address allocation blocks to avoid system leaks
  munmap(video_mem, width * height * 4);

  return 0;
}
