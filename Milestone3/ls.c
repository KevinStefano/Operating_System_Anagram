#include "libFolderIO.h"
#include "libFileIO.h"

int main () {
    char curdir;
    int success;
    int berhasil;

    //Inisialisasi
    getCurdir(&curdir);
    printString("File/Folder yang ada :\n");
    listContent(curdir);

}


#include "libFolderIO_imp.h"
#include "libFileIO_imp.h"
