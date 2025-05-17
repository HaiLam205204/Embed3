// ----------------------------------- framebf.c -------------------------------------
#include "../../include/mbox.h"
#include "../../include/uart0.h"
#include "../../include/framebf.h"

// Use RGBA32 (32 bits for each pixel)
#define COLOR_DEPTH 32

// Pixel Order: BGR in memory order (little endian --> RGB in byte order)
#define PIXEL_ORDER 0

// Screen info
unsigned int width, height, pitch;
unsigned int buffer_size;

/* Frame buffer address
 * (declare as pointer of unsigned char to access each byte) */
unsigned char *fb;
unsigned char *front_buffer;
unsigned char *back_buffer;
static int current_buffer = 0; // 0 = front buffer visible, 1 = back buffer visible

/**
 * Set screen resolution to 1024x768
 */
void framebf_init()
{
    mBuf[0] = 35 * 4; // Length of message in bytes
    mBuf[1] = MBOX_REQUEST;

    mBuf[2] = MBOX_TAG_SETPHYWH; // Set physical width-height
    mBuf[3] = 8;                 // Value size in bytes
    mBuf[4] = 0;                 // REQUEST CODE = 0
    mBuf[5] = 1024;              // Value(width)
    mBuf[6] = 768;               // Value(height)

    mBuf[7] = MBOX_TAG_SETVIRTWH; // Set virtual width-height
    mBuf[8] = 8;
    mBuf[9] = 0;
    mBuf[10] = 1024;
    mBuf[11] = 768 * 2;

    mBuf[12] = MBOX_TAG_SETVIRTOFF; // Set virtual offset
    mBuf[13] = 8;
    mBuf[14] = 0;
    mBuf[15] = 0; // x offset
    mBuf[16] = 0; // y offset

    mBuf[17] = MBOX_TAG_SETDEPTH; // Set color depth
    mBuf[18] = 4;
    mBuf[19] = 0;
    mBuf[20] = COLOR_DEPTH; // Bits per pixel

    mBuf[21] = MBOX_TAG_SETPXLORDR; // Set pixel order
    mBuf[22] = 4;
    mBuf[23] = 0;
    mBuf[24] = PIXEL_ORDER;

    mBuf[25] = MBOX_TAG_GETFB; // Get frame buffer
    mBuf[26] = 8;
    mBuf[27] = 0;
    mBuf[28] = 16; // alignment in 16 bytes
    mBuf[29] = 0;  // will return Frame Buffer size in bytes

    mBuf[30] = MBOX_TAG_GETPITCH; // Get pitch
    mBuf[31] = 4;
    mBuf[32] = 0;
    mBuf[33] = 0; // Will get pitch value here

    mBuf[34] = MBOX_TAG_LAST;

    // First buffer allocation
    if (!mbox_call(ADDR(mBuf), MBOX_CH_PROP) ||
        mBuf[20] != COLOR_DEPTH ||
        mBuf[24] != PIXEL_ORDER ||
        mBuf[28] == 0)
    {
        uart_puts("Failed to allocate front buffer!\n");
        return;
    }

    // Convert GPU address to ARM address
    mBuf[28] &= 0x3FFFFFFF;
    front_buffer = (unsigned char *)((unsigned long)mBuf[28]);

    // Get returned parameters
    buffer_size = mBuf[29]; // Total buffer size
    width = mBuf[5];        // Physical width
    height = mBuf[6];       // Physical height
    pitch = mBuf[33];       // Bytes per line

    // Verify buffer size is sufficient for double buffering
    unsigned int min_required_size = pitch * height * 2;
    if (buffer_size < min_required_size)
    {
        uart_puts("Error: Allocated buffer too small for double buffering\n");
        return;
    }

    // Set up back buffer
    // After mailbox call, check ACTUAL virtual height (mBuf[11])
    if (mBuf[11] >= height * 2)
    {
        back_buffer = front_buffer + (pitch * height);
        uart_puts("Double buffering enabled\n");

        unsigned int expected_buffer_size = pitch * height;
        if ((back_buffer - front_buffer) != expected_buffer_size)
        {
            uart_puts("Warning: Buffer spacing unexpected\n");
        }
        else
        {
            uart_puts("Buffer spacing is correct\n");
        }

        // Safety check
        if ((unsigned long)back_buffer + expected_buffer_size >
            (unsigned long)front_buffer + buffer_size)
        {
            uart_puts("Error: Back buffer exceeds allocated memory!\n");
        }
        else
        {
            uart_puts("Back buffer is within allocated memory\n");
        }
    }
    else
    {
        uart_puts("Warning: Couldn't get double height buffer\n");
        back_buffer = front_buffer; // Fallback to single buffer
    }
    fb = front_buffer; // Display starts with front buffer

    uart_puts("FB Init: ");
    uart_dec(width);
    uart_puts("x");
    uart_dec(height);
    uart_puts(" at ");
    uart_hex((unsigned long)front_buffer);
    uart_puts(", width=");
    uart_dec(width);
    uart_puts(", height=");
    uart_dec(height);
    uart_puts(", pitch=");
    uart_dec(pitch);
    uart_puts(", size=");
    uart_dec(buffer_size);
    uart_puts("\n");
    uart_puts("Front buffer at: ");
    uart_hex((unsigned long)front_buffer);
    uart_puts("\nBack buffer at: ");
    uart_hex((unsigned long)back_buffer);
    uart_puts("\nBuffer size: ");
    uart_dec(buffer_size);
    uart_puts(" bytes\n");
}

