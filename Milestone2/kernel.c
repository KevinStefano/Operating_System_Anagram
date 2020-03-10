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

void lengthString(char *stringInput, int *length_String);
void countChar(char *stringInput, char c, int *count_Char);
void isStringSame (char *stringInput1, char *stringInput2, int *output); //output bernilai 0/1 0 jika beda 1 jika sama
void copyString (char *stringInput, char *stringOutput, int idxMulai, int panjangKopian);
void takeFileNameFromPath (char *path, char *directoryPath, char *fileName);
void makePathtoMatriks (char *path, char c, char matriks[64][14]);
void isSameSector(char *sector, char start, char end[14], char *index, char *output) ;
void searchDirectoryParent(char *dirParent, char *pathParent, char *index, char *output, char idxParent);

char input[100];
char buff[1000];
char buffs[10000];
char pengguna[1000];
char c[5] =  {'a','b','c','d'};
int succ = 0;
int sec = 10;
char buffer[10240];

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
        interrupt(0x21,0x0, "Kunci : ", 0, 0);
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
        if (huruf == '\r') {
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
    //Baca sektor dir
    readSector(dir, 2);

    //Mengecek nama file
    //looping di tiap baris dari 0 hingga 512
	//1 baris dir punya 32 kolom, jadi dichecknya di awal-awalnya aja sehingga iterasinya +32
    for (i=0;i<512;i+=32) {
        flag =1;
        //PROSES CHECK NAMA FILE
        //Di tiap baris yang mana punya 32 kolom
		//12 kolom pertama adalah kolom file name
		//Kita check kolom file name dari indeks kolom 0 hingga 12
        for (k=0; k<12;k++) {
            if (filename[k]!='\0') {
                //check masukkan fileName dari user
				//Jika filename user sama dengan fileName dir, maka True
				//cara ngechecknya dengan mengiterasi fileeName[0..12] dan
				//mencocokannya dengan dir[0..12] tiap baris jadi dir[K..K+12]
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
            break;
        }
    }

    if (flag!=1) {
        *success = 0;
        enter();
        return;
    }
    else {
        //Jika sudah menemukan nama filenya di dir[0..12]
		//Maka langkah selanjutnya adalah membaca isi filenya
		//Isi filenya terdapat di baris yang sama dengan nama file, namun di kolom setelah nama file
		//Jika kolom namafile [0..12]
		//Maka kolom alamat isi dari filename [12..32] //20 kolom setelahnya
        for(j = 0; j < 20; j++) {
            //Baca isi dari alamatnya di sector di kolom 512 setelahnya
            //Jika alamat isifilename di [12..32]
            //Maka isi alamat isifilename di [12*512..32*512]
            //readSector(A,B) adalah A berisi isi file sebenernya yang direference oleh B
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
        
		// ^ <-------------------------32 (dir)-----------------------------------> 
		// | <---------12 (Nama file)-----><------20(Alamat isi file) ------------>
		// |
		// |
		// v  (ukuran kebawah untuk dir adalah 512)
		// ^
		// | //Ini tempat isi file yang sebenernya (512+i) (sektor)
		// |  
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

    //PROSES CHECK DIR KOSONG
	//16 BARIS awal di dir berisi nama-nama file yang kita miliki
	//Kita check apakah di 16 baris awal itu ada tempat yang kosong
	//Maka kita melooping dai baris 0 sampai 16, dari kolom 0 sampai 32
    for(dir_index = 0; dir_index < 16; dir_index++) {
		//Jika ada yang 0 maka ada yang kosong yuhu~~~
		//checknya tiap baris*32
        if(dir[dir_index*32] == 0x00) {
            break;
        }
    }

	//Jika TIDAK ada yang kosong di baris 0..16, maka kita tidak bisa menulis file
    if(dir_index>=16) {
        printString("Not able to find an empty directory for the current file.");
        *sectors = -1;
        return;

	//Jika ada yang kosong di baris 0..16, maka kita bisa menulis file
    }
    else {
         //Cek jumlah sektor di map cukup untuk buffer file
         //Kita mengecheck sekarang apakah map yang kita miliki kosong
		//Chek dilakukan apakah i<256 dan map yang ksong sejumlah sectornya
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
            //Jika map yang kosong sejumlah sector yang kita mau, maka kita clear
            clear(dir+dir_index*32,32);

            //PROSES MENULIS NAMA FILE DI DIR
			//Tulis namafile
			//Tulis di dir dengan index idx*32
			//Kemudian looping dari kolom ke 0 sampai kolom ke 12
			for(i = 0; i < 12; i++) {
                if (filename[i] != 0x00) {
                    dir[dir_index*32 + i] = filename[i];
                }
                else {
                    break;
                }
            }

            //Sekarang kita mencari lagi di map yang kosong tadi
            for(j = 0, sector_num =0; j < 512 && sector_num < (*sectors); j++) {
                if (map[j] == 0x00) {
                    //Menandai di map
                    map[j] = 0xFF;

                    //Tandai di dir
                    //dir_Index*32 berisi nama file kita
					//+12  berisi isi dari file kita
					//+sector_num berarrti terdapat di kolom setelah isi dari filename kita
					//karena isi dari filename kita berukuran sector_num
					//Kita isi dengan sembarang yaitu i
                    dir[dir_index*32+12+sector_num ] = j;

                    //PROSES MENULIS DI ISI FILE DI SECTOR (512 sertelah dir)
					//Bersihkan sector yang akan kita pakai sejumlah 512 baris
                    clear(sub_buffer,512);

					//Kita tulis dari baris 0..512
                    for(l = 0; l < 512; l++) {

                        //Memindahkan dari buffer ke kita punya sektor
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
printString(" -------------- ----------------- "); enter();
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
            jumlah ++;
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

    //Validasi start
	//Dilakukan hanya jika idxMulai lebih kecil dari panjangKopian
    if (idxMulai >= panjangKopian) {
        //Do nothing
    }
    else {
        if (panjangKopian>0) {
            while (panjangKopian>0 && stringInput[idxMulai]!=0x00) {
                stringOutput[it]=stringInput[idxMulai];
                it++; idxMulai++;
            }
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

//MASI SALAH
void isSameSector(char *sector, char start, char end[14], char *index, char *output) {
   /*
    //Inisialisasi blank space
    char blankspace[14];
    clear(blankspace,14);

    int it = 0;
    int bol = 0;
    int jumlahSektor = 16;
    while (bol==0 && it<64) {
        if(sector[it*jumlahSektor] ==start) {
            clear(blankspace,14);
            for( int i=0;i<15;i++) {
                blankspace[i] = sector[it*jumlahSektor + it+1];
            }
            isStringSame(blankspace,end,output);
            bol= *output;
        }
        it++;
    }
    *output = bol;
    */
}


void searchDirectoryParent(char *dirParent, char *pathParent, char *index, char *output, char idxParent) {
    char matriks[64][14]; //Ukuran baris kita 16, dan panjang nama file (kolom) 14
    char fileName[14];
    int it =0; int cslas; int bol = 1;
    countChar(pathParent,'/',&cslas);

    if (pathParent[it]==0x00) {
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