#include "../header/uart0.h"

int DATA_BITS = 8;	 // default: 8 data bits
int STOP_BITS = 2;	 // default: 2 stop bits
int PARITY = 0;		 // default: parity - none
int HANDSHAKING = 0; // default: no handshaking

/**
 * Set baud rate and characteristics and map to GPIO
 */
void uart_init(int ibrd, int fbrd)
{
	unsigned int r;

	/* Turn off UART0 */
	UART0_CR = 0x0;

	// Set GPIO14 and GPIO15 to be pl011 TX/RX which is ALT0    /
	r = GPFSEL1;
	r &= ~((7 << 12) | (7 << 15));		// clear bits 17-12 (FSEL15, FSEL14)
	r |= (0b100 << 12) | (0b100 << 15); // Set value 0b100 (select ALT0: TXD0/RXD0)
	GPFSEL1 = r;

#ifdef RPI3
	// enable GPIO 14, 15 /

	GPPUD = 0; // No pull up/down control
	// Toogle clock to flush GPIO setup
	r = 150;
	while (r--)
	{
		asm volatile("nop");
	}								   // waiting 150 cycles
	GPPUDCLK0 = (1 << 14) | (1 << 15); // enable clock for GPIO 14, 15
	r = 150;
	while (r--)
	{
		asm volatile("nop");
	}			   // waiting 150 cycles
	GPPUDCLK0 = 0; // flush GPIO setup

#else // RPI4
	r = GPIO_PUP_PDN_CNTRL_REG0;
	r &= ~((0 << 28) | (0 << 30)); // No resistor is selected for GPIO 14, 15
	GPIO_PUP_PDN_CNTRL_REG0 = r;
#endif

	/* Mask all interrupts. */
	UART0_IMSC = 0;

	/* Clear pending interrupts. */
	UART0_ICR = 0x7FF;

	/* Set integer & fractional part of Baud rate
	Divider = UART_CLOCK/(16 * Baud)
	Default UART_CLOCK = 48MHz (old firmware it was 3MHz);
	Integer part register UART0_IBRD  = integer part of Divider
	Fraction part register UART0_FBRD = (Fractional part * 64) + 0.5 */

	UART0_IBRD = ibrd;
	UART0_FBRD = fbrd;

	/* Set up the Line Control Register */
	/* Enable FIFO */
	/* Set length to 8 bit */
	/* Defaults for other bit are No parity, 1 stop bit */

	UART0_LCRH = set_lcfh_val();

	/* Enable UART0, receive, and transmit */
	UART0_CR = 0x301; // enable Tx, Rx, FIFO

	if (HANDSHAKING)
	{
		UART0_CR |= UART0_CR_RTSEN | UART0_CR_CTSEN;
	}
	else
	{
		UART0_CR &= ~(UART0_CR_RTSEN | UART0_CR_CTSEN);
	}
}
/**
 * Send a character
 */
void uart_sendc(char c)
{

	/* Check Flags Register */
	/* And wait until transmitter is not full */
	do
	{
		asm volatile("nop");
	} while (UART0_FR & UART0_FR_TXFF);

	/* Write our data byte out to the data register */
	UART0_DR = c;

	if (c == '\b')
	{
		do
		{
			asm volatile("nop");
		} while (UART0_FR & UART0_FR_TXFF);

		UART0_DR = ' ';

		do
		{
			asm volatile("nop");
		} while (UART0_FR & UART0_FR_TXFF);

		UART0_DR = 0x08;
	}
}

void uart_send_string(const char *str)
{
	// Iterate over each character in the string
	for (int i = 0; str[i] != '\0'; i++)
	{
		// Send each character using uart_sendc
		uart_sendc(str[i]);
	}
}

/**
 * Receive a character
 */
char uart_getc()
{
	char c = 0;

	/* Check Flags Register */
	/* Wait until Receiver is not empty
	 * (at least one byte data in receive fifo)*/
	do
	{
		asm volatile("nop");
	} while (UART0_FR & UART0_FR_RXFE);

	/* read it and return */
	c = (unsigned char)(UART0_DR);

	/* convert carriage return to newline */
	return (c == '\r' ? '\n' : c);
}

/**
 * Display a string
 */
void uart_puts(char *s)
{
	while (*s)
	{
		/* convert newline to carriage return + newline */
		if (*s == '\n')
			uart_sendc('\r');
		uart_sendc(*s++);
	}
}

/**
 * Display a value in hexadecimal format
 */
