dd if=/dev/zero of=system.img bs=512 count=2880
nasm bootloader.asm -o bootloader
dd if=bootloader of=system.img bs=512 count=1 conv=notrunc
dd if=map.img of=system.img bs=512 count=1 seek=256 conv=notrunc
dd if=files.img of=system.img bs=1024 count=1 seek=257 conv=notrunc
dd if=sectors.img of=system.img bs=512 count=1 seek=259 conv=notrunc
bcc -ansi -c -o kernel.o kernel.c
nasm -f as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o
dd if=kernel of=system.img bs=512 conv=notrunc seek=1

nasm -f as86 lib.asm -o lib_asm.o
bcc -ansi -c -o shell.o shell.c
ld86 -o shell -d shell.o lib_asm.o
./loadFile shell

bcc -ansi -c -o libMatematika.o libMatematika.c
ld86 -o libMatematika -d libMatematika.o lib_asm.o
./loadFile libMatematika

bcc -ansi -c -o libTeks.o libTeks.c
ld86 -o libTeks -d libTeks.o lib_asm.o
./loadFile libTeks

bcc -ansi -c -o libFileIO.o libFileIO.c
ld86 -o libFileIO -d libFileIO.o lib_asm.o
./loadFile libFileIO

echo c | bochs -f if2230.config