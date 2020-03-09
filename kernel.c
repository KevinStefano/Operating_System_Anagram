
#define MAX_BYTE 256
#define SECTOR_SIZE 512
#define MAX_FILES 16
#define MAX_FILENAME 12
#define MAX_SECTORS 20
#define DIR_ENTRY_LENGTH 32
#define MAP_SECTOR 1
#define DIR_SECTOR 2
#define TRUE 1
#define FALSE 0
#define INSUFFICIENT_SECTORS 0
#define NOT_FOUND -1
#define INSUFFICIENT_DIR_ENTRIES -1
#define EMPTY 0x00
#define USED 0xFF
#define LOGO_COLUMN 72
#define LOGO_ROW 8

void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string); //
void readString(char *string); //
void readSector(char *buffer, int sector); //
void writeSector(char *buffer, int sector); //
void readFile(char *buffer, char *filename, int *success);
void clear(char *buffer, int length); // Fungsi untuk mengisi buffer dengan 0 //
void writeFile(char *buffer, char *filename, int *sectors);
void executeProgram(char *filename, int segment, int *success);//
int doesFileNameExist(char* buffer, char* filename);
int mod(int bil1, int bil2); //
int div(int bil1, int bil2); //
void logo();//
void enter();

char input[100];
char buff[1000];
char buffs[10000];
char pengguna[1000];
char c[5] =  {'a','b','c','d'};
int succ = 0;
int sec = 10;
char buffer[SECTOR_SIZE * MAX_SECTORS];

int main() {
    makeInterrupt21();
    logo();
    enter();
    enter();
    printString("executee");
    enter();
    enter();
    interrupt(0x21, 0x4, buffer, "key.txt", &succ);
    if (succ) {
        interrupt(0x21,0x0, "Key : ", 0, 0);
	 	interrupt(0x21,0x0, buffer, 0, 0);
    }
    else {
        interrupt(0x21, 0x6, "milestone1", 0x2000, &succ);
    }
  while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX){
  switch (AX) {
    case 0x0:
      printString(BX);
      break;
    case 0x1:
      readString(BX);
      break;
    case 0x2:
      readSector(BX, CX);
      break;
    case 0x3:
      writeSector(BX, CX);
      break;
    case 0x4:
      readFile(BX, CX, DX);
      break;
    case 0x5:
      writeFile(BX, CX, DX);
      break;
    case 0x6:
      executeProgram(BX, CX, DX);
      break;
    default:
      printString("Invalid interrupt");
  }
}
void printString(char *string){
    int i = 0;
    while(string[i] != '\0'){
        interrupt(0x10, (0xe<<8)+string[i], 0, 0, 0);
        i++;
    }

}
void readString(char* string)
{
    //Inisialisasi awal string
    int flag =0;
    int i=0;

    //Proses string selanjutnya
    //Dilakukan selama bukan enter
    while (flag==0) {      

        int huruf = interrupt(0x16,0,0,0,0);
        if (huruf == '\r')
		{
			string[i] = '\0';
            flag=1;
		}
        else {
            //Jika huruf masukkan adalah backspac
            if (huruf == 0x8 ) {
                    interrupt(0x10, (0xe<<8)+0x8, 0, 0, 0); //backspace 1x
                    interrupt(0x10, (0xe<<8)+0x0, 0, 0, 0); //jadiin nul
                    interrupt(0x10, (0xe<<8)+0x8, 0, 0, 0); //backspace 1x
                if (i>=1) {
                    string[i] = '\0';
                    i--;
                }
    
            }
            else {
                string[i] = huruf;
                interrupt(0x10, (0xe<<8)+huruf, 0, 0, 0);
                i++;
            }
        }
         
    }
    enter();
 }

void readSector(char *buffer, int sector) 
{
    interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}
