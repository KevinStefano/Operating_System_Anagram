//LIBRARY FILE I/O

#ifndef __LIBTEST_H
#define __LIBTEST_H

void moveFile(char* pathasal, char* pathtujuan, int* result, char parentIndex) {
    interrupt(0x21,parentIndex << 8 | 0x26, *pathtujuan+1, *pathtujuan+2, &result);
}

void copyFile(char* pathasal, char* pathtujuan, int* result, char parentIndex) {
    interrupt(0x21,parentIndex << 8 | 0x25, *pathtujuan+1, *pathtujuan+2, &result);
}

void readFile(char *buffer, char *path, int *result, char parentIndex)
{
    interrupt(0x21, parentIndex << 8 | 0x04, *buffer, *path, *result);
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
   interrupt(0x21, parentIndex << 8 | 0x04, *buffer, *path, *sectors);
}

void deleteFile(char *path, int *result, char parentIndex) {

    char success;
    int idx;
    char dirsOrFile[1024];
    char maps[512];
    char sectors[512];
    int i = 0;
    
    //Inisialisasi awal dengan memasukkan data
    interrupt(0x21,0x02,maps,0x100,0);
    interrupt(0x21,0x02,dirsOrFile,0x101,0);
    interrupt(0x21,0x02,dirsOrFile+512,0x102,0);
	interrupt(0x21, 0x02, sectors, 0x103, 0);
    
    searchFile(dirsOrFile, path, &idx, &success, parentIndex);
	interrupt(0x21, 0x20, idx, 0, 0);
    if (success) {
        
        dirsOrFile[idx*15]= 0x00;
        dirsOrFile[(idx*15)+1]= 0x00;
        dirsOrFile[(idx*15)+2]= 0x00;

        while (i<16) {
            //Hapus map dan sectors
            maps[sectors[idx*16 + i]]=0x00;
            sectors[idx*16+i]=0x00;
            i++;
            if (sectors[idx*16+i]==0x00 || i>=16) {
                break;
            }
        }
        
        writeSector(maps,0x100);
        writeSector(dirsOrFile,0x101);
        writeSector(dirsOrFile+512,0x102);
        writeSector(sectors,0x103);
        //Maka berhasil
        
        *result = 0;
    }
    else {
        
        *result = -1; //Tidak ditemukan
    }
}

void readSector(char *buffer, int sector) 
{
    interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
    interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector,18) + 1, mod(div(sector,18), 2) * 0x100);
}

void searchFile(char *dirsOrFile, char *path, char *index, char *success, char parentIndex) {
    char pathParent[1024]; //64 baris * 14+2 kolom, +1 kolom buat 0x00?,  path Parent without file
    char file[14];
    int idx;
    clear(file,14);
    takeFileNameFromPath(path,pathParent,file);
    //Check apakah Parent atau foldernya valid atau tidak
    searchDirectoryParent(dirsOrFile,pathParent,&idx,success,parentIndex);

    if (*success==0) {
        //Do nothing 
    }
    else if (*success==1){
        //Check apakah filenya valid atau tidak
        isSameSector(dirsOrFile,idx,file,index,success);   
    }
}

