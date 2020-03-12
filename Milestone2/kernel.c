void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string); //
void readString(char *string); //
void readSector(char *buffer, int sector); //
void writeSector(char *buffer, int sector); //
void readFile(char *buffer, char *path, int *result, char parentIndex);
void clear(char *buffer, int length); // Fungsi untuk mengisi buffer dengan 0 //
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void executeProgram(char *path, int segment, int *result, char parentIndex);
int doesFileNameExist(char* buffer, char* filename);
int mod(int bil1, int bil2); //
int div(int bil1, int bil2); //
void logo();//
void enter();

void lengthString(char *stringInput, int *length_String);
void countChar(char *stringInput, char c, int *count_Char);
void isStringSame (char *stringInput1, char *stringInput2, int *output); //output bernilai 0/1 0 jika beda 1 jika sama
void copyString (char *stringInput, char *stringOutput, int idxMulai, int panjangKopian);
void takeFileNameFromPath (char *path, char *directoryPath, char *fileName);
void makePathtoMatriks (char *path, char c, char matriks[64][14]);
void isSameSector(char *sector, char start, char checker[14], char *index, char *output) ;
void searchDirectoryParent(char *dirParent, char *pathParent, char *index, char *output, char idxParent);
void searchFile(char *dirsOrFile, char *path, char *index, char *success, char parentIndex);

char input[100];
char buff[1000];
char buffs[10000];
char pengguna[1000];
char c[10] =  {'a','b','c','d','e','f','g','h','i','j'};
int succ = 0;
int sec = 10;
char buffer[8192];

