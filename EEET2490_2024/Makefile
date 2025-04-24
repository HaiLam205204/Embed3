#--------------------------------------Makefile-------------------------------------

CFILES = $(wildcard ./kernel/*.c)
OFILES = $(CFILES:./kernel/%.c=./object/%.o)
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib

all: clean uart0_build ./object/kernel8.img run

./object/boot.o: ./kernel/boot.S
	aarch64-none-elf-gcc $(GCCFLAGS) -c ./kernel/boot.S -o ./object/boot.o

uart0_build: ./peripheral/uart0.c
	aarch64-none-elf-gcc $(GCCFLAGS) -c ./peripheral/uart0.c -o ./object/uart.o

./object/%.o: ./kernel/%.c
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

./object/kernel8.img: ./object/boot.o ./object/uart.o $(OFILES)
	aarch64-none-elf-ld -nostdlib ./object/boot.o ./object/uart.o $(OFILES) -T ./kernel/link.ld -o ./object/kernel8.elf
	aarch64-none-elf-objcopy -O binary ./object/kernel8.elf ./object/kernel8.img

clean:
	del .\object\kernel8.elf .\object\*.o .\object\*.img

# Run emulation with QEMU
run: 
	qemu-system-aarch64 -M raspi3 -kernel ./object/kernel8.img -serial stdio
