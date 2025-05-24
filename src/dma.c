#include "../include/dma.h"
#include "../include/uart0.h"
#include "../include/timer.h"

// TEST DATA ARRAY SIZE
#define SIZE_LARGE_DATA 20000000

// Pointer to the currently used DMA channel
static dma_channel *dma;

// Static array of DMA channels to manage multiple channels
dma_channel channels[15];

// Bitmask for tracking available DMA channels (1 = available)
static unsigned int channel_map = 0x1F35;

// Let the array as global to ultilize global memory region to avoid memory violation
unsigned int src_data[SIZE_LARGE_DATA];
unsigned int dest_data_CPU[SIZE_LARGE_DATA];
unsigned int dest_data_DMA[SIZE_LARGE_DATA];

void dma_setup_2d_copy(dma_channel *channel, void *dest, const void *src,
                       unsigned int width, unsigned int height,
                       unsigned int dest_stride, unsigned int src_stride,
                       unsigned int burst_length) {
    channel->block->transfer_info = (burst_length << TI_BURST_LENGTH_SHIFT)
                                   | TI_SRC_WIDTH
                                   | TI_DEST_WIDTH
                                   | TI_SRC_INC
                                   | TI_DEST_INC
                                   | TI_TDMODE;  // Enable 2D transfer

    channel->block->src_addr = (unsigned long)src;
    channel->block->dest_addr = (unsigned long)dest;
    channel->block->transfer_length = width;
    channel->block->mode_2d_stride = (src_stride << 16) | dest_stride;
    channel->block->next_block_addr = 0;
}

/**
 * Allocate a DMA channel.
 * channel: Specific channel requested or CT_NORMAL/CT_PERIPHERAL for automatic selection.
 * Allocated channel index, or CT_NONE (-1) if none are available.
 */
static unsigned int allocate_channel(unsigned int channel) {
    // If a specific channel (0–15) is requested
    if (!(channel & ~0x0F)) {
        // Check if it is available in channel_map
        if (channel_map & (1 << channel)) {
            // Mark it as used
            channel_map &= ~(1 << channel);
            uart_puts("[DMA ALLOCATION] Channel allocated: ");
            uart_dec(channel);
            uart_puts("\n");
            return channel;
        }

        uart_puts("[DMA ALLOCATION] Requested channel unavailable.\n");
        return -1; // Channel unavailable
    }

    // For automatic selection: prefer different ranges depending on context
    unsigned int i = channel == CT_NORMAL ? 6 : 12;

    // Try to allocate from preferred range
    for (; i >= 0; i--) {
        // Check if it is available in channel_map
        if (channel_map & (1 << i)) {
            // Mark it as used
            channel_map &= ~(1 << i);
            uart_puts("[DMA ALLOCATION] Auto-allocated channel: ");
            uart_dec(i);
            uart_puts("\n");
            return i;
        }
    }
    return CT_NONE; // No channels available
}

/**
 * Opens and configures a DMA channel.
 * channel: Desired channel number or allocation type.
 * Pointer to the initialized dma_channel structure.
 */
dma_channel *dma_open_channel(unsigned int channel) {
    uart_puts("[DMA OPEN CHANNEL] dma_open_channel called with channel=");
    uart_dec(channel);
    uart_puts("\n");

    unsigned int _channel = allocate_channel(channel);

    // If the allocated channel is unavailable
    if (_channel == CT_NONE) {
        uart_puts("[DMA OPEN CHANNEL] INVALID CHANNEL!\n");
        return 0;
    }

    // Get channel structure
    dma_channel *dma = (dma_channel *)&channels[_channel]; // define a dma_channel object
    dma->channel = _channel;

    // Allocate and align DMA control block (32-byte aligned)
    static dma_control_block __attribute__((aligned(32))) dma_block;
    dma->block = &dma_block;

    // Clear reserved control block fields
    dma->block->res[0] = 0;
    dma->block->res[1] = 0;

    // Enable this DMA channel in the global enable register
    REGS_DMA_ENABLE |= (1 << dma->channel);

    // Reset the DMA controller (clear error and state)
    REGS_DMA(dma->channel)->control |= CS_RESET;
    
    // Wait for reset to complete
    while(REGS_DMA(dma->channel)->control & CS_RESET) ;

    uart_puts("[DMA OPEN CHANNEL] DMA channel opened successfully. Channel = ");
    uart_dec(dma->channel);
    uart_puts("\n");

    return dma;
}

/**
 * Releases a DMA channel for reuse.
 */
void dma_close_channel(dma_channel *channel) {

    uart_puts("[DMA CLOSE CHANNEL] dma_close_channel called for channel ");
    uart_dec(channel->channel);
    uart_puts("\n");

    channel_map |= (1 << channel->channel); // Mark channel as available
}



/**
 * Configures a memory-to-memory DMA transfer.
 * channel: DMA channel to configure
 * dest: Destination address
 * src: Source address
 * length: Number of bytes to copy
 * burst_length: Number of words per burst (performance tuning)
 */
