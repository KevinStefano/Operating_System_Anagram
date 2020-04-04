#include "libFolderIO.h"
#include "libFileIO.h"

int main () {
    char curdir;
    int success;
    int berhasil;
    char argv[1][128];

    //Inisialisasi
    getCurdir(&curdir);
    getArgv(0,argv[0]);
    deleteFolder(argv[0],&berhasil,curdir);
    deleteFile(argv[0],&berhasil,curdir);
}