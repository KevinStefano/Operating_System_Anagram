//LIBRARY FOLDER I/O

#ifndef __LIBFOLDERIO_H
#define __LIBFOLDERIO_H

#include "libFileIO.h"

void createFolder(char* dir, char matriks[64][14], int* success, char parentIndex);
void deleteFolder();
void listContent();

void putStr(char curdir, char argc, char argv[64][128]);
void getCurdir(char *curdir);
void getArgc(char *argc);
void getArgv(char idx, char *argv); 
void clear(char *buffer, int length);


void createFolder(char* dir, char matriks[64][14], int* success, char parentIndex) {
    //Buat MKDIR 
    //Punya Sam belom di revisi
    int  i, j, k, foldername_length;
    char idx;
    *success = 0;
    for(k = 1; matriks[k][0] != 0x00; k++) {
        lengthString(matriks[k],&foldername_length);
        searchIndexbyFileName(dir,matriks[k],parentIndex,&idx);
        if(IsStringSameBol(&idx,-1 +'0') || IsStringSameBol(idx,-2 + '0')) {
            for(i = 0; i < 64; i++) {
                if(dir[i*16+2] == 0x00) break;
            }
            if(i == 64) {
                *success = -2;
                return;
            }

            dir[i*16] = parentIndex;
            dir[i*16+1] = 0xFF;

            for(j = 0; j < 14; j++) {
                dir[i*16+2+j] = 0x00;
            }
            for(j = 0; j < foldername_length; j++) {
                dir[i*16+2+j] = matriks[k][j];
            }
            *success = 1;
            return;
        }
        *success = -1;
        return;              
    } 
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
    interrupt(0x21, 0x03, str, 512,0);
}

void getCurdir(char *curdir) {
    char str[512];
    interrupt(0x21, 0x02, str, 512,0);
    *curdir = str[0];
}

void getArgc(char *argc) {
    char str[512];
    interrupt(0x21, 0x02, str, 512,0);
    *argc = str[1];
}
void getArgv(char idx, char *argv) {
    int idx1 = 2;
    int idx3 = 0;
    char str[512];
    interrupt(0x21, 0x02, str, 512,0);

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

void clear(char *buffer, int length) {
    int i;
    for(i = 0; i < length; i++) {
        buffer[i] = 0x0;
    } 
}

#endif