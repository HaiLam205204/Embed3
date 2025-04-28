// ----------------------------------- framebf.h -------------------------------------
void framebf_init(int x_offset, int y_offset);
void drawPixelARGB32(int x, int y, unsigned int attr);
void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr, int fill);
void drawImage(int x, int y, const unsigned long *image, int image_width, int image_height);
