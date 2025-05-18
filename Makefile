CFILES = $(wildcard ./src/*.c ./src/*/*.c ./src/*/*/*.c)
OFILES = $(CFILES:./src/%.c=./build/%.o)

GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib

all: clean kernel8.img run0
uart0: clean kernel8.img run0

./build/boot.o: ./boot/boot.S
	aarch64-none-elf-gcc $(GCCFLAGS) -c ./boot/boot.S -o ./build/boot.o

./build/%.o: ./src/%.c
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

kernel8.img: ./build/boot.o $(OFILES)
	aarch64-none-elf-ld -nostdlib ./build/boot.o $(OFILES) -T ./linker/link.ld -o ./build/kernel8.elf
	aarch64-none-elf-objcopy -O binary ./build/kernel8.elf kernel8.img

clean:
	@echo [INFO] Start cleaning...
	@powershell -NoProfile -ExecutionPolicy Bypass -File clean.ps1
	@echo [INFO] Finish cleaning!

run0: 
	@echo [INFO] Run emulation with QEMU...
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial stdio
