//LIBRARY FILE I/O

void moveFile(char* pathasal, char* pathtujuan, int* result, char parentIndex) {
    interrupt(0x21,parentIndex << 8 | 0x27, *pathasal, *pathtujuan, &result);
}

void copyFile(char* pathasal, char* pathtujuan, int* result, char parentIndex) {
    interrupt(0x21,parentIndex << 8 | 0x26, *pathasal, *pathtujuan, &result);
}

void prosesCat(char *path, int *success, char parentIndex) {
    interrupt(0x21, parentIndex << 8 | 0x25, path, &success, 0);
}

void readFile(char *buffer, char *path, int *result, char parentIndex)
{
    interrupt(0x21, parentIndex << 8 | 0x04, *buffer, *path, *result);
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
   interrupt(0x21, parentIndex << 8 | 0x05, *buffer, *path, *sectors);
}

void deleteFile(char *path, int *result, char parentIndex) {
     interrupt(0x21, parentIndex << 8 | 0x22, *path, *result);
}



void printString(char *string){
    int i = 0;
    while(string[i] != '\0'){
        interrupt(0x10, (0xe<<8)+string[i], 0, 0, 0);
        i++;
    }
}


