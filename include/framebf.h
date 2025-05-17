// ----------------------------------- framebf.h -------------------------------------
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

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
void drawImage_double_buffering_stride(int x, int y, const unsigned long *image, int image_width, int image_height, int image_stride);
