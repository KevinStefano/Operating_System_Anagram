

int command(char* input);
void enter();
void checkmatriks(char matriks[64][14], char *curdir, char* dirs, char *succes);
void printString(char *string);
void printMatrikstoPath(char matriks[64][14]);


void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void deleteFile(char *path, int *result, char parentIndex);

void searchIndexbyFileName (char *dir, char* stringInput, char idxParent, char* IdxChildoutput);
void searchFileNameParentbyIndexFromChild(char *dirs, int* idx, char* stringOutput);

int IsStringSameBol(char *stringInput1, char *stringInput2); //output bernilai 0/1 0 jika beda 1 jika sama
void isStringSame (char *stringInput1, char *stringInput2, int *output); //output bernilai 0/1 0 jika beda 1 jika sama
void readSector(char *buffer, int sector) ;
void writeSector(char *buffer, int sector);
void searchFile(char *dirsOrFile, char *path, char *index, char *success, char parentIndex);
void clear(char *buffer, int length);
void takeFileNameFromPath (char *path, char *directoryPath, char *fileName);
void lengthString(char *stringInput, int *length_String);
void copyString (char *stringInput, char *stringOutput, int idxMulai, int panjangKopian);
void searchDirectoryParent(char *dirParent, char *pathParent, char *index, char *output, char idxParent);
void countChar(char *stringInput, char c, int *count_Char);
void makePathtoMatriks (char *path, char c, char matriks[64][14]);
void isSameSector(char *sector, char start, char checker[14], char *index, char *output);


void createFolder(char* path, int* success, char parentIndex);
void deleteFolder(char *path, int *success, char parentIndex);
void listContent(char currDir);

void putStr(char curdir, char argc, char argv[64][128]);
void getCurdir(char *curdir);
void getArgc(char *argc);
void getArgv(char idx, char *argv); 


