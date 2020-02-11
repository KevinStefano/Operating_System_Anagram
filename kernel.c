
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *filename, int *success);
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
void writeFile(char *buffer, char *filename, int *sectors);
void executeProgram(char *filename, int segment, int *success);
int mod(int bil1, int bil2);
int div(int bil1, int bil2);

int main() {
//     putInMemory(0xB000, 0x8000, 'h');
//   putInMemory(0xB000, 0x8001, 0xD);
//   putInMemory(0xB000, 0x8002, 'a');
//   putInMemory(0xB000, 0x8003, 0xD);
//   putInMemory(0xB000, 0x8004, 'l');
//   putInMemory(0xB000, 0x8005, 0xD);

interrupt(0x10, 0xe*256+'H',0,0,0);
    printString("HALOOO");
  makeInterrupt21();
  while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX){}
void readSector(char *buffer, int sector) 
{
    interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void printString(char *string){
    int i = 0;
    while(string[i] != '\0'){
        interrupt(0x10, 0xe*256+string[i], 0, 0, 0);
        i++;
    }

}
int mod(int bil1, int bil2){
    while(bil1 >= bil2){
        bil1 = bil1 - bil2;
    }
    return bil1;
}

int div(int bil1, int bil2){
    int z = 0;
    while(bil2 * z<=bil1){
        z++;
    }
    return z-1;

}


