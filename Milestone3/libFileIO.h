//LIBRARY FILE I/O

#ifndef __LIBFILEIO_H
#define __LIBFILEIO_H


void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void deleteFile(char *path, int *result, char parentIndex);
void copyFile(char* pathasal, char* pathtujuan, int* result, char parentIndex);
void moveFile(char* pathasal, char* pathtujuan, int* result, char parentIndex);
void prosesCat(char *path, int *success, char parentIndex) ;


#endif
