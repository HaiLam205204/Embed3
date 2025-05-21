void drawChar(unsigned char ch, int x, int y, unsigned int attr, int zoom);
void drawInputCharacters(char ch, unsigned int attr, int zoom);
void drawString(int x, int y, char *str, unsigned int attr, int zoom);
void drawChar_double_buffering(unsigned char ch, int x, int y, unsigned int attr, int zoom);
void drawString_double_buffering(int x, int y, char *str, unsigned int attr, int zoom);
unsigned int uart_isReadByteReady();
unsigned char getUart();
void wait_us(uint64_t usVal);
void set_wait_timer(int set, uint64_t usVal);
void* memcpy(void *dest, const void *src, unsigned long n);
void *memset(void *ptr, int value, unsigned int num);
extern const char* welcome_message;

// Seed the custom PRNG
void srand_custom(uint64_t seed);

// Generate a pseudo-random 64-bit number
uint64_t rand_custom(void);

// CHoose between 0 or 1
uint64_t rand_0_or_1();

// CHoose number between 0 to 3
uint64_t rand_0_to_3();

// Retrieve current system timer ticks (you provide implementation)
uint64_t get_arm_system_time(void);

// Busy wait for specified microseconds (you provide implementation)
void wait_us(uint64_t usVal);

// // Print a number (you provide implementation)
// void print_number(uint64_t n);

// // Run the sequence: print random number 3 times every 2 seconds
// void run_random_prints(void);
