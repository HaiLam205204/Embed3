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
// #include "../include/Orpheus_Skill_Option.h"
#include "../include/game_menu.h"

#include "../include/game_design.h"
#define DESIGN 0x18 //CTRL X
#define OPEN_CLI 0x14 // CTRL T
#define VIDEO 0x16 // CTRL V
#define GAME 0x01 // CTRL A
#define ESCAPE 0x1B // ESC

void main()
{
    // set up serial console
	uart_init();

	//Print welcome message
	uart_puts((char*)welcome_message);

	// Initialize frame buffer
	framebf_init();

    // draw_skill_option_screen(0, 1);
    // drawImage_double_buffering(
    //             ORPHEUS_SKILL_OPTION_SCRREN_X,
    //             ORPHEUS_SKILL_OPTION_SCRREN_Y,
    //             orpheus_skill_bitmap_allArray[0],
    //             ORPHEUS_SKILL_OPTION_WIDTH,
    //             ORPHEUS_SKILL_OPTION_HEIGHT
    //         );

    //Draw background image
    draw_background();

    while(1) {
        // Read character from UART
        char c = getUart();
        uart_sendc(c);  // Echo character back

        if (c == OPEN_CLI) {
            // Trigger the CLI popup window
            draw_cli_window();
            cli_loop();  // Start handling input in the CLI window
		} else if (c == VIDEO) {
            video_playback(video_allArray, video_allArray_LEN, 0, 0, VIDEO_WIDTH, VIDEO_HEIGHT, DESTINATION_WIDTH, DESTINATION_HEIGHT);
        } else if(c == GAME) {
            displayRewardScreen();
            //game_loop();
        } else if(c == DESIGN) {
            design_screen_loop();   
        } 
    }
}