int mod(int bil1, int bil2);
int div(int bil1, int bil2);
int main() {
          
    interrupt(0x21, 0x0, "Welcome to ANAGRAM SHELL 1.0 ",0,0);
    enter();

    while (1) {

        char masukkan[200];
        int type_masukkan;
        int sumKataSetelahSpasi;
        int kataSetelahSpasi;
        char dirsOrFile[1024];
        char path[14];
        char fileName[14];
        int output;
        int i=0;
        int j=0;
        int l;
        int idxoutput;
        int out;
        int success = 0;
        char curdir = 0xFF;
        char argc;
        char argv[64][14];

        interrupt(0x21,0x02,dirsOrFile,0x101,0);
        interrupt(0x21,0x02,dirsOrFile+512,0x102,0);
        printString("Anagram/");
        printString("> ");
        interrupt(0x21, 0x1, &masukkan,0,0);

        type_masukkan = command(masukkan);
        countChar(masukkan,0x20,&sumKataSetelahSpasi);
        makePathtoMatriks(masukkan, 0x20, argv);

        //Ambil element kedua matriks
        clear(path,14);
        while(j<14) {
            path[j] = argv[1][j];
            j++;
        }

        //Inisialisasi arc, curdir
        interrupt(0x21, 0x08 &curdir, 0, 0);
        argc = sumKataSetelahSpasi;
        
        if (type_masukkan == 111) { //cd
            if (sumKataSetelahSpasi>=2) {
                interrupt(0x21, 0x00, "Masukkan salah", 0,0);
                enter();
            }
            else if (sumKataSetelahSpasi==0) {
                curdir = 0xFF;
                putStr(curdir,0,0);
            }
            else {
                isStringSame(path, "..",&out);
                if (argc ==1 && IsStringSameBol(argv[1],"..")) {
                    if (curdir == 0xFF) {
                        interrupt(0x21, 0x00, "Stay on level",0,0);
                        enter();
                    }
                    else {
                        interrupt(0x21, 0x00, "Back 1 level ",0,0);
                        curdir = dirsOrFile[curdir * 16];
                        putStr(curdir, 0, 0);
                        enter();
                        }  
                    }
                
                else {
                    //KASUS cd a/b/c/d
                        searchDirectoryParent(dirsOrFile,argv[1], &idxoutput, &out, curdir);
                        if (out==0) {
                            interrupt(0x21, 0x00, "Gagal... ",0,0);
                            enter();
                        }
                        else {
                            interrupt(0x21, 0x00, "Go to ",0,0);
                            printString(argv[1]);
                            enter();
                            curdir = idxoutput;
                            putStr(curdir,0,0);
                        }
                }
            }
        }
        
        else if(type_masukkan == 112) { // ls

            putStr(curdir, argc, argv + 1);
            listContent(curdir);
            //interrupt(0x21, curdir << 8 | 0x6, "ls", 0x2000, &out);
        }
        else if (type_masukkan == 113) {
            clear(fileName,14);
            i=2;
            j=0;
            while(masukkan[i]!=0x00 &&j<14) {
                fileName[j] = masukkan[i];
                i++;
                j++;
            }
            fileName[j]=0x00;
            if (fileName[0]!=0x00) {
                out=0;
                interrupt(0x21, curdir << 8 | 0x6, &fileName, 0x2000, &out);
                enter();
                if (out==1) {
                    printString("Berhasil execute");
                    enter();
                }
                else {
                    printString("Gagal execute");
                    enter();
                }
            }
        }

        else if(type_masukkan == 114) { // rm 
           if(argv[1][0] == 0x00) {
                interrupt(0x21,0x00,"Paramter rm minimal 1\n\r",0,0);
            }
            else { 
                interrupt(0x21, curdir << 8 | 0x23, argv[1], &out, 0);
                interrupt(0x21, curdir << 8 | 0x22, argv[1], &out, 0);
                //printString("Berhasil"); enter();
            }

        }
        else if(type_masukkan == 115) { // mkdir
            if(argv[1][0] == 0x00) {
                interrupt(0x21,0x00,"Nama folder tidak ada untuk mkdir\n\r",0,0);
            }
            else {
                
            putStr(curdir, argc, argv + 1);
            createFolder(argv[1],&out,curdir);
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
        if (input[it]==0x20) { // space
            break; 
        }
        it++;
    }

        if (input[it]==0x20) {
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

        if (masukkan[0]==0x2E && masukkan[1]=='/') {
            bol=0;
            return 113;
        }
        isStringSame(masukkan, "rm", &bol);
        if (bol==1) {
            bol=0;
            return 114;
        }
        isStringSame(masukkan, "mkdir", &bol);
        if(bol==1) {
            bol = 0;
            return 115;
        }
}

void printMatrikstoPath(char matriks[64][14]) {
    int i=0;
    int j=0;
    while (i<64) {
        printString(matriks[i]);
        if (matriks[i]==0x00) {
            printString("/");
        }
        i++;
    }
}

void checkmatriks(char matriks[64][14], char *curdir, char* dirs, char *succes) {
    // cd a/b/c
    //c curdir

    int fileName[14];
    char curdirs;
    char idxP;
    char idx;
    int output;
    int out;
    int i =0;
    int j,k;
    int it=0;
    int idxParent;
    int simpanIdx;
    int bol =0;
    *succes =1;

    curdirs = *curdir;
    while (matriks[i]!=0x00 && bol==0) {
        out =0;
        isStringSame(matriks[i],"..",&out); 
        //Jika sama dengan ".."
        if (out==1){
            i++;
        }
        //Jika beda dengan ".."
        else {
            idx =0x00;
            searchIndexbyFileName(dirs,matriks[i],curdirs,&idx);
           
            simpanIdx=idx;
            //Pengecheckan dengan keluaran idx
            //Jika nilai Idx>0 atau matriks nilainya sama dengan parent curdirs
            if (!IsStringSameBol(&idx,'0'+(-1))&&!IsStringSameBol(&idx,'0'+(-2))) { 
                
                curdirs = simpanIdx;
                //Jika curdir skrg bukan file
                if (dirs[simpanIdx]==0x00 && dirs[simpanIdx+1]==0x00) {
                    interrupt(0x21, 0x00,"Folders/file tidak ada",0,0);
                    enter();
                    *succes =0;
                    break;
                    
                }
                else if(dirs[simpanIdx+1]!=0xFF) {
                    i++;
                }
                //Jika curdir sekarang adalah file
                else {
                    //Jika masih ada next di matriksnya gagal
                    if (matriks[i+1]!=0x00) {
                        *succes = 0;
                        break; //Program gawgal
                    }
                    else {
                        *succes = 1;
                        break;
                    }
                }
            }
            //jika nilai matriks i tidak sama dengan nilai parent i curdirs
            else{
                idxP = 0x00;
                while(1) {
                    if (idxP == 0x4F) { //BASIS, jika tidak ditemukan
                        bol=1;
                        *succes = 0;
                        break;
                    }
                    else if (idxP ==curdirs) { //BASIS
                        curdirs = simpanIdx;
                        i++;
                        break;
                    }
                    else if(idxP==0xFF) { //BASIS, jika parent root
                        bol=1;
                        *succes =0;
                        break;
                    }
                    //Jika idx==-2
                    //Jika berbeda dengan parent, artinya ada ".."
                    else {
                        //Search idxParent
                        out=0;
                        for(j=0;j<64;j++) {
                            clear(fileName,14);
                            for (k=0;k<14;k++) {
                                fileName[j] = dirs[j*16+2+k];
                            }
                            isStringSame(fileName,matriks[i],&out);
                            if (out==1) {
                                break;
                            }
                        }

                        if (out==1) {
                            idxP= dirs[j];
                        }
                        else if (out==0) {
                            idxP = 0x4F;
                        }
                    }
                }
            }
        }
    }
    *curdir=curdir;
}

void enter() {
    interrupt(0x10, 0xe*256+'\r',0,0,0);
    interrupt(0x10, 0xe*256+'\n',0,0,0);
}


void printString(char *string){
    int i = 0;
    while(string[i] != '\0'){
        interrupt(0x10, (0xe<<8)+string[i], 0, 0, 0);
        i++;
    }
}


void readFile(char *buffer, char *path, int *result, char parentIndex)
{
    interrupt(0x21, parentIndex << 8 || 0x04, *buffer, *path, *result);
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
   interrupt(0x21, parentIndex << 8 || 0x04, *buffer, *path, *sectors);
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

void createFolder(char* path, int* success, char parentIndex) {
    interrupt(0x21, parentIndex << 8 || 0x21, *path, *success, 0);
}

void deleteFolder(char *path, int *success, char parentIndex){
    interrupt(0x21, parentIndex << 8 || 0x23, *path, *success, 0);
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
