#ifndef DMA_H
#define DMA_H

#include "gpio.h"

// Macro to access DMA registers for a specific channel
#define REGS_DMA(channel) ((volatile dma_channel_regs *)(unsigned long)(MMIO_BASE + 0x00007000 + (channel * 0x100)))

// Macros for DMA interrupt status and enable register addresses
#define REGS_DMA_INT_STATUS *((volatile unsigned int *)(MMIO_BASE + 0x00007FE0))
#define REGS_DMA_ENABLE *((volatile unsigned int *)(MMIO_BASE + 0x00007FF0))

//defines for differnet bits of the control and transfer info
// --- DMA Control and Status (CS) Register Bit Definitions ---
#define CS_RESET			(1 << 31)
#define CS_ABORT			(1 << 30)
#define CS_WAIT_FOR_OUTSTANDING_WRITES	(1 << 28)
#define CS_PANIC_PRIORITY_SHIFT		20
    #define DEFAULT_PANIC_PRIORITY		15
#define CS_PRIORITY_SHIFT		16
    #define DEFAULT_PRIORITY		1
#define CS_ERROR			(1 << 8)
#define CS_INT				(1 << 2)
#define CS_END				(1 << 1)
#define CS_ACTIVE			(1 << 0)

// --- DMA Transfer Information (TI) Register Bit Definitions ---
#define TI_PERMAP_SHIFT			16
#define TI_BURST_LENGTH_SHIFT		12
#define DEFAULT_BURST_LENGTH		0
#define TI_SRC_IGNORE			(1 << 11)
#define TI_SRC_DREQ			(1 << 10)
#define TI_SRC_WIDTH			(1 << 9)
#define TI_SRC_INC			(1 << 8)
#define TI_DEST_DREQ			(1 << 6)
#define TI_DEST_WIDTH			(1 << 5)
#define TI_DEST_INC			(1 << 4)
#define TI_WAIT_RESP			(1 << 3)
#define TI_TDMODE			(1 << 1)
#define TI_INTEN			(1 << 0)

// -----------------------------------
// DMA Control Block
// ----------------------------------
typedef struct {
    unsigned int transfer_info;       // Transfer Information (TI): controls direction, burst size, increment modes, etc.
    unsigned int src_addr;            // Source address of the data to transfer
    unsigned int dest_addr;           // Destination address where data should be copied
    unsigned int transfer_length;     // Number of bytes to transfer
    unsigned int mode_2d_stride;      // Used for 2D mode (stride/pitch); 0 for linear mode
    unsigned int next_block_addr;     // Address of next control block (for chained DMA transfers)
    unsigned int res[2];              // Reserved / padding (ensures 32-byte alignment)
} dma_control_block;

// -----------------------------------
// DMA Channel Register Map
// -----------------------------------
typedef struct {
    unsigned int control;             // Control and Status (CS) register
    unsigned int control_block_addr; // Physical address of the current control block
    dma_control_block block;         // Software-side DMA control block (for convenience/mirroring)
} dma_channel_regs;

// -----------------------------------
// High-Level DMA Channel Abstraction
// -----------------------------------
typedef struct {
    unsigned int channel;            // Channel number (0–14)
    dma_control_block *block;       // Pointer to the control block used by this channel
    int status;                      // Transfer status: 1 = success, 0 = error/failure
} dma_channel;

// -----------------------------------
// DMA Channel Type Enum
// -----------------------------------
typedef enum {
    CT_NONE = -1,                    // No available channel
    CT_NORMAL = 0x81                 // Normal DMA channel type (custom identifier)
} dma_channel_type;

// -----------------------------------
// DMA Function Prototypes
// -----------------------------------
dma_channel *dma_open_channel(unsigned int channel);
void dma_close_channel(dma_channel *channel);
void dma_setup_mem_copy(dma_channel *channel, void *dest, const void *src, unsigned int length, unsigned int burst_length);
void dma_start(dma_channel *channel);
int dma_wait(dma_channel *channel);
void do_dma(void *dest,const void *src, unsigned int total);
// void dma_init();
// void test_dma();
// void compare_CPU_DMA();
void dma_setup_2d_copy(dma_channel *channel, void *dest, const void *src,
                       unsigned int width, unsigned int height,
                       unsigned int dest_stride, unsigned int src_stride,
                       unsigned int burst_length);

#endif