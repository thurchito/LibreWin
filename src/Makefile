FILES = ./build/kernel.asm.o ./build/kernel.o ./build/disk/disk.o ./build/disk/streamer.o ./build/fs/pparser.o ./build/fs/file.o ./build/fs/fat/fat16.o ./build/idt/idt.asm.o ./build/idt/idt.o ./build/memory/memory.o ./build/io/io.asm.o ./build/memory/heap/heap.o ./build/memory/heap/kheap.o ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/string/string.o
INCLUDES = -I./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> ./bin/os.bin
	sudo mount -t vfat ./bin/os.bin /mnt/d
	# Copy a file over
	sudo cp ./hello.txt /mnt/d
	sudo umount /mnt/d
./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./base/txos/init/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./bin/boot.bin: ./base/txos/boot/fat/x86fboot.asm
	nasm -f bin ./base/txos/boot/fat/x86fboot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./base/txos/init/kernel.asm
	nasm -f elf -g ./base/txos/init/kernel.asm -o ./build/kernel.asm.o

./build/kernel.o: ./base/txos/init/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./base/txos/init/kernel.c -o ./build/kernel.o

./build/idt/idt.asm.o: ./base/txos/ke/idt/idt.asm
	nasm -f elf -g ./base/txos/ke/idt/idt.asm -o ./build/idt/idt.asm.o

./build/idt/idt.o: ./base/txos/ke/idt/idt.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/idt $(FLAGS) -std=gnu99 -c ./base/txos/ke/idt/idt.c -o ./build/idt/idt.o

./build/memory/memory.o: ./base/txos/ke/memory/memory.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/memory $(FLAGS) -std=gnu99 -c ./base/txos/ke/memory/memory.c -o ./build/memory/memory.o

./build/io/io.asm.o: ./base/txos/ke/io/io.asm
	nasm -f elf -g ./base/txos/ke/io/io.asm -o ./build/io/io.asm.o

./build/memory/heap/heap.o: ./base/txos/ke/memory/heap/heap.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/memory/heap $(FLAGS) -std=gnu99 -c ./base/txos/ke/memory/heap/heap.c -o ./build/memory/heap/heap.o

./build/memory/heap/kheap.o: ./base/txos/ke/memory/heap/kheap.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/memory/heap $(FLAGS) -std=gnu99 -c ./base/txos/ke/memory/heap/kheap.c -o ./build/memory/heap/kheap.o

./build/memory/paging/paging.o: ./base/txos/ke/memory/paging/paging.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/memory/paging $(FLAGS) -std=gnu99 -c ./base/txos/ke/memory/paging/paging.c -o ./build/memory/paging/paging.o

./build/memory/paging/paging.asm.o: ./base/txos/ke/memory/paging/paging.asm
	nasm -f elf -g ./base/txos/ke/memory/paging/paging.asm -o ./build/memory/paging/paging.asm.o

./build/disk/disk.o: ./base/txos/ke/disk/disk.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/disk $(FLAGS) -std=gnu99 -c ./base/txos/ke/disk/disk.c -o ./build/disk/disk.o

./build/disk/streamer.o:./base/txos/ke/disk/streamer.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/disk $(FLAGS) -std=gnu99 -c ./base/txos/ke/disk/streamer.c -o ./build/disk/streamer.o

./build/fs/fat/fat16.o: ./base/txos/ke/fs/fat/fat16.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/fs -I./src/fat $(FLAGS) -std=gnu99 -c ./base/txos/ke/fs/fat/fat16.c -o ./build/fs/fat/fat16.o

./build/fs/file.o: ./base/txos/ke/fs/file.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/fs $(FLAGS) -std=gnu99 -c ./base/txos/ke/fs/file.c -o ./build/fs/file.o

./build/fs/pparser.o: ./base/txos/ke/fs/pparser.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/fs $(FLAGS) -std=gnu99 -c ./base/txos/ke/fs/pparser.c -o ./build/fs/pparser.o

./build/string/string.o: ./base/txos/ke/string/string.c
	i686-elf-gcc $(INCLUDES) -I./base/txos/ke/string $(FLAGS) -std=gnu99 -c ./base/txos/ke/string/string.c -o ./build/string/string.o

clean:
	rm -rf ./bin/boot.bin
	rm -rf ./bin/kernel.bin
	rm -rf ./bin/os.bin
	rm -rf ${FILES}
	rm -rf ./build/kernelfull.o
