#include "libFolderIO.h"
#include "libFileIO.h"

int main () {
    char curdir;
    int success;
    int berhasil;
    char argv[0][14];

    //Inisialisasi

    printString("Welcome to cat.c\n");
    getCurdir(&curdir);
    getArgv(0,argv[0]);
    cat(argv[0],&success,curdir);
}


#include "libFolderIO_imp.h"
#include "libFileIO_imp.h"
