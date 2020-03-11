
void copyString (char *stringInput, char *stringOutput, int idxMulai, int panjangKopian);
void isStringSame (char *stringInput1, char *stringInput2, int *output); //output bernilai 0/1 0 jika beda 1 jika sama
void lengthString(char *stringInput, int *length_String);
int command(char* input);
void countChar(char *stringInput, char c, int *count_Char);
void makePathtoMatriks (char *path, char c, char matriks[64][14]);
void enter();
void searchDirectoryParent(char *dirParent, char *pathParent, char *index, char *output, char idxParent);
void isSameSector(char *sector, char start, char checker[14], char *index, char *output);
void takeFileNameFromPath (char *path, char *directoryPath, char *fileName);
void clear(char *buffer, int length);
void searchParent(char *dirparent, char *stringInput, int *output);
void searchFileNamebyIndex(int idx, char* stringOutput);
void searchIndexbyFileName (char *dir, char* stringInput, int idxParent, int* Idxoutput);

int main() {
    while (1) {
        char masukkan[200];
        int type_masukkan;
        int sumKataSetelahSpasi;
        int kataSetelahSpasi;
        char matriks[64][200];
        char nowValue[64][200];
        char dirsOrFile[1024];
        char fileName[14];
        char fileNameOutput[14];
        int output;
        int curdir = 0xFF;
        int i=0;
        int l;
        int idxoutput;
        
        
        interrupt(0x21, 0x0, "Anagram > ",0,0);
        interrupt(0x21, 0x1, &masukkan,0,0);

        type_masukkan = command(masukkan);
        countChar(masukkan,0x20,&sumKataSetelahSpasi);
        makePathtoMatriks(masukkan, 0x20, matriks);

        if (type_masukkan == 111) { //cd 
            if (sumKataSetelahSpasi>=2) {
                interrupt(0x21, 0x00, "Masukkan salah", 0,0);
                enter();
            }
            else if (sumKataSetelahSpasi==0) {
                //Do nothing :(
            }
            else {
                if (isStringSame(matriks[1], "..")) {
                    if (curdir == 0xFF) {
                        interrupt(0x21, 0x00, "Stay on level",0,0);
                    }
                    else {
                        clear(fileName,14);
                        clear(fileNameOutput,14);
                        searchFileNamebyIndex(curdir,fileName);
                        searchParent(dirsOrFile,fileName,&output);
                        if(output==-2) {
                            interrupt(0x21, 0x00, "Current directory wrong",0,0);
                            enter();
                        }
                        else if (output==-1) {
                            interrupt(0x21, 0x00, "Stay on level",0,0);
                            enter();
                        }
                        else {
                            curdir = output;
                            searchFileNamebyIndex(output,fileNameOutput);
                            interrupt(0x21, 0x00, "Back 1 level to ",0,0);
                            interrupt(0x21, 0x00, fileNameOutput, 0,0);
                            enter();
                        }     
                    }
                }
                else {
                    //Kasus cd a maka matriks[2]==0
                    if (matriks[2]==0x00) {
                        clear(fileNameOutput,14);
                        searchIndexbyFileName(dirsOrFile,matriks[1],curdir,&idxoutput);
                        if (idxoutput>=0) {    
                            curdir = idxoutput;
                            interrupt(0x21, 0x00, "Go to ",0,0);
                            searchFileNamebyIndex(curdir,fileNameOutput);
                            interrupt(0x21, 0x00,fileNameOutput,0,0);
                            enter();
                        }
                        else {
                            interrupt(0x21, 0x00, "Masukkan cd dir salah",0,0);
                            enter();
                        }
                    }

                    //KASUS cd a/b/c/d
                        //Pengecheckan cdir/a/b/c/d




                      //Ambil element terakhir matriks
                        while(matriks[i]!=0x00) {
                            i++;
                        }
                        i--;
                        
                        clear(fileName,14);
                        //Akses elemen paling terakhir (b)
                        l=0;
                        while (l<14) {
                            fileName[l] =matriks[i][l];
                            l++;
                        }
                        output = 0;
                        searchParent(dirsOrFile, fileName, &output);
                        if (output>=0) {
                            curdir = output;
                            clear(fileName,14);
                            interrupt(0x21, 0x00, "Go to ",0,0);
                            searchFileNamebyIndex(curdir,fileName);
                            interrupt(0x21, 0x00,fileName,0,0);
                            enter();

                        }
                }
            }
        }
    }
}

