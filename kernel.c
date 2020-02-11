
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *filename, int *success);
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
void writeFile(char *buffer, char *filename, int *sectors);
void executeProgram(char *filename, int segment, int *success);
int doesFileNameExist(char* buffer, char* filename);
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
void writeSector(char *buffer, int sector) {
    interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector,18) + 1, mod(div(sector,18), 2) * 0x100);
}
void readFile(char *buffer, char *filename, int *success) {
    int sectors[21];
    int file_exist;
    int i,j,buffer_address;

    //Baca sektor dir
    readSector(buffer, 2);

    //Mengecek nama file
    file_exist = doesFileNameExist(buffer, filename);

    if(file_exist == 0) {
        success = 0;
        printString("File not found!");
        return;
    }

    for(i = 0; i < 20; i++) {
        sectors[i] = buffer[file_exist*32+12+i];
    }

    sectors[20] = 0;

    j = 0;
    while(sectors[j] != 0x00) {
        readSector(buffer+buffer_address,sectors[j]);
        buffer_address += 512;
        j++;
    }

    success = 1;
    return;
}
void clear(char *buffer, int length) {
    int i;
    for(i = 0; i < length; i++) {
        buffer[i] = 0x0;
    } 
}
void writeFile(char *buffer, char *filename, int *sectors) {
    char map[512], dir[512], sub_buffer[512];
    int dir_index, free_dir = 0, name_length = 0, sector_idx, name_difference, sector_num;
    int i,j,k,l;
    int val;

    //Baca sector map dan dir
    readSector(map,1);
    readSector(dir,2);

    //Cek dir kosong
    for(dir_index = 0; dir_index < 16; dir_index++) {
        if(dir[dir_index*32] == 0x00) {
            free_dir = 1;
            break;
        }
    }

    if(free_dir == 0) {
        printString("Not able to find an empty directory for the current file.");
        return;
    }

    //Cek jumlah sektor di map cukup untuk buffer file
    for(k = 0; k < (*sectors); k++) {
        sector_num = 0;

        while(map[sector_num] != 0x00) {
            sector_num++;
        }

        if(sector_num == 26) {
            printString("Not enough directory space for the current file.");
            return;
        }
    }

    //Bersihkan sektor yang akan digunakan untuk menyimpan nama

    //Isi sektor pada dir dengan nama file
    while(filename[name_length] != 0x00 && filename[name_length] != '\0') {
        name_length++;
    }

    for(i = 0; i < name_length; j++) {
        dir[dir_index*32 + i] = filename[i];
    }

    if(name_length < 12) {
        while(i < 12) {
            dir[dir_index*32 + i] = 0x00;
            i++;
        }
    }


    for(j = 0; j < (*sectors); j++) {
        sector_idx = 0;

        //Cari sektor di map yang kosong
        while(map[sector_idx] != 0x00) {
            sector_idx++;
        }

        //Menandai di map
        map[sector_idx] = 0xFF;

        dir[dir_index*32+12+j] = sector_idx;

        for(l = 0; l < 512; l++) {
            val = j+1;
            sub_buffer[j] = buffer[j*val];
        }

        writeSector(sub_buffer, sector_idx);
    }

    writeSector(map,1);
    writeSector(dir,2);
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
int doesFileNameExist(char* buffer, char* filename) {
    int i,j,k;
    int found = 0;

    for(i = 0; i < 16; i++) {
        if(buffer[32*i] != 0x00) {
            for(j = 0; j < 12; j++) {
                if(buffer[j+32] == '\n' || buffer[j+32] == '\r' || buffer[j+32] == 0x00 || filename[j] == '\n' || filename[j] == '\r' || filename[j] == 0x00) {
                    break;
                }
                else if(buffer[j+32*i] == filename[j]) {
                    found = 1;
                }
                else {
                    found = 0;
                    break;
                }
            }
        }
    }

    if(found) {
        return i;
    } 
    
    for(k = 0; k < 10240; k++) {
        buffer[k] = 0x00;
    }

    return 0;
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