int main() {
    makeInterrupt21();
    logo();
    enter();
    enter();
    printString("execute");
    enter();
    enter();
    // interrupt(0x21, 0x4, buffer, "key.txt", &succ);
    // if (succ) {
    //     interrupt(0x21,0x0, "Kunci : ", 0, 0);
	//  	interrupt(0x21,0x0, buffer, 0, 0);
    // }
    // else {
    //     interrupt(0x21, 0x6, "milestone1", 0x2000, &succ);
    // }
    printString("Writing file\n\r");
    writeFile(c,"c.txt",&succ,0xFF);
    if(succ == 1) printString("Write File success\n\r");
    else if(succ == -1) printString("File sudah ada\n\r");
    else if(succ == -2) printString("Tidak cukup entri di files\n\r");
    else if(succ == -3) printString("Tidak cukup sektor kosong\n\r");
    else if(succ == -4) printString("Folder tidak valid\n\r");
    printString("Reading file\n\r");
    readFile(buffer,"c.txt",&succ,0xFF);
    if(succ == 0) {
        printString(buffer);
        enter();
    }
    else if(succ == -1) printString("Read File failed\n\r");
    // readFile(buffer,"milestone1",&succ,0xFF);
    // if(succ == 0) {
    //     printString("Read File berhasil\n\r");
    //     printString(buffer);
    //     enter();
    // }
    // else {
    //     printString("Read File gagal!\n\r");
    // }
  while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX) {
   char AL, AH;
   AL = (char) (AX);
   AH = (char) (AX >> 8);
   switch (AL) {
      case 0x00:
         printString(BX);
         break;
      case 0x01:
         readString(BX);
         break;
      case 0x02:
         readSector(BX, CX);
         break;
      case 0x03:
         writeSector(BX, CX);
         break;
      case 0x04:
         readFile(BX, CX, DX, AH);
         break;
      case 0x05:
         writeFile(BX, CX, DX, AH);
         break;
      case 0x06:
         executeProgram(BX, CX, DX, AH);
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
        if (huruf == '\r') {
			string[i] = 0x00;
            flag=1;
		}
        else {
            //Jika huruf masukkan adalah backspac
            if (huruf == 0x8 ) {
                    interrupt(0x10, (0xe<<8)+0x8, 0, 0, 0); //backspace 1x
                    interrupt(0x10, (0xe<<8)+0x0, 0, 0, 0); //jadiin nul
                    interrupt(0x10, (0xe<<8)+0x8, 0, 0, 0); //backspace 1x
                if (i>=1) {
                    string[i] = 0x00;
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
void readFile(char *buffer, char *path, int *result, char parentIndex)
{
    char dirsOrFile[1024]; //16*64
    char sectors[512]; //16*32
    int idx; int success; int i=0;

    // read sector
    readSector(dirsOrFile,0x101);
    readSector(dirsOrFile+512,0x102);
    readSector(sectors,0x103);
    //PROSES PENGECHECKAN dir folder DAN file
    searchFile(dirsOrFile,path,&idx,&success,parentIndex);
    if (success==0) {
        *result = -1; //File tidak ditemukan (readFile)
    }
    else {
        *result = 0; //Berhasil;
        //Proses membaca isi sector
        while (i<16) {
            if (sectors[(idx*16)+i]==0x00) {
                break; //Proses membca selesai
            }
            else {
                //Pembacaan dilakukan ke buffer yang terletak pada buffer +(i*512)
                //Yang dibaca adalah sector idx*16 == tabel sector
                // yang ditambah i untuk membaca character 0..15
                readSector(buffer+i*512,sectors[idx*16+i]);
            }
            i++;
        }
    } 
}

void clear(char *buffer, int length) {
    int i;
    for(i = 0; i < length; i++) {
        buffer[i] = 0x0;
    } 
}
void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
    char file1[1024], map[512], sector_[512];
    char filename[14];
    char directory_path[512];
    char sub_buffer[512];
    char idx_parent, dir_valid, idx_file, file_exist, idx_sector;
    int i, j, k, l, m = 0, count_emptymap = 0, buffer_length = 0, filename_length = 0, count_neededsector, file_index;
    int available_entries;

    readSector(map,0x100);
    readSector(file1,0x101);
    readSector(file1+512,0x102);
    readSector(sector_,0x103);

    //Cek jumlah map kosong
    for(i = 0; i < 512; i++) {
        if(map[i] == 0x00) {
            count_emptymap++;
        }
    }

    lengthString(buffer, &buffer_length);
    count_neededsector = div(buffer_length,512);
    if(count_neededsector <= 0) {
        count_neededsector = 1;
    }

    if(count_emptymap >= count_neededsector) {
        available_entries = 0;
        //Cek dari sector file
        for(j = 0; j < 1024; j += 16) {
            if(file1[j] == 0x00 && file1[j+1] == 0x00 && file1[j+2] == 0x00) {
                available_entries = 1;
                file_index = div(j,16);
                break;
            }
        }
        if(available_entries) {
            takeFileNameFromPath(path,directory_path,filename);
            searchDirectoryParent(file1,directory_path,&idx_parent,&dir_valid,parentIndex);
            if(dir_valid) {
                searchFile(file1,path,&idx_file,&file_exist,parentIndex);
                if(!file_exist) {
                    //isi P dengan index dirParent
                    file1[file_index*16] = idx_parent;
                    lengthString(filename,&filename_length);
                    //Cari index sector
                    for(idx_sector = 0; idx_sector < 32; idx_sector++) {
                        if(sector_[idx_sector*16] == 0x00) {
                            break;
                        }
                    }
                    file1[file_index*16+1] = idx_sector;
                    //Isi nama file di files dengan 00 dulu
                    for(k = 0; k < 14; k++) {
                        file1[file_index*16+2+k] = 0x00;
                    }
                    for(k = 0; k < filename_length; k++) {
                        file1[file_index*16+2+k] = filename[k];
                    }
                    //Cari sector kosong
                    for(l = 0; l < count_neededsector; l++) {
                        while(map[m] != 0x00 && m < 256) {
                            m++;
                        }
                        //Tandai di map
                        map[m] == 0xFF;
                        //Isi di sector
                        sector_[idx_sector*16+l] = m;
                        //Pindahkan ke sub_buffer sebesar 512
                        clear(sub_buffer,512);
                        copyString(buffer, sub_buffer, l*512, 512);
                        //Menuliskan ke sector m
                        writeSector(sub_buffer, m);
                    }

                    writeSector(map,0x100);
                    writeSector(file1,0x101);
                    writeSector(file1+512,0x102);
                    writeSector(sector_,0x103);
                    *sectors = 1;
                }
                else {
                    *sectors = -1; // File sudah ada
                }
            }
            else {
                *sectors = -4; // Folder tidak valid
            }
            
        }
        else {
            *sectors = -2; // Tidak cukup entri di files
        }
    }
    else {
        *sectors = -3; // Tidak cukup sektor kosong
    }

}

void executeProgram(char *path, int segment, int *result, char parentIndex) {
	char _buffer[16*512];
    int i;
	readFile(_buffer, path, result, parentIndex);
	if (*result == 1)
	{
		for (i = 0; i < 16*512; i++)
		{
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
printString(" -------------- ------------------ "); enter();
}


void lengthString(char *stringInput, int *length_String) {
    int panjang = 0;
    while (stringInput[panjang] != 0x00) {
        panjang++;
    }
    *length_String = panjang;
}

void countChar(char *stringInput, char c, int *count_Char) {
    int jumlah = 0;
    int it= 0;
    while (stringInput[it]!= 0x00) {
        if (stringInput[it]!=c){
            it++;
        }
        else {
            jumlah++;
            it++;
        }
    }
    *count_Char= jumlah;
}

void isStringSame (char *stringInput1, char *stringInput2, int *output) //output bernilai 0/1 0 jika beda 1 jika sama
{
    int it =0;
    int bol = 1; //true
    while (stringInput1[it]!=0x00 && stringInput2[it]!=0x00) {
        if (stringInput1[it] == stringInput2[it]) {
            it++;
            bol = bol*1;
        }
        else {
            bol= bol*0;
            break;
        }
    }
    if (bol==0) {
        *output = 0;
    }
    else {
        if (stringInput1[it]!=0x00 || stringInput2[it]!=0x00) {
            *output = 0;
        }
        else {
            *output =1;
        }
    }
}

void copyString (char *stringInput, char *stringOutput, int idxMulai, int panjangKopian) {
    int li, lo;
    int it =0;

	//hitung panjang string masukkan dan coppied output
    lengthString(stringInput,&li);
    lengthString(stringOutput, &lo);

	//Bersihkan string output
    clear(stringOutput, lo);
    clear(stringOutput,li);
    clear(stringOutput,panjangKopian);

    if(idxMulai < li) {
        while (panjangKopian>0 && stringInput[idxMulai]!=0x00) {
            stringOutput[it]=stringInput[idxMulai];
            it++; 
            idxMulai++;
            panjangKopian--;
        }
    }
}
void takeFileNameFromPath (char *path, char *directoryPath, char *fileName) {
    int lp, lpfix;
    int flag =0;
    lengthString(path,&lpfix);
    lp = lpfix;
    while(flag==0 && lp>=0) {
        lp--;
        if (path[lp]=='/') {
            flag =1;
        }
    }
    copyString(path,fileName,lp+1,lpfix-lp-1); //semoga bener
    copyString(path,directoryPath,0,lp);
}

void makePathtoMatriks (char *path, char c, char matriks[64][14]) {
    int it=0; int j=0;
    
    //Inisialisasiawal
    //clear dengan pnjnng nama files 14 sektor
    clear(matriks[it],14);
    while(1) {
        if (path[it] == 0x00) {
            break;
        }
        else if (path[it]!= c) {
            matriks[it][j] = path[it];
            j++;
        }
        else {
            matriks[it][j] = path[it];
            it++;j=0;
            clear(matriks[it],14);
        }
        it++;
    }
    matriks[it][j]= 0x00;
}


void isSameSector(char *sector, char start, char checker[14], char *index, char *output) {
   //sector adalah tabel dir/filenya
   //start nya adalah indexParentnya
   //checker adalah nama directory yang ingin kita check apakah sama atau tidak
   //Keluarannya adalah index tempat ditemukan dan output 1/0

    int i; //INISIALISASI HARUS DI AWAL!1!1
    int it = 0;
    int bol = 0;
    int jumlahSektor = 16;

    //Inisialisasi blank space
    char blankspace[14];
    clear(blankspace,14);


    //Melakukan iterasi di sepnjang tabelfile/dir kita
    //Tabel dir memiliki index dari  0 hingga 63
    while (it<=63) {
        //Check apakah P nya atau it*16 di table dir sama dengan nilai indexParent
        if(sector[it*jumlahSektor] ==start) {

            //Jika nilai index sama, maka sekarang kita check
            //Apakah "namaFolder" nya sama atau tidak
            clear(blankspace,14);

            //Kita isi blankspace dengan namaFolder yg diambil dari sector / tabel dir
            for(i=0;i<14;i++) {
                blankspace[i] = sector[it*jumlahSektor + (i+2)];
            }
            isStringSame(blankspace,checker,output);
            break;
        }
        it++;
    }
    if(*output) {
        *index = it-1;
    }
}


void searchDirectoryParent(char *dirParent, char *pathParent, char *index, char *output, char idxParent) {
    //Akan melakukan seaching pada directory Parentnya
    //Apakah directory parentnya valid atau tidak
    //Jika valid outputnya 1, jika tidak, outputnya 0

    //matriks akan berisi matriks dari path parent yang sudah dipisah pisah
    char matriks[64][14]; //Ukuran baris kita 64, dan panjang nama file (kolom) 14
    //FileName adalah matriks yang diakses setiap baris
    char fileName[14];
    int it =0; int cslas; int bol = 1;
    countChar(pathParent,'/',&cslas);

    if (pathParent[it]!=0x00) {
        makePathtoMatriks(pathParent,'/',matriks);
        while (bol==1 && it<=cslas) {
            
            //CCC (clear copy cari)
            clear(fileName,14);
            copyString(matriks[it], fileName, 0, 14);
            isSameSector(dirParent, idxParent, fileName, index, output);
            it++;
            bol = *output;
        }
        *output = bol;
    }
    else {
        *output = 1;
        *index = idxParent;
    }
}

void searchFile(char *dirsOrFile, char *path, char *index, char *success, char parentIndex) {
    char pathParent[960]; //64 baris * 14+1 kolom, +1 kolom buat 0x00?,  path Parent without file
    char file[14];
    int idx;

    takeFileNameFromPath(path,pathParent,file);

    //Check apakah Parent atau foldernya valid atau tidak
    searchDirectoryParent(dirsOrFile,pathParent,&idx,success,parentIndex);
    if (*success==0) {
        //Do nothing 
    }
    else {
        //Check apakah filenya valid atau tidak
        isSameSector(dirsOrFile,idx,file,index,success);    
    }
}