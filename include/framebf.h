// ----------------------------------- framebf.h -------------------------------------
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// ======================
// DMA Framebuffer System
// ======================

#define FRAMEBUFFER_DMA_CHANNEL 5
#define MAX_BATCH_ITEMS 128
#define BYTES_PER_PIXEL 4  // Assuming 32-bit color (RGBA)

typedef struct {
    void* front_buffer;
    void* back_buffer;
    unsigned int width;
    unsigned int height;
    unsigned int pitch;
} dma_framebuffer;

void framebf_init();
void drawPixelARGB32(int x, int y, unsigned int attr);
void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr, int fill);
void drawImage(int x, int y, const unsigned long *image, int image_width, int image_height);
void drawImageScaled(int x, int y, const unsigned long *image, int src_width, int src_height, int dest_width, int dest_height);
void drawImageScaledAspect(int x, int y, const unsigned long *image, int src_width, int src_height, int max_width, int max_height);
void swap_buffers();
void clear_screen(unsigned long color);
unsigned char *get_drawing_buffer();
void drawPixelARGB32_double_buffering(int x, int y, unsigned int attr);
void drawImage_double_buffering(int x, int y, const unsigned long *image, int image_width, int image_height);
void drawImage_dma(int x, int y, const unsigned long *image, int image_width, int image_height);
void draw_rect_double_buffering(int x, int y, int width, int height, unsigned int color);
void drawRectARGB32_double_buffering(int x1, int y1, int x2, int y2, unsigned int attr, int fill);
void drawImage_double_buffering_stride(int x, int y, const unsigned long *image, int image_width, int image_height, int image_stride);

void* get_framebuffer_address(int buffer_index);
void dma_fb_init(dma_framebuffer* fb, unsigned long width, unsigned long height);
// void dma_fb_clear(dma_framebuffer* fb, unsigned int color);
void dma_fb_copy(dma_framebuffer* fb, void* src,
                 unsigned long x, unsigned long y,
                 unsigned long width, unsigned long height,
                 unsigned long src_stride);
void dma_fb_swap(dma_framebuffer* fb); 