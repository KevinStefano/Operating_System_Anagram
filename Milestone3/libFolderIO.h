//LIBRARY FOLDER I/O

#ifndef __LIBFOLDERIO_H
#define __LIBFOLDERIO_H


void createFolder(char* path, int* success, char parentIndex);
void deleteFolder(char *path, int *success, char parentIndex);
void listContent(char currDir);

void putStr(char curdir, char argc, char argv[64][128]);
void getCurdir(char *curdir);
void getArgc(char *argc);
void getArgv(char idx, char *argv); 

void createFolder(char* path, int* success, char parentIndex) {
    interrupt(0x21, parentIndex << 8 || 0x21, *path, *success, 0);
}

void deleteFolder(char *path, int *success, char parentIndex){
    interrupt(0x21, parentIndex << 8 || 0x23, *path, *success, 0);
}
void listContent(char currDir) {
    interrupt(0x21, 0x24, currDir, 0, 0);
}

void putStr(char curdir, char argc, char argv[64][128]){
    interrupt(0x21,0x07,curdir, argc, argv);
}

void getCurdir(char *curdir) {
    interrupt(0x21,0x08,*curdir,0,0);
}

void getArgc(char *argc) {
    interrupt(0x21,0x09,*argc,0,0);
}
void getArgv(char idx, char *argv) {
    interrupt(0x21,0x0A,idx, *argv,0);
}   

#endif