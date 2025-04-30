#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "mbox.h"
#include "framebf.h"
#include "utils.h"
#include "CLI.h"
#include "welcomeScreen.h"
#include "video.h"
#include "renderFrame.h"

#define OPEN_CLI 0x14 // CTRL T
#define VIDEO 0x16 // CTRL V

void main()
{
    int start_x = 0;
    int start_y = 0;
    // set up serial console
	uart_init();

	// say hello
	uart_puts("\n\nHello World!\n");

	// Initialize frame buffer
	framebf_init();

	// echo everything back
	// Create a basic CLI window popup when a specific key is pressed (e.g., 'C')

    // Draw initial image
    drawImage(start_x, start_y, welcome_image, WELCOME_WIDTH, WELCOME_HEIGHT);

    while(1) {
        // Read character from UART
        char c = uart_getc();
        uart_sendc(c);  // Echo character back

        if (c == OPEN_CLI) {
            // Trigger the CLI popup window
            draw_cli_window();
            cli_loop();  // Start handling input in the CLI window
		} else if (c == VIDEO) {
            video_playback(video_allArray, video_allArray_LEN, start_x, start_y, VIDEO_WIDTH, VIDEO_HEIGHT);
        } else {
            // Handle normal character drawing
            drawInputCharacters(c, 0x00FFFFFF, 1);  // Draw regular characters outside CLI popup
        }
    }
}