#include "ps4.h"

// Configuration coordinates for our custom overlay box layout
#define OVERLAY_X       30
#define OVERLAY_Y       30
#define BOX_WIDTH       120
#define BOX_HEIGHT      50

#define COLOR_WHITE     0xFFFFFFFF  // Solid White frame container
#define COLOR_BLACK     0x000000FF  // Solid Black for pixel mapping matrix

// A compact 8x8 font matrix bitmap representation for the string "TEST"
// This provides raw pixel layouts since high-level text engines are unavailable
static const uint8_t font_TEST[4][8] = {
    {0xFC, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00}, // 'T'
    {0xFC, 0xC0, 0xC0, 0xF8, 0xC0, 0xC0, 0xFC, 0x00}, // 'E'
    {0x7C, 0xC0, 0xC0, 0x78, 0x0C, 0x0C, 0xF8, 0x00}, // 'S'
    {0xFC, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00}  // 'T'
};

int _main(struct thread *td) {
  UNUSED(td);

  // Initialize essential core system wrappers
  initKernel();
  initLibc();
  initSysUtil();

  // Elevate system execution parameters to allow memory writing actions
  jailbreak();

  // Initialize the PS4 2D Graphics Canvas structures
  initGraphics();

  // Standard canvas resolution layout mapping references
  int canvas_width = 1920;
  
  // Persistent rendering cycle loop
  // Keeps the graphic overlay fixed on the screen continuously 
  while(1) {
      
      // Target the active screen frame memory address base channel pointer
      uint32_t *frame_buffer = (uint32_t *)sceKernelGetUserspaceBaseAddr();
      if (frame_buffer == NULL) {
          sceKernelSleep(1);
          continue;
      }

      // --- LAYER 1: RENDER THE WHITE BOX OVERLAY ---
      for (int y = OVERLAY_Y; y < (OVERLAY_Y + BOX_HEIGHT); y++) {
          for (int x = OVERLAY_X; x < (OVERLAY_X + BOX_WIDTH); x++) {
              frame_buffer[y * canvas_width + x] = COLOR_WHITE;
          }
      }

      // --- LAYER 2: DRAW THE BLACK TEXT PIXELS ("TEST") ---
      // We start painting text inside our white box boundary padding boundaries
      int text_start_x = OVERLAY_X + 25;
      int text_start_y = OVERLAY_Y + 18;

      for (int char_idx = 0; char_idx < 4; char_idx++) {
          for (int row = 0; row < 8; row++) {
              uint8_t bits = font_TEST[char_idx][row];
              for (int col = 0; col < 8; col++) {
                  // Inspecting bits to locate custom font pixel configurations
                  if ((bits & (0x80 >> col))) {
                      // Scale font visibility sizes up slightly for readability metrics
                      int draw_x = text_start_x + (char_idx * 16) + (col * 2);
                      int draw_y = text_start_y + (row * 2);
                      
                      // Paint custom black font coordinates safely
                      frame_buffer[draw_y * canvas_width + draw_x] = COLOR_BLACK;
                      frame_buffer[draw_y * canvas_width + (draw_x + 1)] = COLOR_BLACK;
                      frame_buffer[(draw_y + 1) * canvas_width + draw_x] = COLOR_BLACK;
                      frame_buffer[(draw_y + 1) * canvas_width + (draw_x + 1)] = COLOR_BLACK;
                  }
              }
          }
      }

      // Pacing pause to regulate video compositing refresh frequencies
      sceKernelSleep(1);
  }

  return 0;
}
