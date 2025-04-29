#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "mbox.h"
#include "framebf.h"
#include "utils.h"
#include "CLI.h"
#include "welcomeScreen.h"

#define OPEN_CLI 0x14 // CTRL T

int image_x = 0;
int image_y = 0;

void main()
{
    // set up serial console
	uart_init();

	// say hello
	uart_puts("\n\nHello World!\n");

	// Initialize frame buffer
	framebf_init();

	// echo everything back
	// Create a basic CLI window popup when a specific key is pressed (e.g., 'C')

    // Draw initial image
    drawImage(image_x, image_y, welcome_image, WELCOME_WIDTH, WELCOME_HEIGHT);

    while(1) {
        // Read character from UART
        char c = uart_getc();
        uart_sendc(c);  // Echo character back

        if (c == OPEN_CLI) {
            // Trigger the CLI popup window
            draw_cli_window();
            cli_loop();  // Start handling input in the CLI window
		} else {
            // Handle normal character drawing
            drawInputCharacters(c, 0x00FFFFFF, 1);  // Draw regular characters outside CLI popup
        }
    }
}