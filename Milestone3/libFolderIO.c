
void createFolder(char* path, int* success, char parentIndex) {
    interrupt(0x21, parentIndex << 8 | 0x21, *path, *success, 0);
}

void deleteFolder(char *path, int *success, char parentIndex){
    interrupt(0x21, parentIndex << 8 | 0x23, *path, *success, 0);
}

void cat(char *path, int *success, char parentIndex){
    interrupt(0x21, parentIndex << 8 | 0x25, *path, *success, 0);
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

void readSector(char *buffer, int sector) 
{
    interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}
