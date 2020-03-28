#ifndef __LIBFOLDERIOIMP_H
#define __LIBFOLDERIOIMP_H

#include "libFileIO_imp.h"

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

void deleteFolder(char *path, int *success, char parentIndex)
{
    
    int i,j;
    int idx = 0;
    int idxacuan;
    char result;
    char dirsOrFile[1024];
    char maps[512];
    char sectors[512];
    char fileName[14];

    //Inisialisasi awal dengan memasukkan data
    interrupt(0x21,0x02,maps,0x101,0);
    interrupt(0x21,0x02,dirsOrFile,0x101,0);
    interrupt(0x21,0x02,dirsOrFile+512,0x102,0);
	interrupt(0x21, 0x02, sectors, 0x103, 0);
    searchFile(dirsOrFile, path, &idxacuan, &result, parentIndex);

    if (result==0) {
        *success = -1; //Tidak diketemukan
    }
    else {
        /*
        
        //Hapus dalam DIrsrFile
        while (idx<1024) {
            //Dilakukan saat dirsOrFile idx ke 0 nya bernilai idx acuan
            if(dirsOrFile[idx]==idxacuan && idx != idxacuan) {
                //Jika nilai fileNamenya bukan 0x00
                if (dirsOrFile[idx+2]!=0x00 ) {
                    clear(fileName,14);
                    //salin ke fileName
                    for (i=0; i<14; i++) {
                        fileName[i] = dirsOrFile[idx+i+2];
                    }
                    //lakukan rekursif
                    deleteFolder(fileName, success, idxacuan);
                    interrupt(0x21,0x02,maps,0x101,0);
                    interrupt(0x21,0x02,dirsOrFile,0x101,0);
                    interrupt(0x21,0x02,dirsOrFile+512,0x102,0);
                    interrupt(0x21, 0x02, sectors, 0x103, 0);
                
                    //Hapus folder
                    dirsOrFile[i] = 0x00;
                    dirsOrFile[i+1] = 0x00;
                    dirsOrFile[i+2] = 0x00;
                    //Hapus semua file yang ada di dalam folder
                    j=0;
                    while (j<16) {
                        //Hapus map dan sectors
                        maps[sectors[idx*16 + j]]==0x00;
                        sectors[idx*16+j]==0x00;

                        j++;
                        if (sectors[idx*16+j]==0x00 || j>=16) {
                            break;
                        } 
                    }
                }
            } 
            idx = (idx+14)+2;
        }

        //Jadikan nol id ke 0, idx ke 1 idx ke 2
        dirsOrFile[idx*15]= 0x00;
        dirsOrFile[(idx*15)+1]= 0x00;
        dirsOrFile[(idx*15)+2]= 0x00;

        //Tulis ke sector guys ~~
        writeSector(maps,0x100);
        writeSector(dirsOrFile,0x101);
        writeSector(dirsOrFile+512,0x102);
        writeSector(sectors,0x103);
        */
        *success = 0;
    }
    

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
    str[idx1] =  argc;
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

#endif