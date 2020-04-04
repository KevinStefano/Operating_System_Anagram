#include "libFolderIO.h"
#include "libFileIO.h"

int main () {
    char argc;
    char curdir;
    char argv[0][14];
    int success;
    int berhasil;

    //Inisialisasi
    getCurdir(&curdir);
    getArgc(&argc);
    getArgv(0,argv[0]);

    printString("Proses penambahan mkdir di mkdir.c\n");
    createFolder(argv[0],&success, curdir);
    if(success == 0) {
        printString("Berhasil cuyyy :)");
    }
    else if(success == -1) {
        printString("Folder sudah ada\n\r");
    }
    else if (success ==-2) {
        printString("Entry tidak cukup\n\r");
    }
    else {
        printString("Folder tidak valid\n\r");
    }

    //Deinisialisasi
    putStr(curdir, argc, argv[0]);
    interrupt(0x21, curdir << 8 | 0x6, "shell", 0x2000, &berhasil);
}

#include "libFolderIO_imp.h"
#include "libFileIO_imp.h"
