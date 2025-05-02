#include "../../include/uart1.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
    unsigned int r;

    /* initialize UART */
    AUX_ENABLE |= 1;     //enable mini UART (UART1) 
    AUX_MU_CNTL = 0;	 //stop transmitter and receiver
    AUX_MU_LCR  = 3;     //8-bit mode (also enable bit 1 to be used for RPI3)
    AUX_MU_MCR  = 0;	 //clear RTS (request to send)
    AUX_MU_IER  = 0;	 //disable interrupts
    AUX_MU_IIR  = 0xc6;  //enable and clear FIFOs

    /* Note: refer to page 11 of ARM Peripherals guide for baudrate configuration 
    (system_clk_freq is 250MHz by default) */

    /* map UART1 to GPIO pins 14 and 15 */
    r = GPFSEL1;
    r &=  ~( (7 << 12)|(7 << 15) ); //clear bits 17-12 (FSEL15, FSEL14)
    r |= (0b010 << 12)|(0b010 << 15);   //set value 0b010 (select ALT5: TXD1/RXD1)
    GPFSEL1 = r;

	/* enable GPIO 14, 15 */
#ifdef RPI3 //RPI3
	GPPUD = 0;            //No pull up/down control
	//Toogle clock to flush GPIO setup
	r = 150; while(r--) { asm volatile("nop"); } //waiting 150 cycles
	GPPUDCLK0 = (1 << 14)|(1 << 15); //enable clock for GPIO 14, 15
	r = 150; while(r--) { asm volatile("nop"); } //waiting 150 cycles
	GPPUDCLK0 = 0;        // flush GPIO setup

#else //RPI4
	r = GPIO_PUP_PDN_CNTRL_REG0;
	r &= ~((3 << 28) | (3 << 30)); //No resistor is selected for GPIO 14, 15
	GPIO_PUP_PDN_CNTRL_REG0 = r;
#endif

    AUX_MU_CNTL = 3;      //enable transmitter and receiver (Tx, Rx)
}

/**
 * Send a character
 */
void uart_sendc(char c) {
    // wait until transmitter is empty
    do {
    	asm volatile("nop");
    } while ( !(AUX_MU_LSR & 0x20) );

    // write the character to the buffer 
    AUX_MU_IO = c;
}

/**
 * Receive a character
 */
char uart_getc() {
    char c;

    // wait until data is ready (one symbol)
    do {
    	asm volatile("nop");
    } while ( !(AUX_MU_LSR & 0x01) );

    // read it and return
    c = (unsigned char)(AUX_MU_IO);

    // convert carriage return to newline character
    return (c == '\r' ? '\n' : c);
}

/**
 * Display a string
 */
void uart_puts(char *s) {
    while (*s) {
        // convert newline to carriage return + newline
        if (*s == '\n')
            uart_sendc('\r');
        uart_sendc(*s++);
    }
}


/**
* Display a value in hexadecimal format
*/
void uart_hex(unsigned int num) {
	uart_puts("0x");
	for (int pos = 28; pos >= 0; pos = pos - 4) {

		// Get highest 4-bit nibble
		char digit = (num >> pos) & 0xF;

		/* Convert to ASCII code */
		// 0-9 => '0'-'9', 10-15 => 'A'-'F'
		digit += (digit > 9) ? (-10 + 'A') : '0';
		uart_sendc(digit);
	}
}

/*
**
* Display a value in decimal format
*/
void uart_dec(int num)
{
	//A string to store the digit characters
	char str[33] = "";

	//Calculate the number of digits
	int len = 1;
	int temp = num;
	while (temp >= 10){
		len++;
		temp = temp / 10;
	}

	//Store into the string and print out
	for (int i = 0; i < len; i++){
		int digit = num % 10; //get last digit
		num = num / 10; //remove last digit from the number
		str[len - (i + 1)] = digit + '0';
	}
	str[len] = '\0';

	uart_puts(str);
}

/**
* Comparing 2 strings.
*/
int string_compare(char *a, char *b)
{
    int flag = 0;
    while (*a != '\0' && *b != '\0') // while loop
    {
        for (int i = 0; i < sizeof(a); i++)
        {
            if (*a != *b)
            {
                flag = 1;
            }
            a++;
            b++;
        }
    }
    if (flag == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
* Substring.
*/
void substring(char s[], char sub[], int p, int l)
{
    int c = 0;

    while (c < l)
    {
        sub[c] = s[p + c - 1];
        c++;
    }
    sub[c] = '\0';
}

/**
 * String copy - copies the source string to destination
 * param dest: Destination buffer (must have enough space)
 * param src: Source string to copy
 * return: Pointer to destination string
 */
char* strcpy(char *dest, char *src) {
    char *original_dest = dest;  // Save original destination for return
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';  // Null-terminate the destination
    return original_dest;
}

/**
* Convert string char to int.
*/
int convert(char s[])
{
    int i, n = 0;

    for (i = 0; s[i] >= '0' && s[i] <= '9'; i++)
        n = 10 * n + (s[i] - '0');

    return n;
}
 
/**
* Function to set baudrate.
*/
void set_baudrate(int baudrate) {
    unsigned int baud_reg_value = (250000000 / (baudrate * 8)) - 1;  // Assuming a clock of 250MHz
    AUX_MU_BAUD = baud_reg_value;
    uart_puts("Baud rate set to ");
    uart_dec(baudrate);
    uart_puts(" bps\n");
}