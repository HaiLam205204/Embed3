#include "../include/uart0.h"
#include "../include/uart1.h"
#include "../include/mbox.h"
#include "../include/framebf.h"
#include "../include/utils.h"
#include "../include/CLI.h"
#include "../include/welcomeScreen.h"
#include "../include/video.h"
#include "../include/renderFrame.h"
#include "../include/game.h"


#define OPEN_CLI 0x14 // CTRL T
#define VIDEO 0x16 // CTRL V
#define GAME 0x01 // CTRL A

void main()
{
    int start_x = 0;
    int start_y = 0;
    // set up serial console
	uart_init();

	//Print welcome message
	uart_puts((char*)welcome_message);

	// Initialize frame buffer
	framebf_init();

    // Draw background image
    //drawImage(start_x, start_y, welcome_image, WELCOME_WIDTH, WELCOME_HEIGHT);

    while(1) {
        // Read character from UART
        char c = uart_getc();
        uart_sendc(c);  // Echo character back

        if (c == OPEN_CLI) {
            // Trigger the CLI popup window
            draw_cli_window();
            cli_loop();  // Start handling input in the CLI window
		} else if (c == VIDEO) {
            video_playback(video_allArray, video_allArray_LEN, start_x, start_y, VIDEO_WIDTH, VIDEO_HEIGHT, DESTINATION_WIDTH, DESTINATION_HEIGHT);
        } else if(c == GAME) {
            game_loop();
        } else {
            // Handle normal character drawing
            drawInputCharacters(c, 0x00FFFFFF, 1);  // Draw regular characters outside CLI popup
        }
    }
}