void dma_setup_mem_copy(dma_channel *channel, void *dest, const void *src, unsigned int length, unsigned int burst_length) {

    /* Current Transfer Info (TI) setting: 128-bit source read width, 
                                            Source address increments after each read. The address will increment by 4, if SRC_WIDTH=0 else by 32
                                            Use 128-bit destination write width
                                            Destination address increments after each write. The address will increment by 4, if DEST_WIDTH=0 else by 32
    */
    channel->block->transfer_info = (burst_length << TI_BURST_LENGTH_SHIFT)
                            | TI_SRC_WIDTH       // 128-bit read
                            | TI_SRC_INC         // Increment source address
                            | TI_DEST_WIDTH      // 128-bit write
                            | TI_DEST_INC;       // Increment destination address

    channel->block->src_addr = (unsigned long)src;
    channel->block->dest_addr = (unsigned long)dest;
    channel->block->transfer_length = length;
    channel->block->mode_2d_stride = 0; // No 2D stride
    channel->block->next_block_addr = 0; // No chained blocks  
}

/**
 * Starts the DMA transfer using the configured control block.
 */
void dma_start(dma_channel *channel) {
    uart_puts("[DMA START] dma_start\n");

    /* Convert ARM address to bus address (set higher address bits)
     * DMA control blocks are located in RAM memory.
     * Software accessing RAM directly uses physical addresses
     * (based at 0x00000000). To ensure proper access by the DMA,
     * the ARM address must be converted to a bus address.
     * This is achieved by clearing the upper address bits and
     * setting the base address to 0xC0000000.
    */ 

    // Convert ARM physical address to DMA bus address
    REGS_DMA(channel->channel)->control_block_addr = (((unsigned long)(channel->block) & (0x3FFFFFFF)) | 0xC0000000);

    // Start DMA by writing to control register with priorities
    REGS_DMA(channel->channel)->control = CS_WAIT_FOR_OUTSTANDING_WRITES
					      | (DEFAULT_PANIC_PRIORITY << CS_PANIC_PRIORITY_SHIFT)
					      | (DEFAULT_PRIORITY << CS_PRIORITY_SHIFT)
					      | CS_ACTIVE;

    uart_puts("[DMA START] DMA transfer started on channel ");
    uart_dec(channel->channel);
    uart_puts("\n");
}

/**
 * Waits for DMA transfer to complete and returns status.
 * return 1 if successful, 0 if error occurred.
 */
int dma_wait(dma_channel *channel) {
    
    // Poll the CS_ACTIVE bit until it's cleared (DMA done)
    while(REGS_DMA(channel->channel)->control & CS_ACTIVE) ;

    // Check for errors in the control register
    channel->status = REGS_DMA(channel->channel)->control & CS_ERROR ? 0 : 1;

    return channel->status;

}

/**
 * Performs DMA copy with performance timing.
 */
// Call DMA to start memory transfer with brust_length = 16 word
void do_dma(void *dest,const void *src, unsigned int total) {
    unsigned int ms_start = timer_get_ticks() / 1000;

        
    dma_setup_mem_copy(dma, dest, src, total, 15);  // Set up transfer with burst length = 16
    dma_start(dma);                                 // Start DMA
    dma_wait(dma);                                  // Wait for completion

    unsigned int ms_end = timer_get_ticks() / 1000;
    uart_puts("The ms take to copy using DMA: ");
    uart_dec((ms_end - ms_start));
    uart_puts("\n");
}

// /**
//  * Initializes the DMA subsystem and performs a basic test.
//  */
// void dma_init(){
//     dma = dma_open_channel(5);

//     uart_puts("Opened DMA CHANNEL: ");
//     uart_puts("\n");

//     test_dma();
// }

// /**
//  * Test function to verify that DMA correctly copies memory.
//  */
// void test_dma() {
//     uart_puts("Testing DMA...\n");

//     const unsigned int src_data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//     unsigned int dest_data[10]; // Initialize destination array with zeros

//     // Perform DMA operation (assuming size is in bytes)
//     do_dma(dest_data, src_data, sizeof(src_data));
//     uart_puts("After do_dma\n");
//     // Verify the copy
//     if (compare_memory(src_data, dest_data, sizeof(src_data)/sizeof(unsigned int))) {
//         uart_puts("\nDMA Test PassedVVVV.\n");
//     } else {
//         uart_puts("\nDMA Test Failed.\n");
//     }
// }

// // Test DMA functionality
// int compare_memory(const unsigned int *a, const unsigned int *b, unsigned int size) {
//     // Compare each value of 2 array
//     for (unsigned int i = 0; i < size; i++) {
//         uart1_dec(a[i]);
//         uart1_puts("-");
//         uart1_dec(b[i]);
//         uart1_puts(" ");
//         if (a[i] != b[i]) {
            
//             return 0; // Memory differs
//         }
//     }
//     return 1; // Memory is the same
// }


// // Compare speed of copy large data set between CPU and DMA
// void compare_CPU_DMA(){
//     uart1_puts("Compare speed between CPU and DMA \n");


//     // To save time value
//     unsigned int ms_start;
//     unsigned int ms_end;

//     // Initialize source array
//     for(unsigned int i = 0; i < SIZE_LARGE_DATA; i++){
//         src_data[i] = i;
//     }
//     ms_start = timer_get_ticks() / 1000;

//     // Copy using CPU
//     for(unsigned int i = 0; i < SIZE_LARGE_DATA; i++){
//         dest_data_CPU[i] = src_data[i];
//     }

//     ms_end = timer_get_ticks() / 1000;
//     uart1_puts("The ms take to copy using CPU: ");
//     uart1_dec((ms_end - ms_start));
//     uart1_puts("\n");

//     // Copy using DMA
//     do_dma(dest_data_DMA, src_data, SIZE_LARGE_DATA * sizeof(unsigned int));
// } 





