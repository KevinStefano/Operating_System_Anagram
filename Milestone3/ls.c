//#include "libFolderIO.h"
//#include "libFileIO.h"

int main () {
    char curdir;
    int success;
    int berhasil;

    //Inisialisasi
    getCurdir(&curdir);
    printString("File/Folder yang ada :\n");
    //listContent(curdir);

    interrupt(0x21, 0x24, curdir, 0, 0);
    interrupt(0x21, curdir << 8 | 0x6, "shell", 0x2000, &berhasil);
}


#include "libFolderIO_imp.h"
#include "libFileIO_imp.h"