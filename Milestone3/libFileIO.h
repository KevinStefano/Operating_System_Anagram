//LIBRARY FILE I/O

#ifndef __LIBFILEIO_H
#define __LIBFILEIO_H


int mod(int bil1, int bil2);
int div(int bil1, int bil2);

void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void deleteFile(char *path, int *result, char parentIndex);

void searchIndexbyFileName (char *dir, char* stringInput, char idxParent, char* IdxChildoutput);
void searchFileNameParentbyIndexFromChild(char *dirs, int* idx, char* stringOutput);

int IsStringSameBol(char *stringInput1, char *stringInput2); //output bernilai 0/1 0 jika beda 1 jika sama
void isStringSame (char *stringInput1, char *stringInput2, int *output); //output bernilai 0/1 0 jika beda 1 jika sama
void readSector(char *buffer, int sector) ;
void writeSector(char *buffer, int sector);
void searchFile(char *dirsOrFile, char *path, char *index, char *success, char parentIndex);
void clear(char *buffer, int length);
void takeFileNameFromPath (char *path, char *directoryPath, char *fileName);
void lengthString(char *stringInput, int *length_String);
void copyString (char *stringInput, char *stringOutput, int idxMulai, int panjangKopian);
void searchDirectoryParent(char *dirParent, char *pathParent, char *index, char *output, char idxParent);
void countChar(char *stringInput, char c, int *count_Char);
void makePathtoMatriks (char *path, char c, char matriks[64][14]);
void isSameSector(char *sector, char start, char checker[14], char *index, char *output);
void printString(char *string);

#endif
