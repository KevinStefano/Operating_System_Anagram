//LIBRARY FOLDER I/O

#ifndef __LIBFOLDERIO_H
#define __LIBFOLDERIO_H

void createFolder();
void deleteFolder();
void listContent();

void putStr(char curdir, char argc, char argv[64][128]);
void getCurdir(char *curdir);
void getArgc(char *argc);
void getArgv(char idx, char *argv); 



void createFolder() {

}
void deleteFolder() {

}
void listContent() {

}


void putStr(char curdir, char argc, char argv[64][128]){
    int idx1 = 0;
    int idx2 = 0;
    int idx3 = 0;

    //Inisialisasi str[]
    //str[0] = curdir
    //str[1] = arg count
    //str[2 ..] = argv
    char str[512];
    clear(str,512);
    str[idx1] = curdir;
    idx1++;
    str[idx1] = = argc;
    idx1++;

    //Masukkan argv ke dalam str[]
    while (idx1<512 && idx2<argc) {
        str[idx1] = argv[idx2][idx3];

        //Jika argv belom habis
        if (argv[idx2][idx3]!= 0x00){
            idx3++;
        }
        //Jika sudah habis dalam  baris
        else {
            idx3 = 0;
            idx2++;
        }
        idx1++;
    }
    writeSector(str, 512);
}

void getCurdir(char *curdir) {
    char str[512];
    readSector(str,512);
    *curdir = str[0];
}

void getArgc(char *argc) {
    char str[512];
    readSector(str,512);
    *argc = str[1];
}
void getArgv(char idx, char *argv) {
    int idx1 = 2;
    int idx3 = 0;
    char str[512];
    readSector(str, 512);

    idx1 = idx;
    //Mengisi ke dalam argv jika sudah ditemukan idexnya di str
    while (idx1<512) {
        argv[idx3] = str[idx1];
        idx3++;
    
        if (str[idx1]==0x00) {
            break;
        }
        idx1++;
    } 
}   

#endif