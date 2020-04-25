
int main () {
    char curdir;
    int success;
    int berhasil;
    char argv[14];
    char temp[512];
    int idx;
    int i =0;
    //Inisialisasi

    readSector(temp, 512);    
    //Inisialisasi
    curdir = temp[0];
    for (idx=1;idx<=13;idx++) {
        argv[i] = temp[idx+1];
        i++;
    }
    argv[i] = 0x00;
    
    printString("Proses penghapusan di rm.c\n\r");
    deleteFolder(argv,&berhasil,curdir);
    deleteFile(argv,&berhasil,curdir);
    printString("-----Berhasil menghapus ");
    printString(argv);
    printString(" (jika ada di file/folder)-----------");
    interrupt(0x21, 0x6, " ", 0x10000, &berhasil);
}

