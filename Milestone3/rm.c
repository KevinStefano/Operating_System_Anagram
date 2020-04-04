#include "libFolderIO.h"
#include "libFileIO.h"

int main () {
    char curdir;
    int success;
    int berhasil;
    char argv[0][14];

    //Inisialisasi

    printString("Proses penghapusan di rm.c\n");
    getCurdir(&curdir);
    getArgv(0,argv[0]);
    deleteFolder(argv[0],&berhasil,curdir);
    deleteFile(argv[0],&berhasil,curdir);
}


#include "libFolderIO_imp.h"
#include "libFileIO_imp.h"
