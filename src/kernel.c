#include "../include/uart0.h"
#include "../include/mbox.h"
#include "../include/framebf.h"
#include "../include/utils.h"
#include "../include/CLI.h"
#include "../include/bitmaps/welcomeScreen.h"
#include "../include/bitmaps/video.h"
#include "../include/renderFrame.h"
#include "../include/game.h"
#include "../include/game_combat.h"
#include "../include/game_design.h"
#include "../include/game_menu.h"

#include "../include/game_design.h"
#define OPEN_CLI 0x14 // CTRL T

void main()
{
    // set up serial console
	uart_init();

	//Print welcome message
	uart_puts((char*)welcome_message);

	// Initialize frame buffer
	framebf_init();

    //Draw background image
    draw_background();

    while(1) {
        // Read character from UART
        char c = getUart();

        if (c == OPEN_CLI) {
            // Trigger the CLI popup window
            draw_cli_window();
            cli_loop();  // Start handling input in the CLI window
        }
    }
}