int command(char* input) {

    char masukkan[50];
    int lengthStringSebelomSpace;
    int bol;
    int it=0;
    
    //Pencarian lengthstringSebelom space OR sebelum /
    while (input[it]!=0x00) {
        if (input[it]==0x2F) { // /
            break;
        }
        if (input[it]==0x20) { // space
            break; 
        }
        it++;

        if (input[it]==0x20) {
            lengthStringSebelomSpace = it;
        }
        else if (input[it]==0x2F && input[it-1]==0x2E) { 
            lengthStringSebelomSpace = it;
        }
        else {
            lengthString(input,&lengthStringSebelomSpace);
        }

        copyString(input,masukkan,0,lengthStringSebelomSpace);

        //Mencocokkan dengan cd
        isStringSame(masukkan,"cd",&bol);
        if (bol==1) {
            bol=0;
            return 111;
        }
        isStringSame(masukkan,"ls",&bol);
        if (bol==1) {
            bol=0;
            return 112;
        }
        isStringSame(masukkan,"./",&bol);
        if (bol==1) {
            bol=0;
            return 113;
        }
        isStringSame(masukkan, "echo", &bol);
        if (bol==1) {
            bol=0;
            return 114;
        }
    }
}

void lengthString(char *stringInput, int *length_String) {
    int panjang = 0;
    while (stringInput[panjang] != 0x00) {
        panjang++;
    }
    *length_String = panjang;
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
    if (idxMulai >= li) {
        //Do nothing
    }
    else {
        if (panjangKopian>0) {
            while (panjangKopian>0 && stringInput[idxMulai]!=0x00) {
                stringOutput[it]=stringInput[idxMulai];
                it++; idxMulai++; panjangKopian--;
            }
        }
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
            jumlah ++;
            it++;
        }
    }
    *count_Char= jumlah;
}

void makePathtoMatriks (char *path, char c, char matriks[64][200]) {
    int it=0; int j=0;
    
    //Inisialisasiawal
    //clear dengan pnjnng nama files 14 sektor
    clear(matriks[it],200);
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
            clear(matriks[it],200);
        }
        it++;
    }
    matriks[it][j]= 0x00;
}

void enter() {
    interrupt(0x10, 0xe*256+'\r',0,0,0);
    interrupt(0x10, 0xe*256+'\n',0,0,0);
}

void searchParent(char *dirparent, char *stringInput, int *output) {
    //Mengembalikan nilai output
    //-2 jika parent tidak ada
    //-1 jika parent root
    //dan Idx ditemukannya posisi now parent
    int i =0;
    int k=0;
    int l;
    int outputl;
    char fileName[14];
    while (i<64 && k<16) {
        if (dirparent[i]!=0x00) { //Kayanya masi salah
            clear(fileName,14);
            l=0;
            while (l<14) {
                fileName[l] = dirparent[i*16+2+l];
                l++;
            }
            isStringSame(fileName,stringInput,&outputl);
            if (outputl==1) {
                break;
            }   
        }
        i++;
    }

    //Jika StringInputan tidak ada di dir
    if (outputl ==-2) {
        *output =-2;
    }
    //Jika StringInput ada di dir
    else if (outputl==1) {
        //Tapi parentnya root
        if (dirparent[i*16]==0xFF) {
            *output = -1;
        }
        //Jika parent nya tidak root
        else {
            *output = dirparent[i*16];
        }
    }
}


void searchFileNamebyIndex(int idx, char* stringOutput) {
    int l=0;
    char fileNameOutput[14];
    clear(fileNameOutput,14);
    while (l<14) {
        fileNameOutput[l] = dirparent[dirparent[idx*16]+2+l];
        l++;
    }
    copyString(fileNameOutput,stringOutput,0,14);
}

void searchIndexbyFileName (char *dir, char* stringInput, int idxParent, int* Idxoutput) {
    int l=0;
    int j=0;
    int output;
    char fileNameOutput[14];
    while (j<64) {
        while (l<14) {
            clear(fileNameOutput,14);
            fileNameOutput[l] = dir[j*16+2+l];
            l++;
        }
        isStringSame(stringInput,fileNameOutput,&output);
        if (output==1) {
            if (dir[j*16]==idxParent)
            break;
        }
    }
    if (output==1 && (dir[j*16]==idxParent)) {
        *Idxoutput = j*16;
    }
    else {
        *Idxoutput = -1;
    }
}

void searchDirectoryParent(char *dirParent, char *pathParent, char *index, char *output, char idxParent) {
    //Akan melakukan seaching pada directory Parentnya
    //Apakah directory parentnya valid atau tidak
    //Jika valid outputnya 1, jika tidak, outputnya 0

    //matriks akan berisi matriks dari path parent yang sudah dipisah pisah
    char matriks[64][14]; //Ukuran baris kita 16, dan panjang nama file (kolom) 14
    //FileName adalah matriks yang diakses setiap baris
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
    *index = it-1;
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

void clear(char *buffer, int length) {
    int i;
    for(i = 0; i < length; i++) {
        buffer[i] = 0x0;
    } 
}