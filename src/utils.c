#include "../include/font.h"          // Contains font bitmap and constants like FONT_HEIGHT, FONT_WIDTH
#include "../include/framebf.h"       // Provides drawPixelARGB32() and framebuffer access
#include "../include/uart0.h" // UART0 definitions (if enabled)
#include "../include/uart1.h" // UART1 definitions (if enabled)

/* 
 * =============================
 * TEXT RENDERING FUNCTIONS
 * =============================
 * These functions allow rendering bitmap font characters and strings
 * to the screen using framebuffer graphics. Useful for displaying CLI
 * output or basic UI on a bare-metal OS with no graphics libraries.
 */

/*
 * drawChar
 * ----------
 * Draws a single character `ch` at screen position (x, y) using a specified
 * color `attr`, scaled by `zoom`.
 *
 * Parameters:
 * - ch: ASCII character to display
 * - x, y: Top-left coordinates of the character
 * - attr: Pixel color in ARGB32 format
 * - zoom: Scaling factor (e.g., zoom = 2 means 2x2 enlargement per pixel)
 *
 * Notes:
 * - Uses bitmap font where each character is a grid of pixels
 * - If zoom == 0, character is not displayed
 * - Uses bitmasking to determine which pixels to draw
 */
void drawChar(unsigned char ch, int x, int y, unsigned int attr, int zoom)
{
    // Get pointer to glyph data for character `ch`
    // If `ch` is out of range, default to glyph 0
    unsigned char *glyph = (unsigned char *)&font + (ch < FONT_NUMGLYPHS ? ch : 0) * FONT_BPG;

    for (int i = 1; i <= (FONT_HEIGHT * zoom); i++) {
        for (int j = 0; j < (FONT_WIDTH * zoom); j++) {
            unsigned char mask = 1 << (j / zoom); // Scale horizontally
            if (*glyph & mask) {
                drawPixelARGB32(x + j, y + i, attr); // Draw pixel with color
            }
        }
        // Only advance to the next glyph row after processing all zoomed copies
        glyph += (i % zoom) ? 0 : FONT_BPL;
    }
}

void drawInputCharacters(char ch, unsigned int attr, int zoom)
{
    static int cursorX = 1;
    static int cursorY = 1;

    if (ch == '\r') {
        cursorX = 1;
    } else if (ch == '\n') {
        cursorX = 1;
        cursorY += FONT_HEIGHT * zoom;
    } else {
        drawChar((unsigned char)ch, cursorX, cursorY, attr, zoom);
        cursorX += FONT_WIDTH * zoom;
    }
}

/*
 * drawString
 * ----------
 * Renders a null-terminated string to the screen, starting at (x, y).
 *
 * Automatically handles:
 * - Newlines ('\n'): move to the next line
 * - Carriage returns ('\r'): reset to start of line
 *
 * Parameters:
 * - x, y: Starting coordinates for the string
 * - str: Pointer to null-terminated ASCII string
 * - attr: Pixel color in ARGB32 format
 * - zoom: Font zoom level
 *
 * Example:
 * drawString(0, 0, "HELLO WORLD", 0x00FF00FF, 1);  // Draw pink text
 */
void drawString(int x, int y, char *str, unsigned int attr, int zoom)
{
    while (*str) {
        if (*str == '\r') {
            x = 0;
        } else if (*str == '\n') {
            x = 0;
            y += (FONT_HEIGHT * zoom); // Move to next line
        } else {
            drawChar(*str, x, y, attr, zoom);       // Draw character
            x += (FONT_WIDTH * zoom);               // Move cursor right
        }
        str++;
    }
}

/*
 * UART INPUT FUNCTIONS
 * ====================
 * getUart:
 * - Checks for incoming UART character (non-blocking)
 * - Returns character if available, or 0 otherwise
 *
 * uart_isReadByteReady:
 * - Hardware-dependent function that checks whether a character is available
 * - Implemented for UART0 or UART1 depending on preprocessor flag
 */

