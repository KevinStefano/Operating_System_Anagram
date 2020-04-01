#include "libFileIO.h"
#include "libFolderIO.h"
#include "libMatematika.h"

int command(char* input);
void enter();
void listAll(char* dir, char matriks[64][14], char parentIndex);
void checkmatriks(char matriks[64][14], char *curdir, char* dirs, char *succes);
void printString(char *string);
void printMatrikstoPath(char matriks[64][14]);



int main() {
          
    interrupt(0x21, 0x0, "Welcome to ANAGRAMS SHELL 1.0 ",0,0);
    enter();

    while (1) {

        char masukkan[200];
        int type_masukkan;
        int sumKataSetelahSpasi;
        int kataSetelahSpasi;
        char dirsOrFile[1024];
        char path[14];
        char fileName[14];
        int output;
        int i=0;
        int j=0;
        int l;
        int idxoutput;
        int out;
        int success = 0;
        char curdir = 0xFF;
        char argc;
        char argv[64][14];

        interrupt(0x21,0x02,dirsOrFile,0x101,0);
        interrupt(0x21,0x02,dirsOrFile+512,0x102,0);
        interrupt(0x21, 0x0, "Anagram/",0,0);
        
        printString("> ");
        interrupt(0x21, 0x1, &masukkan,0,0);

        type_masukkan = command(masukkan);
        countChar(masukkan,0x20,&sumKataSetelahSpasi);
        makePathtoMatriks(masukkan, 0x20, argv);

        //Ambil element kedua matriks
        clear(path,14);
        while(j<14) {
            path[j] = argv[1][j];
            j++;
        }

        //Inisialisasi arc, curdir
        interrupt(0x21, 0x08 &curdir, 0, 0);
        argc = sumKataSetelahSpasi;
        
        if (type_masukkan == 111) { //cd
            if (sumKataSetelahSpasi>=2) {
                interrupt(0x21, 0x00, "Masukkan salah", 0,0);
                enter();
            }
            else if (sumKataSetelahSpasi==0) {
                curdir = 0xFF;
                putStr(curdir,0,0);
            }
            else {
                isStringSame(path, "..",&out);
                if (argc ==1 && IsStringSameBol(argv[1],"..")) {
                    if (curdir == 0xFF) {
                        interrupt(0x21, 0x00, "Stay on level",0,0);
                        enter();
                    }
                    else {
                        interrupt(0x21, 0x00, "Back 1 level ",0,0);
                        curdir = dirsOrFile[curdir * 16];
                        putStr(curdir, 0, 0);
                        enter();
                        }  
                    }
                
                else {
                    //KASUS cd a/b/c/d
                        searchDirectoryParent(dirsOrFile,argv[1], &idxoutput, &out, curdir);
                        if (out==0) {
                            interrupt(0x21, 0x00, "Gagal... ",0,0);
                            enter();
                        }
                        else {
                            interrupt(0x21, 0x00, "Go to ",0,0);
                            printString(argv[1]);
                            enter();
                            curdir = idxoutput;
                            putStr(curdir,0,0);
                        }
                }
            }
        }
        
        else if(type_masukkan == 112) { // ls

            putStr(curdir, argc, argv + 1);
            listContent(curdir);
        }
        else if (type_masukkan == 113) {
            clear(fileName,14);
            i=2;
            j=0;
            while(masukkan[i]!=0x00 &&j<14) {
                fileName[j] = masukkan[i];
                i++;
                j++;
            }
            fileName[j]=0x00;
            if (fileName[0]!=0x00) {
                out=0;
                interrupt(0x21, curdir << 8 | 0x6, &fileName, 0x2000, &out);
                enter();
                if (out==1) {
                    printString("Berhasil execute");
                    enter();
                }
                else {
                    printString("Gagal execute");
                    enter();
                }
            }
        }
        else if(type_masukkan == 115) { // mkdir
            if(argv[1][0] == 0x00) {
                interrupt(0x21,0x00,"Nama folder tidak ada\n\r",0,0);
            }
            else {
                createFolder(dirsOrFile,&success,curdir);
                if(success == -2) {
                    interrupt(0x21,0x00,"Entry tidak cukup\n\r",0,0);
                }
                else if(success == -1) {
                    interrupt(0x21,0x00,"Folder sudah ada\n\r", 0,0);
                }
                else {
                    printString("Berhasil");
                    enter();
                }
            }
        }
    }
}

int command(char* input) {

    char masukkan[50];
    int lengthStringSebelomSpace;
    int bol;
    int it=0;
    
    //Pencarian lengthstringSebelom space OR sebelum /
    while (input[it]!=0x00) {
        if (input[it]==0x20) { // space
            break; 
        }
        it++;
    }

        if (input[it]==0x20) {
            lengthStringSebelomSpace = it;
        }
        else {
            lengthString(input,&lengthStringSebelomSpace);
        }

        copyString(input,masukkan,0,lengthStringSebelomSpace);

        //Mencocokkan dengan cd
        isStringSame(masukkan,"cd",&bol);
        if (bol==1) {
            bol=0;
            return 111;
        }
        isStringSame(masukkan,"ls",&bol);
        if (bol==1) {
            bol=0;
            return 112;
        }

        if (masukkan[0]==0x2E && masukkan[1]=='/') {
            bol=0;
            return 113;
        }
        isStringSame(masukkan, "echo", &bol);
        if (bol==1) {
            bol=0;
            return 114;
        }
        isStringSame(masukkan, "mkdir", &bol);
        if(bol==1) {
            bol = 0;
            return 115;
        }
}

