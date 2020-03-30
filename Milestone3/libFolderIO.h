//LIBRARY FOLDER I/O

#ifndef __LIBFOLDERIO_H
#define __LIBFOLDERIO_H

#include "libFileIO.h"

void createFolder(char* path_, int* success, char parentIndex);
void deleteFolder(char* path_, int *success, char parentIndex);
void listContent(char currDir);

void putStr(char curdir, char argc, char argv[64][128]);
void getCurdir(char *curdir);
void getArgc(char *argc);
void getArgv(char idx, char *argv); 

#endif