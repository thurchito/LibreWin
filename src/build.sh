# Add the path of where your i686-elf-gcc cross compiler tools are.
# export PATH="/home/kap/Downloads/bin:$PATH"
make all
qemu-system-i386 -drive format=raw,file=./bin/os.bin -serial stdio