#if 0 // Use UART0
unsigned int uart_isReadByteReady() {
    return ( !(*UART0_FR & UART0_FR_RXFE) ); // RXFE = Receive FIFO Empty
}
#else // Use UART1
unsigned int uart_isReadByteReady() {
    return (AUX_MU_LSR & 0x01); // LSR bit 0 = data ready
}
#endif

unsigned char getUart() {
    unsigned char ch = 0;
    if (uart_isReadByteReady())
        ch = uart_getc(); // Read character from UART
    return ch;
}

/*
 * TIMER UTILITIES
 * ===============
 * wait_us:
 * - Blocks execution for specified number of microseconds
 * - Uses system counter and frequency registers
 *
 * set_wait_timer:
 * - SET mode: Initializes a timer that will expire in `usVal` us
 * - WAIT mode: Blocks until timer expires
 *
 * These functions use ARM system registers:
 * - cntfrq_el0: Counter frequency (Hz)
 * - cntpct_el0: Physical count of system timer
 *
 * Useful for non-blocking wait loops and precise timing
 */

 void wait_us(uint64_t usVal) {
    uint64_t freq, t, r, expiredTime;

    // Get counter frequency (ticks per second)
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(freq));

    // Get current timer count
    asm volatile ("mrs %0, cntpct_el0" : "=r"(t));

    // Calculate expiration time (ticks)
    expiredTime = t + (freq * usVal) / 1000000ULL;  // us → ticks

    // Busy-wait until target time
    do {
        asm volatile ("mrs %0, cntpct_el0" : "=r"(r));
    } while (r < expiredTime);
}


void set_wait_timer(int set, uint64_t usVal) {
    static uint64_t expiredTime = 0;
    uint64_t r, f, t;

    if (set) {
        // Get counter frequency (ticks per second)
        asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
        // Get current timer count in ticks
        asm volatile ("mrs %0, cntpct_el0" : "=r"(t));
        // Calculate expiration time (microseconds → ticks)
        expiredTime = t + (f * usVal) / 1000000ULL;
    } else {
        // Busy-wait until target time
        do {
            asm volatile ("mrs %0, cntpct_el0" : "=r"(r));
        } while (r < expiredTime);
    }
}

const char* welcome_message = 
"8888888888 8888888888 8888888888 88888888888 .d8888b.      d8888   .d8888b.   .d8888b.  \n"
"888        888        888            888    d88P  Y88b    d8P888  d88P  Y88b d88P  Y88b \n"
"888        888        888            888           888   d8P 888  888    888 888    888 \n"
"8888888    8888888    8888888        888         .d88P  d8P  888  Y88b. d888 888    888 \n"
"888        888        888            888     .od888P\"  d88   888   \"Y888P888 888    888 \n"
"888        888        888            888    d88P\"      8888888888        888 888    888 \n"
"888        888        888            888    888\"             888  Y88b  d88P Y88b  d88P \n"
"8888888888 8888888888 8888888888     888    888888888        888   \"Y8888P\"   \"Y8888P\"  \n"
"                                                                                       \n"
"888888b.         d8888 8888888b.  8888888888     .d88888b.   .d8888b.                   \n"
"888  \"88b       d88888 888   Y88b 888           d88P\" \"Y88b d88P  Y88b                  \n"
"888  .88P      d88P888 888    888 888           888     888 Y88b.                       \n"
"8888888K.     d88P 888 888   d88P 8888888       888     888  \"Y888b.                    \n"
"888  \"Y88b   d88P  888 8888888P\"  888           888     888     \"Y88b.                  \n"
"888    888  d88P   888 888 T88b   888           888     888       \"888                  \n"
"888   d88P d8888888888 888  T88b  888           Y88b. .d88P Y88b  d88P                  \n"
"8888888P\" d88P     888 888   T88b 8888888888     \"Y88888P\"   \"Y8888P\"                   \n"
"\n\n"
    "Developed by <NGUYEN HAI LAM> - <S3979802>\n"
    "             <UNG XUAN DAT> - <S3932156>\n"
    "             <NGUYEN DUC ANH> - <S3878010>\n"
    "             <NGUYEN TRONG KHOA> - <S3978477>\n"
    "Ctrl + T to access the CLI, Ctrl + V to run the game\n";                                                  