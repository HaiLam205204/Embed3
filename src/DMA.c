#include "../include/DMA.h"
#include "../include/uart0.h"
#include "../include/renderFrame.h"

#define SIZE_LARGE_DATA 20000000

static dma_channel *dma;

dma_channel channels[15];

static unsigned int channel_map = 0x1F35;

// Let the array as global to ultilize global memory region to avoid memory violation
unsigned int src_data[SIZE_LARGE_DATA];
unsigned int dest_data_CPU[SIZE_LARGE_DATA];
unsigned int dest_data_DMA[SIZE_LARGE_DATA];

static unsigned int allocate_channel(unsigned int channel) {
    if (!(channel & ~0x0F)) {
        if (channel_map & (1 << channel)) {
            channel_map &= ~(1 << channel);
            return channel;
        }

        return -1;
    }

    unsigned int i = channel == CT_NORMAL ? 6 : 12;

    for (; i >= 0; i--) {
        if (channel_map & (1 << i)) {
            channel_map &= ~(1 << i);
            return i;
        }
    }
    return CT_NONE;
}

dma_channel *dma_open_channel(unsigned int channel) {
    unsigned int _channel = allocate_channel(channel);

    if (_channel == CT_NONE) {
        uart_puts("INVALID CHANNEL!");
        return 0;
    }

    dma_channel *dma = (dma_channel *)&channels[_channel];
    dma->channel = _channel;

    static dma_control_block __attribute__((aligned(32))) dma_block;
    dma->block = &dma_block;
    dma->block->res[0] = 0;
    dma->block->res[1] = 0;

    REGS_DMA_ENABLE |= (1 << dma->channel);
    REGS_DMA(dma->channel)->control |= CS_RESET;
    
    while(REGS_DMA(dma->channel)->control & CS_RESET) ;
    uart_puts("DMA open successfully!!!");

    return dma;
}

void dma_close_channel(dma_channel *channel) {
    channel_map |= (1 << channel->channel);
}

void dma_setup_mem_copy(dma_channel *channel, void *dest, const void *src, unsigned int length, unsigned int burst_length) {
    /* Current Transfer Info (TI) setting: 128-bit source read width, 
                                            Source address increments after each read. The address will increment by 4, if SRC_WIDTH=0 else by 32
                                            Use 128-bit destination write width
                                            Destination address increments after each write. The address will increment by 4, if DEST_WIDTH=0 else by 32
    */
    channel->block->transfer_info = (burst_length << TI_BURST_LENGTH_SHIFT)
						    | TI_SRC_WIDTH
						    | TI_SRC_INC
						    | TI_DEST_WIDTH
						    | TI_DEST_INC;

    channel->block->src_addr = (unsigned long)src;
    channel->block->dest_addr = (unsigned long)dest;
    channel->block->transfer_length = length;
    channel->block->mode_2d_stride = 0;
    channel->block->next_block_addr = 0;   
}

void dma_start(dma_channel *channel) {
    /* Convert ARM address to bus address (set higher address bits)
     * DMA control blocks are located in RAM memory.
     * Software accessing RAM directly uses physical addresses
     * (based at 0x00000000). To ensure proper access by the DMA,
     * the ARM address must be converted to a bus address.
     * This is achieved by clearing the upper address bits and
     * setting the base address to 0xC0000000.
    */ 
    REGS_DMA(channel->channel)->control_block_addr = (((unsigned long)(channel->block) & (0x3FFFFFFF)) | 0xC0000000);


    REGS_DMA(channel->channel)->control = CS_WAIT_FOR_OUTSTANDING_WRITES
					      | (DEFAULT_PANIC_PRIORITY << CS_PANIC_PRIORITY_SHIFT)
					      | (DEFAULT_PRIORITY << CS_PRIORITY_SHIFT)
					      | CS_ACTIVE;
}

// Wait until DMA transfer is complete
int dma_wait(dma_channel *channel) {
    while(REGS_DMA(channel->channel)->control & CS_ACTIVE) ;

    channel->status = REGS_DMA(channel->channel)->control & CS_ERROR ? 0 : 1;

    return channel->status;

}

void dma_init(){
    dma = dma_open_channel(5);

    uart_puts("DMA CHANNEL: ");
    uart_dec(dma->channel);
    uart_puts("\n");

    test_dma();
}

// Call DMA to start memory transfer with brust_length = 16 word
void do_dma(void *dest,const void *src, unsigned int total) {
    unsigned int ms_start = get_arm_system_time();
    unsigned int us_start = ticks_to_us(ms_start);

        dma_setup_mem_copy(dma, dest, src, total, 15);
        
        dma_start(dma);

        dma_wait(dma);

    unsigned int ms_end = get_arm_system_time();
    unsigned int us_end = ticks_to_us(ms_end);
    uart_puts("The ms take to copy using DMA: ");
    uart_dec((us_end - us_start));
    uart_puts("\n");
}

// Test DMA functionality
int compare_memory(const unsigned int *a, const unsigned int *b, unsigned int size) {
    // Compare each value of 2 array
    for (unsigned int i = 0; i < size; i++) {
        uart_dec(a[i]);
        uart_puts("-");
        uart_dec(b[i]);
        uart_puts(" ");
        if (a[i] != b[i]) {
            
            return 0; // Memory differs
        }
    }
    return 1; // Memory is the same
}

// Test DMA functionality
void test_dma() {
    uart_puts("Testing DMA...\n");

    const unsigned int src_data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    unsigned int dest_data[10]; // Initialize destination array with zeros

    // Perform DMA operation (assuming size is in bytes)
    do_dma(dest_data, src_data, sizeof(src_data));
    uart_puts("After do_dma\n");
    // Verify the copy
    if (compare_memory(src_data, dest_data, sizeof(src_data)/sizeof(unsigned int))) {
        uart_puts("\nDMA Test PassedVVVV.\n");
    } else {
        uart_puts("\nDMA Test Failed.\n");
    }
}

// Compare speed of copy large data set between CPU and DMA
void compare_CPU_DMA(){
    uart_puts("Compare speed between CPU and DMA \n");


    // To save time value
    unsigned int ticks_start;
    unsigned int ticks_end;
    unsigned int us_start;
    unsigned int us_end;

    // Initialize source array
    for(unsigned int i = 0; i < SIZE_LARGE_DATA; i++){
        src_data[i] = i;
    }
    ticks_start = get_arm_system_time();
    us_start = ticks_to_us(ticks_end);

    // Copy using CPU
    for(unsigned int i = 0; i < SIZE_LARGE_DATA; i++){
        dest_data_CPU[i] = src_data[i];
    }

    ticks_end = get_arm_system_time();
    us_end = ticks_to_us(ticks_end);
    uart_puts("The ms take to copy using CPU: ");
    uart_dec((us_end - us_start));
    uart_puts("\n");

    // Copy using DMA
    do_dma(dest_data_DMA, src_data, SIZE_LARGE_DATA * sizeof(unsigned int));
} 





