// -----------------------------------mbox.c -------------------------------------
#include "../include/mbox.h"
#include "../include/mbox.h"
#include "../uart/uart1.h"
#include "../uart/uart0.h"

/* Mailbox Data Buffer (each element is 32-bit)*/
/*
* The keyword attribute allows you to specify special attributes
*
* The aligned(N) attribute aligns the current data item on an address
* which is a multiple of N, by inserting padding bytes before the data item
*
* __attribute__((aligned(16)) : allocate the variable on a 16-byte boundary.
* *
* We must ensure that our our buffer is located at a 16 byte aligned address,
* so only the high 28 bits contain the address
* (last 4 bits is ZERO due to 16 byte alignment)
*
*/
volatile unsigned int __attribute__((aligned(16))) mBuf[36];

/**
* Read from the mailbox
*/
uint32_t mailbox_read(unsigned char channel)
{
    //Receiving message is buffer_addr & channel number
    uint32_t res;
    // Make sure that the message is from the right channel
    do {
        // Make sure there is mail to receive
        do {
            asm volatile("nop");
        } while (MBOX0_STATUS & MBOX_EMPTY);
        // Get the message
        res = MBOX0_READ;
    } while ( (res & 0xF) != channel);

    return res;
}

/**
* Write to the mailbox
*/
void mailbox_send(uint32_t msg, unsigned char channel)
{
    //Sending message is buffer_addr & channel number
    // Make sure you can send mail
    do {
        asm volatile("nop");
    } while (MBOX1_STATUS & MBOX_FULL);
    // send the message
    MBOX1_WRITE = msg;
}

/**
* Make a mailbox call. Returns 0 on failure, non-zero on success
*/
int mbox_call(unsigned int buffer_addr, unsigned char channel)
{
    //Check Buffer Address
    uart_puts("Buffer Address: ");
    uart_hex(buffer_addr);
    uart_sendc('\n');

    //Prepare Data (address of Message Buffer)
    unsigned int msg = (buffer_addr & ~0xF) | (channel & 0xF);
    mailbox_send(msg, channel);

    /* now wait for the response */
    /* is it a response to our message (same address)? */
    if (msg == mailbox_read(channel)) {
        /* is it a valid successful response (Response Code) ? */
        if (mBuf[1] == MBOX_RESPONSE)
            uart_puts("Got successful response \n");

        return (mBuf[1] == MBOX_RESPONSE);
    }

    return 0;
}

/**
* 
*/
uint32_t get_board_revision() {
    // Total size of the buffer in bytes (8 words × 4 bytes/word)
    mBuf[0] = 8 * 4;

    // Mailbox request code (0x00000000 indicates a request)
    mBuf[1] = MBOX_REQUEST;

    // Tag identifier for "Get Board Revision" (from mailbox property tags)
    mBuf[2] = 0x00010002;

    // The number of bytes available for the response value (4 bytes for revision)
    mBuf[3] = 4;

    // Request code for this tag (set to 0 for request)
    mBuf[4] = 0;

    // Placeholder for the result: the VideoCore fills in the board revision here
    mBuf[5] = 0;

    // Marks the end of the message buffer (as per protocol)
    mBuf[6] = 0;

    // Call the mailbox (channel 8 is for property tags)
    if (mbox_call((uint32_t)((uint64_t)mBuf), 8)) {
        // If successful, return the board revision (filled by GPU in mBuf[5])
        return mBuf[5];
    }

    // Return a known invalid value to indicate failure
    return 0xFFFFFFFF;
}

/**
* 
*/
int get_mac_address(uint8_t mac[6]) {
    // Total buffer size: 8 words = 32 bytes
    mBuf[0] = 8 * 4;

    // Indicate this is a request (as opposed to a response)
    mBuf[1] = MBOX_REQUEST;

    // Tag identifier for "Get MAC address"
    mBuf[2] = 0x00010003;

    // Value buffer size: 6 bytes (MAC address is 6 bytes)
    mBuf[3] = 6;

    // Request code (0 for request)
    mBuf[4] = 0;

    // Placeholder for the MAC address - MSB (first 4 bytes)
    mBuf[5] = 0;

    // Placeholder for the MAC address - LSB (last 2 bytes)
    mBuf[6] = 0;

    // End tag (required by the protocol)
    mBuf[7] = 0;

    // Make the mailbox call to channel 8
    if (mbox_call((uint32_t)((uint64_t)mBuf), 8)) {
        // GPU response splits the MAC across two 32-bit words:
        uint32_t mac_hi = mBuf[5];  // Bytes 0–3
        uint32_t mac_lo = mBuf[6];  // Bytes 4–5 (in lower half)

        // Extract and convert into 6 separate 8-bit values
        mac[0] = (mac_hi >> 24) & 0xFF;
        mac[1] = (mac_hi >> 16) & 0xFF;
        mac[2] = (mac_hi >> 8)  & 0xFF;
        mac[3] =  mac_hi        & 0xFF;
        mac[4] = (mac_lo >> 8)  & 0xFF;
        mac[5] =  mac_lo        & 0xFF;

        return 1; // Successfully retrieved MAC
    }

    return 0; // Failed to retrieve MAC
}