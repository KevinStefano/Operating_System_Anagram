int main () {
    char curdir;
    int success;
    char berhasil;
    char temp[512];

    readSector(temp, 512);
    //Inisialisasi
    curdir = temp[0];
    printString("File/Folder yang ada :\n\r");
    listContent(curdir);
    printString("-----Berhasil membaca ls.c----------");
    interrupt(0x21, 0x6, "  ", 0x10000, &berhasil);
}