void uart_hex(unsigned int num)
{
	uart_puts("0x");
	for (int pos = 28; pos >= 0; pos = pos - 4)
	{

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
	// A string to store the digit characters
	char str[33] = "";

	// Calculate the number of digits
	int len = 1;
	int temp = num;
	while (temp >= 10)
	{
		len++;
		temp = temp / 10;
	}

	// Store into the string and print out
	for (int i = 0; i < len; i++)
	{
		int digit = num % 10; // get last digit
		num = num / 10;		  // remove last digit from the number
		str[len - (i + 1)] = digit + '0';
	}
	str[len] = '\0';

	uart_puts(str);
}

void uart_hex_byte(uint8_t byte)
{
	// Print the high nibble
	uart_sendc(hex_to_ascii(byte >> 4));

	// Print the low nibble
	uart_sendc(hex_to_ascii(byte & 0x0F));
}

void set_baud_rate_command(char *arg)
{

	display_start("Baud Rate Setting");
	unsigned int baud_rate = atoi(arg); // Convert string to integer

	if (baud_rate <= 0)
	{
		// Invalid number of data bits
		uart_puts("\nInvalid number of data bits. Please input baud rates, higher than 0.");
		display_end();
		return;
	}

	// Calculate the baud rate divisor
	float BAUDDIVs = UART0_CLOCK_FREQ / (16.0f * baud_rate);

	int ibrd = (int)BAUDDIVs;
	int fbrd = (int)(((BAUDDIVs - ibrd) * 64) + 0.5);

	if (ibrd == UART0_IBRD && fbrd == UART0_FBRD)
	{
		// Same baud rates
		uart_puts("\nBaud rates remain the same.\n");
		display_end();
		return;
	}

	uart_puts("\nIBRD before setting: ");
	uart_dec(UART0_IBRD);
	uart_puts("\nFBRD before setting: ");
	uart_dec(UART0_FBRD);
	uart_puts("\n\n");

	uart_puts("\nIBRD after setting: ");
	uart_dec(ibrd);
	uart_puts("\nFBRD after setting: ");
	uart_dec(fbrd);
	uart_puts("\n\nBaud Rate has been set to ");
	uart_dec(baud_rate);
	uart_puts("\n\nBaud rate of the system has been changed. Please manually change the baud rate of your environment.");

	display_end();
	display_prompt();

	while (!(UART0_FR & UART0_FR_TXFE))
	{
	}

	UART0_CR = 0x0;

	// Set baud rate and characteristics
	UART0_IBRD = ibrd;
	UART0_FBRD = fbrd;

	uart_init(UART0_IBRD, UART0_FBRD);
}

void set_data_bits_command(char *arg)
{
	display_start("Data Bits Setting");

	// Get the number of data bits from the argument
	int data_bits = atoi(arg);

	if (data_bits == DATA_BITS)
	{
		// Same number of data bits
		uart_puts("\nData bits remain the same.\n");
		display_end();
		return;
	}

	// Update the Line Control Register (LCRH) accordingly
	switch (data_bits)
	{
	case 5:
		DATA_BITS = 5;
		break;
	case 6:
		DATA_BITS = 6;
		break;
	case 7:
		DATA_BITS = 7;
		break;
	case 8:
		DATA_BITS = 8;
		break;
	default:
		// Invalid number of data bits
		uart_puts("\nInvalid number of data bits.\n");
		display_end();
		return;
	}

	uart_puts("\nLDRH before setting data bits: ");
	uart_hex(UART0_LCRH);

	uart_puts("\nLDRH after setting data bits: ");
	uart_hex(set_lcfh_val());

	uart_puts("\n\nThe number of data bits has been set to ");
	uart_puts(arg);
	uart_puts("\n\nThe number of data bits has been changed. Please manually change the data bits of your environment.");

	display_end();
	display_prompt();

	while (!(UART0_FR & UART0_FR_TXFE))
	{
	}

	UART0_CR = 0x0;

	uart_init(UART0_IBRD, UART0_FBRD);
}

void set_stop_bits_command(char *arg)
{

	display_start("Stop Bits Setting");

	// Get the number of stop bits from the argument
	int stop_bits = atoi(arg);

	if (stop_bits == STOP_BITS)
	{
		// Same stop bits
		uart_puts("\nStop bits remain the same.\n");
		display_end();
		return;
	}

	// Update the Line Control Register (LCRH) accordingly
	switch (stop_bits)
	{
	case 1:
		STOP_BITS = 1;
		break;
	case 2:
		STOP_BITS = 2;
		break;
	default:
		// Invalid number of stop bits
		uart_puts("\nInvalid number of stop bits.\n");
		display_end();
		return;
	}

	uart_puts("\nLDRH before setting stop bits: ");
	uart_hex(UART0_LCRH);

	uart_puts("\nLDRH after setting stop bits: ");
	uart_hex(set_lcfh_val());
	uart_puts("\n\nStop bits have been set to ");
	uart_puts(arg);
	uart_puts("\n\nStop data bits have been changed. Please manually change the stop bits of your environment.");

	display_end();

	while (!(UART0_FR & UART0_FR_TXFE))
	{
	}

	UART0_CR = 0x0;

	uart_init(UART0_IBRD, UART0_FBRD);
}

void set_parity_command(char *arg)
{

	display_start("Parity Setting");

	int prev_parity = PARITY;

	if (compare_string(arg, "none") == 0)
	{
		PARITY = 0;
	}
	else if (compare_string(arg, "odd") == 0)
	{
		PARITY = 1;
	}
	else if (compare_string(arg, "even") == 0)
	{
		PARITY = 2;
	}
	else
	{
		uart_puts("\nInvalid parity type. Please use 'none', 'even', or 'odd'.\n");
		display_end();
		return;
	}

	if (prev_parity == PARITY)
	{
		// Same parity
		uart_puts("\nParity remain the same.\n");
		display_end();
		return;
	}

	uart_puts("\nLDRH before setting parity: ");
	uart_hex(UART0_LCRH);

	uart_puts("\nLDRH after setting parity: ");
	uart_hex(set_lcfh_val());
	uart_puts("\n\nParity has been set to ");
	uart_puts(arg);
	uart_puts("\n\nParity has been changed. Please manually change the Parity of your environment.");

	display_end();

	while (!(UART0_FR & UART0_FR_TXFE))
	{
	}

	UART0_CR = 0x0;

	uart_init(UART0_IBRD, UART0_FBRD);
}

void set_handshaking_command(char *arg)
{

	display_start("Handshaking Setting");

	unsigned int new_CR = UART0_CR;

	if (compare_string(arg, "on") == 0)
	{
		HANDSHAKING = 1;
		new_CR |= UART0_CR_RTSEN | UART0_CR_CTSEN;
	}
	else if (compare_string(arg, "off") == 0)
	{
		HANDSHAKING = 0;
		new_CR &= ~(UART0_CR_RTSEN | UART0_CR_CTSEN);
	}
	else
	{
		uart_puts("\nInvalid handshaking type. Please use 'on' or 'off'.\n");
		display_end();
		return;
	}

	if (new_CR == UART0_CR)
	{
		// Same handshaking
		uart_puts("\nHandshaking remains the same.\n");
		display_end();
		return;
	}

	uart_puts("\nCR before handshaking: ");
	uart_hex(UART0_CR);

	uart_puts("\nCR after handshaking: ");
	uart_hex(new_CR);
	uart_puts("\n\nHandshaking is ");
	uart_puts(arg);
	uart_puts("\n\nHandshaking has been changed. Please manually change the handshaking of your environment.");
	display_end();

	while (!(UART0_FR & UART0_FR_TXFE))
	{
	}

	UART0_CR = 0x0;

	uart_init(UART0_IBRD, UART0_FBRD);
}

int set_lcfh_val()
{

	unsigned int lcfh = 0;

	// Data bits
	if (DATA_BITS == 5)
	{
		lcfh |= UART0_LCRH_FEN | UART0_LCRH_WLEN_5BIT;
	}
	else if (DATA_BITS == 6)
	{
		lcfh |= UART0_LCRH_FEN | UART0_LCRH_WLEN_6BIT;
	}
	else if (DATA_BITS == 7)
	{
		lcfh |= UART0_LCRH_FEN | UART0_LCRH_WLEN_7BIT;
	}
	else if (DATA_BITS == 8)
	{
		lcfh |= UART0_LCRH_FEN | UART0_LCRH_WLEN_8BIT;
	}

	// Stop bits
	if (STOP_BITS == 1)
	{
		lcfh &= ~UART0_LCRH_STP2;
	}
	else if (STOP_BITS == 2)
	{
		lcfh &= ~UART0_LCRH_STP2;
		lcfh |= UART0_LCRH_STP2;
	}

	// Parity
	if (PARITY == 0) // None
	{
		lcfh &= ~(UART0_LCRH_PEN | UART0_LCRH_EPS);
	}
	else if (PARITY == 1) // Odd
	{
		lcfh |= UART0_LCRH_PEN;
		lcfh &= ~UART0_LCRH_EPS;
	}
	else if (PARITY == 2) // Even
	{
		lcfh &= ~(UART0_LCRH_PEN | UART0_LCRH_EPS);
		lcfh |= UART0_LCRH_PEN | UART0_LCRH_EPS;
	}

	return lcfh;
}