void clear(char *buffer, int length) {
    int i;
    for(i = 0; i < length; i++) {
        buffer[i] = 0x0;
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

void lengthString(char *stringInput, int *length_String) {
    int panjang = 0;
    while (stringInput[panjang] != 0x00) {
        panjang++;
    }
    *length_String = panjang;
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

void searchDirectoryParent(char *dirParent, char *pathParent, char *index, char *output, char idxParent) {
    //Akan melakukan seaching pada directory Parentnya
    //Apakah directory parentnya valid atau tidak
    //Jika valid outputnya 1, jika tidak, outputnya 0

    //matriks akan berisi matriks dari path parent yang sudah dipisah pisah
    char matriks[64][14]; //Ukuran baris kita 64, dan panjang nama file (kolom) 14
    //FileName adalah matriks yang diakses setiap baris
    char fileName[14];
    int it =0; int cslas; int bol = 1;

    if (pathParent[it]!=0x00) {

    countChar(pathParent,'/',&cslas);
        makePathtoMatriks(pathParent,'/',matriks);
        *output = bol;
        while (bol==1 && it<=cslas) {
            
            //CCC (clear copy cari)
            clear(fileName,14);
            copyString(matriks[it], fileName, 0, 14);
            isSameSector(dirParent, idxParent, fileName, index, output);
            it++;
            bol = *output;
        }
    }
    else {
        *output = 1;
        *index = idxParent;
    }
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

void makePathtoMatriks (char *path, char c, char matriks[64][14]) {
    int it=0; int j=0; int l=0;
    
    //Inisialisasiawal
    //clear dengan pnjnng nama files 14 sektor
    clear(matriks[l],14);
    while(1) {
        if (path[it] == 0x00) {
            break;
        }
        else if (path[it]!= c) {
            matriks[l][j] = path[it];
            j++;
        }
        else {
            matriks[l][j] = 0x00;
            j=0;l++;
            clear(matriks[l],14);
        }
        it++;
    }
    matriks[l+1][0]= 0x00;
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
    char blankspace[14];
	*output = 0;
	*index = 0;

    //Inisialisasi blank space
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
            if (*output) {
                break;
            }
        }
        it++;
    }
    *index = it+1;
}


void searchFileNameParentbyIndexFromChild(char *dirs, int* idx, char* stringOutput) {
    //ASUMSI child memiliki index parent TERDEFINISI
    //Diberikan idx yang berupa idx dari child
    //Maka kita akan mencari idxParentnya dan menaruh di idx
    //Menaruh nama fileName Parentnya di stringOutput
    int l=0;
    char fileNameOutput[14];
    clear(fileNameOutput,14);
    if (dirs[(*idx)]==0xFF) {
        stringOutput[0] = 'N';
        stringOutput[1] = '0';
        stringOutput[2] = 'N';
        stringOutput[0] = '3';
    }
    else {
        while (l<14) {
                fileNameOutput[l] = dirs[dirs[(*idx)]+2+l];
                l++;
            }
            copyString(fileNameOutput,stringOutput,0,14);
    }
    *idx =dirs[*idx];
    
}

void searchIndexbyFileName (char *dir, char* stringInput, char idxParent, char* IdxChildoutput) {
    //Output
    //Jika didapat filename di dir, idx = indexnya
    //jika tidak didapat di dir, idx =-1
    //Jika 
    int out;
    int l=0;
    int j=0;
    int output=0;
    int flag=0;
    char fileNameOutput[14];


    while (j<64) {
        clear(fileNameOutput,14);
        l=0;
        while (l<14) {
            fileNameOutput[l] = dir[j*16+2+l];
            l++;
        }
        isStringSame(stringInput,fileNameOutput,&output);
        if (output==1) {
            if (IsStringSameBol(dir[j*16],idxParent)) {
                break;
            }
            else {
                flag++;
            }
        }
        j++;
    }
    isStringSame(dir[j*16],idxParent,&out);
    if (output==1 && out==1) {
        *IdxChildoutput = j*16 + '0';
    }
    else if (flag>0) {
        *IdxChildoutput = -2 + '0';
    }
    else if (output==0){

        *IdxChildoutput = -1 + '0';
    }
}


void isStringSame (char *stringInput1, char *stringInput2, int *output) //output bernilai 0/1 0 jika beda 1 jika sama
{
    int it =0;
    int bol = 1; //true
    int ls1, ls2;
    lengthString(stringInput1,&ls1);
    lengthString(stringInput2,&ls2);
    if (ls1==ls2) {
        while (bol ==1 && (stringInput1[it]!=0x00 )) {
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
    else {
        *output = 0;
    }  
}

int IsStringSameBol(char *stringInput1, char *stringInput2) //output bernilai 0/1 0 jika beda 1 jika sama
{
   int it =0;
    int bol = 1; //true
    int ls1, ls2;
    lengthString(stringInput1,&ls1);
    lengthString(stringInput2,&ls2);
    if (ls1==ls2) {
        while (bol ==1 && (stringInput1[it]!=0x00 )) {
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
             return 0;
        }
        else {
            if (stringInput1[it]!=0x00 || stringInput2[it]!=0x00) {
                 return 0;
            }
            else {
                 return 1;
            }
        }
    }
    else {
        return  0;
    }  
}

void printString(char *string){
    int i = 0;
    while(string[i] != '\0'){
        interrupt(0x10, (0xe<<8)+string[i], 0, 0, 0);
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
#endif