#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "../header/mbox.h"
#include "../header/framebf.h"
#include "../header/ascii_art.h"
#include "../header/cli.h"

void main()
{
    // set up serial console
	uart_init();

	// Show ASCII art banner
    print_ascii_art();

	// Initialize frame buffer
	framebf_init();

	// run CLI
	cli_start();

	// Draw something on the screen
	drawRectARGB32(100,100,400,400,0x00AA0000,1); //RED
	drawRectARGB32(150,150,400,400,0x0000BB00,1); //GREEN
	drawRectARGB32(200,200,400,400,0x000000CC,1); //BLUE
	drawRectARGB32(250,250,400,400,0x00FFFF00,1); //YELLOW
	drawPixelARGB32(300, 300, 0x00FF0000); //RED


	// echo everything back
	while(1) {
		//read each char
		char c = uart_getc();

		//send back 
		uart_sendc(c);
	}
}