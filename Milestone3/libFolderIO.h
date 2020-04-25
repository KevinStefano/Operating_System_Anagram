//LIBRARY FOLDER I/O

#ifndef __LIBFOLDERIO_H
#define __LIBFOLDERIO_H


void createFolder(char* path, int* success, char parentIndex);
void deleteFolder(char *path, int *success, char parentIndex);
void listContent(char currDir);
void cat(char *path, int *success, char parentIndex);


void putStr(char curdir, char argc, char argv[64][128]);
void getCurdir(char *curdir);
void getArgc(char *argc);
void getArgv(char idx, char *argv); 
void readSector(char *buffer, int sector);

#endif