void drawPixelARGB32(int x, int y, unsigned int attr)
{
    int offs = (y * pitch) + (COLOR_DEPTH / 8 * x);

    /*	//Access and assign each byte
     *(fb + offs    ) = (attr >> 0 ) & 0xFF; //BLUE  (get the least significant byte)
     *(fb + offs + 1) = (attr >> 8 ) & 0xFF; //GREEN
     *(fb + offs + 2) = (attr >> 16) & 0xFF; //RED
     *(fb + offs + 3) = (attr >> 24) & 0xFF; //ALPHA
     */

    // Access 32-bit together
    *((unsigned int *)(fb + offs)) = attr;
}

void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr, int fill)
{
    for (int y = y1; y <= y2; y++)
        for (int x = x1; x <= x2; x++)
        {
            if ((x == x1 || x == x2) || (y == y1 || y == y2))
                drawPixelARGB32(x, y, attr);
            else if (fill)
                drawPixelARGB32(x, y, attr);
        }
}

// Function to draw line
void drawLine(int x1, int y1, int x2, int y2, unsigned int attr)
{
    for (int x = x1; x <= x2; x++)
    {
        int y = (float)(y1 - y2) / (x1 - x2) * (x - x1) + y1;
        drawPixelARGB32(x, y, attr);
    }
}

// Function to calculate the square root of a number using the Newton-Raphson method
double sqrt(double number)
{
    if (number < 0)
    {
        return -1; // Return -1 for negative inputs as square root of negative is not defined in real numbers
    }

    double tolerance = 0.000001; // Define the tolerance for the result
    double guess = number / 2.0; // Initial guess (can be any positive number, here half of the number)
    double result = 0.0;

    while (1)
    {
        result = 0.5 * (guess + number / guess); // Calculate the next approximation

        // Check if the difference between the current guess and the new result is within the tolerance
        int diff = (result > guess) ? (result - guess) : (guess - result);
        if (diff < tolerance)
        {
            break;
        }

        guess = result; // Update the guess for the next iteration
    }

    return result;
}

// Function to draw circle
void drawLCircle(int center_x, int center_y, int radius, unsigned int attr, int fill)
{
    // Draw the circle when going on x side
    for (int x = center_x - radius; x <= center_x + radius; x++)
    {
        // Calculate the corresponding y values using the circle equation
        int dy = sqrt(radius * radius - (x - center_x) * (x - center_x));
        int upper_y = center_y + dy;
        int lower_y = center_y - dy;

        drawPixelARGB32(x, upper_y, attr);
        drawPixelARGB32(x, lower_y, attr);

        // Fill the circle, draw a line between lower_y and upper_y
        if (fill)
        {
            for (int y = lower_y; y <= upper_y; y++)
            {
                drawPixelARGB32(x, y, attr);
            }
        }
    }

    /* Also draw the circle border when going on y side (
    since some points may be missing due to inaccurate calculation above) */

    for (int y = center_y - radius; y <= center_y + radius; y++)
    {
        // Calculate the corresponding x values using the circle equation
        int dx = sqrt(radius * radius - (y - center_y) * (y - center_y));

        int left_x = center_x - dx;
        int right_x = center_x + dx;

        drawPixelARGB32(left_x, y, attr);
        drawPixelARGB32(right_x, y, attr);
    }
}

void drawImage(int x, int y, const unsigned long *image, int image_width, int image_height)
{
    for (int j = 0; j < image_height; j++)
    {
        for (int i = 0; i < image_width; i++)
        {
            drawPixelARGB32(x + i, y + j, image[j * image_width + i]);
        }
    }
}

/**
 * Draws an image with arbitrary scaling using nearest-neighbor interpolation
 */
void drawImageScaled(int x, int y, const unsigned long *image, int src_width, int src_height, int dest_width, int dest_height)
{
    // Fixed-point scaling factors (16.16 format)
    // These represent (src_dimension/dest_dimension) ratio as fixed-point numbers
    uint32_t x_scale = ((uint32_t)src_width << 16) / dest_width; // value << 16 converts to fixed-point
    uint32_t y_scale = ((uint32_t)src_height << 16) / dest_height;

    // Loop through every pixel in destination image
    for (int j = 0; j < dest_height; j++)
    {
        // Calculate corresponding Y position in source image (fixed-point to integer)
        uint32_t src_y = (j * y_scale) >> 16; // value >> 16 converts back to integer
        for (int i = 0; i < dest_width; i++)
        {
            // Calculate corresponding X position in source image (fixed-point to integer)
            uint32_t src_x = (i * x_scale) >> 16;
            // Get pixel from source image (nearest-neighbor interpolation)
            unsigned long pixel = image[src_y * src_width + src_x];
            drawPixelARGB32(x + i, y + j, pixel); // Draw the pixel at calculated position
        }
    }
}

