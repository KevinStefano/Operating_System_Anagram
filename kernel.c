
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

interrupt(0x10, 0xe*256+'B',0,0,0);
    readString();
    //printString(buff);

  makeInterrupt21();
  while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX){}


void printString(char *string){
    int i = 0;
    while(string[i] != '\0'){
        interrupt(0x10, (0xe<<8)+string[i], 0, 0, 0);
        i++;
    }

}
    
void readString(char* string){

    //Inisialisasi awal string
    int flag =0;
    int i=0;
    string[i] =0xa;

    //Proses string selanjutnya
    //Dilakukan selama bukan enter
    while (flag==0) {      

        int huruf = interrupt(0x16,0,0,0,0);
         //Jika huruf masukkan adalah backspace
        if (huruf == 0x8 ) {
            string[i]=0x0;
            i--;
            interrupt(0x10, (0xe<<8)+0x8, 0, 0, 0); //backspace 1x
            interrupt(0x10, (0xe<<8)+0x0, 0, 0, 0); //jadiin nul
            interrupt(0x10, (0xe<<8)+0x8, 0, 0, 0); //backspace 1x
  
        }
         //jika huruf masukkasn enter
        else if (huruf == 0xd) {
            string[i]='\0';
            flag=1;
        }
        else {
            string[i] = huruf;
            interrupt(0x10, (0xe<<8)+huruf, 0, 0, 0);
            i++;
        }
    }
 }

void readSector(char *buffer, int sector) 
{
    interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
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


