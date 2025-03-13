SRC_DIR := ./base/txos
BUILD_DIR := ./build
BIN_DIR := ./bin

# Find all .c and .asm files, excluding the bootloader's .asm file
C_SOURCES := $(shell find $(SRC_DIR) -type f -name "*.c")
ASM_SOURCES := $(shell find $(SRC_DIR) -type f -name "*.asm" ! -name "x86fboot.asm")

# Convert sources to object file paths
C_OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
ASM_OBJS := $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%.asm.o, $(ASM_SOURCES))

# Include paths and compiler flags
INCLUDES := -I./src
FLAGS := -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops \
         -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function \
         -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter \
         -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: $(BIN_DIR)/boot.bin $(BIN_DIR)/kernel.bin
	rm -rf $(BIN_DIR)/os.bin
	dd if=$(BIN_DIR)/boot.bin >> $(BIN_DIR)/os.bin
	dd if=$(BIN_DIR)/kernel.bin >> $(BIN_DIR)/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> $(BIN_DIR)/os.bin
	sudo mkdir -p /mnt/d
	sudo mount -t vfat $(BIN_DIR)/os.bin /mnt/d
	sudo cp ./hello.txt /mnt/d
	sudo umount /mnt/d

$(BIN_DIR)/kernel.bin: $(C_OBJS) $(ASM_OBJS)
	mkdir -p $(BIN_DIR)
	i686-elf-ld -g -relocatable $(C_OBJS) $(ASM_OBJS) -o $(BUILD_DIR)/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./base/txos/init/linker.ld -o $(BIN_DIR)/kernel.bin -ffreestanding -O0 -nostdlib $(BUILD_DIR)/kernelfull.o

$(BIN_DIR)/boot.bin: $(SRC_DIR)/boot/fat/x86fboot.asm
	mkdir -p $(BIN_DIR)
	nasm -f bin $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c $< -o $@

$(BUILD_DIR)/%.asm.o: $(SRC_DIR)/%.asm
	mkdir -p $(dir $@)
	nasm -felf32 -g $< -o $@

clean:
	rm -rf $(BIN_DIR)/boot.bin $(BIN_DIR)/kernel.bin $(BIN_DIR)/os.bin $(BUILD_DIR)/*

qemu:
	qemu-system-i386 -hda $(BIN_DIR)/os.bin -serial stdio