void writeSector(char *buffer, int sector) {
    interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector,18) + 1, mod(div(sector,18), 2) * 0x100);
}
void readFile(char *buffer, char *filename, int *success) 
{
    char dir[512];
    int i,j,k;
    int flag;
    int founded = 0;
    //Baca sektor dir
    readSector(dir, 2);

    //Mengecek nama file
    for (i=0;i<512;i+=32) {
        flag =1;
        //PROSES CHECK NAMA FILE
        for (k=0; k<12;k++) {
            if (filename[k]!='\0') {
                if (filename[k] != dir[i+k]) {
                    flag = -1;
                    break;
                }
            }
            else {
                break; //iterasi berhenti, file kosong
            }
        }

        if (flag==1) {
            founded = 1;
            break;
        }
    }

    if (founded==0) {
        *success = 0;
        enter();
        return;
    }
    else {
        for(j = 0; j < 20; j++) {
            if (dir[i+12+j]!=0) {
                readSector(buffer+j*512, dir[i+12+j]);
            }
            else {
                break;
            }
        }

        *success = 1;
        printString("Read File Success");
        enter();
        return;
    }
}

void clear(char *buffer, int length) {
    int i;
    for(i = 0; i < length; i++) {
        buffer[i] = 0x0;
    } 
}
void writeFile(char *buffer, char *filename, int *sectors) 
{
    char map[512];
    char dir[512];
    char sub_buffer[512];
    int dir_index, sector_idx=0, sector_num;
    int i,j,k,l;

    //Baca sector map dan dir
    readSector(map,1);
    readSector(dir,2);

    //Cek dir kosong
    for(dir_index = 0; dir_index < 16; dir_index++) {
        if(dir[dir_index*32] == '\0') {
            break;
        }
    }

    if(dir_index>=16) {
        printString("Not able to find an empty directory for the current file.");
        *sectors = -1;
        return;
    }
    else {
         //Cek jumlah sektor di map cukup untuk buffer file
        for(k = 0, sector_num = 0; k < 512 && sector_num<(*sectors); k++) {
            if(map[k] == 0x00) {
                sector_num++;
            }
        }
        if(sector_num < *sectors) {
            printString("Not enough directory space for the current file.");
            *sectors = 0;
            return;
        }
        else {
            //Bersihkan sektor yang akan digunakan
            clear(dir+dir_index*32,32);

            //Isi sektor pada dir dengan nama file
            for(i = 0; i < 12; i++) {
                if (filename[i] != '\0') {
                    dir[dir_index*32 + i] = filename[i];
                }
                else {
                    break;
                }
            }

            for(j = 0, sector_num =0; j < 512 && sector_num < (*sectors); j++) {
                if (map[j] == 0x00) {
                    //Menandai di map
                    map[j] = 0xFF;

                    //Tandai di dir
                    dir[dir_index*32+12+sector_num ] = j;
                    clear(sub_buffer,512);
                    for(l = 0; l < 512; l++) {
                        sub_buffer[l] = buffer[sector_num *512+l];
                    }
                    writeSector(sub_buffer,j);
                    sector_num++;
                }
            }
        }     
    }
    writeSector(map,1);
    writeSector(dir,2);
    printString("File accepted");
    enter();
}

void executeProgram(char *filename, int segment, int *success) {
    char _buffer[10240];
    int i;

    readFile(_buffer,filename,success);
    if(*success == 1) {
        for(i = 0; i < 10240; i++) {
            putInMemory(segment, i, _buffer[i]);
        }
        launchProgram(segment);
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

void enter() {
    interrupt(0x10, 0xe*256+'\r',0,0,0);
    interrupt(0x10, 0xe*256+'\n',0,0,0);
}
void logo() {
printString("          .dWeL  udWbL "); enter();
printString("         :$$$$$.x$$$$$:"); enter();
printString("     ... 9$$$$$E|$$$$$E  .."); enter();
printString("    d$$$e`$$$$$F9$$$$$|o$$$N."); enter();
printString("   d$$$$$kR$$$---$$$$$z$$$$$&"); enter();
printString("   $$$$$$$/$$<| |'$$$$&$$$$$$R"); enter();
printString("   ^*$$$$$$|$$| |$$$F$$$$$$*"); enter();
printString(" .uu.|R$$$$c$$| |$$$x$$$$P|.uu."); enter();
printString("o$$$$$u?*$$$/$| |4$$$$$*)o$$$$$c"); enter();
printString("$$$$$$$$oC#$b#| |#'F@$#)d$$$$$$$$"); enter();
printString("*$$$$$$$$$NU#(| |)#x#u$$$$$$$$$$P"); enter();
printString(" |***$$$$$$$NU| |UNb$$$$$$$$***|"); enter();
printString(" -------------------------------- "); enter();
}
