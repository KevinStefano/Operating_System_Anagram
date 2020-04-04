#include "libFolderIO.h"
#include "libFileIO.h"

int main () {
    char curdir;
    int success;
    int berhasil;
    char argv;

    //Inisialisasi
    getCurdir(&curdir);
    getArgv(0,&argv);
    cat(&argv,&success,curdir);
}