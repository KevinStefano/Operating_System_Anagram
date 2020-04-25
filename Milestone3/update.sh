dd if=/dev/zero of=system.img bs=512 count=2880
nasm bootloader.asm -o bootloader
dd if=bootloader of=system.img bs=512 count=1 conv=notrunc
dd if=map.img of=system.img bs=512 count=1 seek=256 conv=notrunc
dd if=files.img of=system.img bs=1024 count=1 seek=257 conv=notrunc
dd if=sectors.img of=system.img bs=512 count=1 seek=259 conv=notrunc
bcc -ansi -c -o kernel.o kernel.c
bcc -ansi -c -o libFileIO.o libFileIO.c
bcc -ansi -c -o libFolderIO.o libFolderIO.c
bcc -ansi -c -o libMatematika.o libMatematika.c
bcc -ansi -c -o libTeks.o libTeks.c
nasm -f as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o
dd if=kernel of=system.img bs=512 conv=notrunc seek=1

gcc loadFile.c -o loadFile


nasm -f as86 lib.asm -o lib_asm.o
bcc -ansi -c -o shell.o shell.c
ld86 -o shell -d shell.o lib_asm.o
./loadFile shell

bcc -ansi -c -o ls.o ls.c
ld86 -o ls -d ls.o lib_asm.o libFileIO.o libFolderIO.o libMatematika.o libTeks.o
./loadFile ls

bcc -ansi -c -o mkdir.o mkdir.c
ld86 -o mkdir -d mkdir.o lib_asm.o libFileIO.o libFolderIO.o libMatematika.o libTeks.o
./loadFile mkdir


bcc -ansi -c -o rm.o rm.c
ld86 -o rm -d rm.o lib_asm.o libFileIO.o libFolderIO.o libMatematika.o libTeks.o
./loadFile rm


bcc -ansi -c -o cat.o cat.c
ld86 -o cat -d cat.o lib_asm.o libFileIO.o libFolderIO.o libMatematika.o libTeks.o
./loadFile cat

echo c | bochs -f if2230.config