/**
 * Draws an image scaled to fit within specified maximum dimensions while maintaining aspect ratio, better for random resolution
 */
void drawImageScaledAspect(int x, int y, const unsigned long *image, int src_width, int src_height, int max_width, int max_height)
{
    // Calculate aspect ratio using fixed-point (16.16 format)
    // Shift left by 16 bits converts to fixed-point, division preserves ratio
    uint32_t aspect_ratio = ((uint32_t)src_width << 16) / src_height;

    int dest_width, dest_height; // Hold final scaled dimensions

    // Determine if we should scale based on width or height constraints:
    // Compare (max_width/aspect_ratio) with max_height using fixed-point math
    if ((max_width << 16) / aspect_ratio <= max_height)
    {
        // Width-constrained scaling: use full available width
        dest_width = max_width;
        // Calculate proportional height (convert back from fixed-point)
        dest_height = (max_width << 16) / aspect_ratio;
    }
    else
    {
        // Height-constrained scaling: use full available height
        dest_height = max_height;
        // Calculate proportional width (fixed-point multiply then shift back)
        dest_width = (max_height * aspect_ratio) >> 16;
    }

    // Center the image within the available space:
    // Add half of remaining space to original coordinates
    x += (max_width - dest_width) >> 1; // Fast division by 2
    y += (max_height - dest_height) >> 1;

    // Draw with scaling (using nearest-neighbor for efficiency)
    drawImageScaled(x, y, image, src_width, src_height, dest_width, dest_height);
}

/**
 * Swap front and back buffers
 */
void swap_buffers()
{

    if (back_buffer == front_buffer)
    {
        uart_puts("Warning: No double buffering available\n");
        return;
    }

    // Update virtual offset to switch buffers
    mBuf[0] = 7 * 4;
    mBuf[1] = MBOX_REQUEST;
    mBuf[2] = MBOX_TAG_SETVIRTOFF;
    mBuf[3] = 8; // Tag payload size
    mBuf[4] = 0; // Request code
    mBuf[5] = 0; // X offset always 0
    // Toggle between 0 (front) and height (back)
    mBuf[6] = (fb == front_buffer) ? height : 0; // Toggle Y offset
    mBuf[7] = MBOX_TAG_LAST;

    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP))
    {
        fb = (fb == front_buffer) ? back_buffer : front_buffer;
    }
    else
    {
        uart_puts("Failed to swap buffers!\n");
    }

    current_buffer = !current_buffer; // Toggle buffer flag
    // Check message
    // uart_puts("Swapped to buffer ");
    // uart_dec(current_buffer);
    // uart_puts("\n");
}

// Clear entire back buffer
void clear_screen(unsigned long color)
{
    unsigned long *buf = (unsigned long *)get_drawing_buffer();
    for (int i = 0; i < (pitch / 4) * height; i++)
    {
        buf[i] = color;
    }
}

/**
 * Get pointer to the NOT currently displayed drawing at (back) buffer
 */
unsigned char *get_drawing_buffer()
{
    return (fb == front_buffer) ? back_buffer : front_buffer;
}

/**
 * Double buffering draw function, writes to back buffer
 */
void drawPixelARGB32_double_buffering(int x, int y, unsigned int attr)
{
    // Calculate offset (same as before)
    int offs = (y * pitch) + (COLOR_DEPTH / 8 * x);

    // Draw to the CURRENTLY ACTIVE back buffer (not the displayed one)
    unsigned char *draw_buffer = get_drawing_buffer();
    *((unsigned int *)(draw_buffer + offs)) = attr;
}

void drawImage_double_buffering(int x, int y, const unsigned long *image, int image_width, int image_height)
{
    for (int j = 0; j < image_height; j++)
    {
        for (int i = 0; i < image_width; i++)
        {
            unsigned int pixel = image[j * image_width + i];
            if ((pixel & 0x00FFFFFF) != 0)
            { // skip transparent pixels
                drawPixelARGB32_double_buffering(x + i, y + j, pixel);
            }
            // drawPixelARGB32_double_buffering(x + i, y + j, image[j * image_width + i]);
        }
    }
}

void draw_rect_double_buffering(int x, int y, int width, int height, unsigned int color) {
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            drawPixelARGB32_double_buffering(x + i, y + j, color);
        }
    }
}









void drawRectARGB32_double_buffering(int x1, int y1, int x2, int y2, unsigned int attr, int fill)
{
    for (int y = y1; y <= y2; y++)
        for (int x = x1; x <= x2; x++)
        {
            if ((x == x1 || x == x2) || (y == y1 || y == y2))
                drawPixelARGB32_double_buffering(x, y, attr);
            else if (fill)
                drawPixelARGB32_double_buffering(x, y, attr);
        }
}