void printMatrikstoPath(char matriks[64][14]) {
    int i=0;
    int j=0;
    while (i<64) {
        printString(matriks[i]);
        if (matriks[i]==0x00) {
            printString("/");
        }
        i++;
    }
}

void checkmatriks(char matriks[64][14], char *curdir, char* dirs, char *succes) {
    // cd a/b/c
    //c curdir

    int fileName[14];
    char curdirs;
    char idxP;
    char idx;
    int output;
    int out;
    int i =0;
    int j,k;
    int it=0;
    int idxParent;
    int simpanIdx;
    int bol =0;
    *succes =1;

    curdirs = *curdir;
    while (matriks[i]!=0x00 && bol==0) {
        out =0;
        isStringSame(matriks[i],"..",&out); 
        //Jika sama dengan ".."
        if (out==1){
            i++;
        }
        //Jika beda dengan ".."
        else {
            idx =0x00;
            searchIndexbyFileName(dirs,matriks[i],curdirs,&idx);
           
            simpanIdx=idx;
            //Pengecheckan dengan keluaran idx
            //Jika nilai Idx>0 atau matriks nilainya sama dengan parent curdirs
            if (!IsStringSameBol(&idx,'0'+(-1))&&!IsStringSameBol(&idx,'0'+(-2))) { 
                
                curdirs = simpanIdx;
                //Jika curdir skrg bukan file
                if (dirs[simpanIdx]==0x00 && dirs[simpanIdx+1]==0x00) {
                    interrupt(0x21, 0x00,"Folders/file tidak ada",0,0);
                    enter();
                    *succes =0;
                    break;
                    
                }
                else if(dirs[simpanIdx+1]!=0xFF) {
                    i++;
                }
                //Jika curdir sekarang adalah file
                else {
                    //Jika masih ada next di matriksnya gagal
                    if (matriks[i+1]!=0x00) {
                        *succes = 0;
                        break; //Program gawgal
                    }
                    else {
                        *succes = 1;
                        break;
                    }
                }
            }
            //jika nilai matriks i tidak sama dengan nilai parent i curdirs
            else{
                idxP = 0x00;
                while(1) {
                    if (idxP == 0x4F) { //BASIS, jika tidak ditemukan
                        bol=1;
                        *succes = 0;
                        break;
                    }
                    else if (idxP ==curdirs) { //BASIS
                        curdirs = simpanIdx;
                        i++;
                        break;
                    }
                    else if(idxP==0xFF) { //BASIS, jika parent root
                        bol=1;
                        *succes =0;
                        break;
                    }
                    //Jika idx==-2
                    //Jika berbeda dengan parent, artinya ada ".."
                    else {
                        //Search idxParent
                        out=0;
                        for(j=0;j<64;j++) {
                            clear(fileName,14);
                            for (k=0;k<14;k++) {
                                fileName[j] = dirs[j*16+2+k];
                            }
                            isStringSame(fileName,matriks[i],&out);
                            if (out==1) {
                                break;
                            }
                        }

                        if (out==1) {
                            idxP= dirs[j];
                        }
                        else if (out==0) {
                            idxP = 0x4F;
                        }
                    }
                }
            }
        }
    }
    *curdir=curdir;
}

void enter() {
    interrupt(0x10, 0xe*256+'\r',0,0,0);
    interrupt(0x10, 0xe*256+'\n',0,0,0);
}

void listAll(char* dir, char matriks[64][14], char parentIndex) {
    int i, j, idx;
    char foldername[14];
    if(matriks[1][0] == 0x00) {
        for(i = 0; i < 64; i++) {
            if(dir[i*16] == parentIndex) {
                clear(foldername,14);
                for(j = 0; j < 14; j++) {
                    foldername[j] = dir[i*16+2+j];
                }
                interrupt(0x21,0x00,foldername,0,0);
                interrupt(0x21,0x00," ", 0, 0);
            }
        }
        interrupt(0x21,0x00, "\n\r",0,0);
        return;
    }
    for(i = 1; matriks[i][0] != 0x00; i++) {
        interrupt(0x21,0x00,matriks[i],0,0);
        interrupt(0x21,0x00,":\n\r",0,0);
        searchIndexbyFileName(dir,matriks[i],parentIndex,&idx);
        if(idx == -1 || idx == -2) {
            continue;
        }
        for(j = 0; j < 64; i++) {
            if(dir[j*16] == idx) {
                clear(foldername,14);
                for(j = 0; j < 14; j++) {
                    foldername[j] = dir[i*16+2+j];
                }
                interrupt(0x21,0x00,foldername,0,0);
                interrupt(0x21,0x00," ",0,0);
            }
        }
        interrupt(0x21,0x00,"\n\r",0,0);
    }
}

void printString(char *string){
    int i = 0;
    while(string[i] != '\0'){
        interrupt(0x10, (0xe<<8)+string[i], 0, 0, 0);
        i++;
    }
}

#include "libFileIO_imp.h"
#include "libFolderIO_imp.h"
#include "libMatematika_imp.h"