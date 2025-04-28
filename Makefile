# Directories
SRC_DIR = ./src
BUILD_DIR = ./obj

# Source and object files
CFILES = $(wildcard $(SRC_DIR)/*.c)
OFILES = $(CFILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Raspberry Pi version
RPI_VER = -DRPI_VERSION=3

# Compiler flags
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib $(RPI_VER)

all: clean kernel8.img run

RPI3:
	make RPI_VER=-DRPI_VERSION=3 all

RPI4:
	make RPI_VER= all

$(BUILD_DIR)/boot.o: $(SRC_DIR)/boot.S
	aarch64-none-elf-gcc $(GCCFLAGS) -c $(SRC_DIR)/boot.S -o $(BUILD_DIR)/boot.o

$(BUILD_DIR)/uart.o: $(SRC_DIR)/uart.c
	aarch64-none-elf-gcc $(GCCFLAGS) -c $(SRC_DIR)/uart.c -o $(BUILD_DIR)/uart.o

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

kernel8.img: $(BUILD_DIR)/boot.o $(OFILES)
	aarch64-none-elf-ld -nostdlib $(BUILD_DIR)/boot.o $(OFILES) -T $(SRC_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
	aarch64-none-elf-objcopy -O binary $(BUILD_DIR)/kernel8.elf kernel8.img

clean:
	del kernel8.elf .\obj\*.o *.img

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio
