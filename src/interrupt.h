#ifdef RPI_VERSION // RPI3 Interrupt registers p. 112
#define IRQ_BASIC_PENDING (*(volatile unsigned int *)(MMIO_BASE + 0x0000B200))
#define IRQ_PENDING_1 (*(volatile unsigned int *)(MMIO_BASE + 0x0000B204))
#define IRQ_PENDING_2 (*(volatile unsigned int *)(MMIO_BASE + 0x0000B208))
#define IRQ_ENABLE_1 (*(volatile unsigned int *)(MMIO_BASE + 0x0000B210))
#define IRQ_ENABLE_2 (*(volatile unsigned int *)(MMIO_BASE + 0x0000B214))
#define IRQ_ENABLE_BASIC (*(volatile unsigned int *)(MMIO_BASE + 0x0000B218))
#define IRQ_DISABLE_1 (*(volatile unsigned int *)(MMIO_BASE + 0x0000B21C))
#define IRQ_DISABLE_2 (*(volatile unsigned int *)(MMIO_BASE + 0x0000B220))
#define IRQ_DISABLE_BASIC (*(volatile unsigned int *)(MMIO_BASE + 0x0000B224))
#else // RPI4 Interrupt registers p. 101
#define IRQ0_PENDING0 (*(volatile unsigned int *)(MMIO_BASE + 0x0000B200))
#define IRQ0_PENDING1 (*(volatile unsigned int *)(MMIO_BASE + 0x0000B204))
#define IRQ0_PENDING2 (*(volatile unsigned int *)(MMIO_BASE + 0x0000B208))
#define IRQ0_SET_EN_0 (*(volatile unsigned int *)(MMIO_BASE + 0x0000B210))
#define IRQ0_SET_EN_1 (*(volatile unsigned int *)(MMIO_BASE + 0x0000B214))
#define IRQ0_SET_EN_2 (*(volatile unsigned int *)(MMIO_BASE + 0x0000B218))
#define IRQ0_CLR_EN_0 (*(volatile unsigned int *)(MMIO_BASE + 0x0000B220))
#define IRQ0_CLR_EN_1 (*(volatile unsigned int *)(MMIO_BASE + 0x0000B224))
#define IRQ0_CLR_EN_2 (*(volatile unsigned int *)(MMIO_BASE + 0x0000B228))
#endif

#define AUX_IRQ (1 << 29) // RPI3 p.113 or RPI4 p. 88

void enable_interrupt_controller();
void handle_irq();
