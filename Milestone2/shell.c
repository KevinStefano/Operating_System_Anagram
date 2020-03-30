
void copyString (char *stringInput, char *stringOutput, int idxMulai, int panjangKopian);
void isStringSame (char *stringInput1, char *stringInput2, int *output); //output bernilai 0/1 0 jika beda 1 jika sama
void lengthString(char *stringInput, int *length_String);
int command(char* input);
void countChar(char *stringInput, char c, int *count_Char);
void makePathtoMatriks (char *path, char c, char matriks[64][14]);
void enter();
void takeFileNameFromPath (char *path, char *directoryPath, char *fileName);
void clear(char *buffer, int length);
void searchFileNameParentbyIndexFromChild(char *dirs, int* idx, char* stringOutput);
void checkmatriks(char matriks[64][14], char *curdir, char* dirs, char *succes);
void listAll(char* dir, char matriks[64][14], char parentIndex);
void makeDir(char* dir, char matriks[64][14], int* success, char parentIndex);
void searchIndexbyFileName (char *dir, char* stringInput, char idxParent, char* Idxoutput);
void checkmatriks(char matriks[64][14], char *curdir, char* dirs, char *succes);
int mod(int bil1, int bil2);
int div(int bil1, int bil2);
int IsStringSameBol(char *stringInput1, char *stringInput2);
void printString(char *string);
void printMatrikstoPath(char matriks[64][14]);
void pushToMatriks(char matriks[64][14], char *stringInput);
void popMatriks(char matriks[64][14]);

char matrikscurdir[64][14];

int main() {
          
    interrupt(0x21, 0x0, "Welcome to ANAGRAMM SHELL 1.0 ",0,0);
    enter();
    //Inisialisasi matriksCurdir

    while (1) {

        char masukkan[200];
        int type_masukkan;
        int sumKataSetelahSpasi;
        int kataSetelahSpasi;
        char matriks[64][14];
        char matrikspath[64][14];
        char dirsOrFile[1024];
        char path[14];
        char fileName[14];
        int output;
        char curdir = 0xFF;
        char curdirtemporal= 0xFF;
        int i=0;
        int j=0;
        int l;
        int idxoutput;
        int out;
        int success = 0;
        int matriks_length;
        int matriks_path_length;

        interrupt(0x21,0x02,dirsOrFile,0x101,0);
        interrupt(0x21,0x02,dirsOrFile+512,0x102,0);
        interrupt(0x21, 0x0, "Anagram/",0,0);
        
        //printMatrikstoPath(matrikscurdir);
        printString("> ");
        interrupt(0x21, 0x1, &masukkan,0,0);


        type_masukkan = command(masukkan);
        countChar(masukkan,0x20,&sumKataSetelahSpasi);
        makePathtoMatriks(masukkan, 0x20, matriks);
        //pushToMatriks(matrikscurdir,masukkan);

        //Ambil element kedua matriks
        clear(path,14);
        while(j<14) {
            path[j] = matriks[1][j];
            j++;
        }
            
        if (type_masukkan == 111) { //cd
            if (sumKataSetelahSpasi>=2) {
                interrupt(0x21, 0x00, "Masukkan salah", 0,0);
                enter();
            }
            else if (sumKataSetelahSpasi==0) {
                //Do nothing :(
            }
            else {
                isStringSame(path, "..",&out);
                if (out==1) {
                    if (curdir == 0xFF) {
                        interrupt(0x21, 0x00, "Stay on level",0,0);
                        enter();
                    }
                    else {
                        clear(fileName,14);
                        searchFileNameParentbyIndexFromChild(dirsOrFile,&curdirtemporal,fileName);
                        if (curdirtemporal==0xFF) {
                            interrupt(0x21, 0x00, "Stay on level",0,0);
                            enter();
                        }
                        else {
                            curdir = curdirtemporal;
                            interrupt(0x21, 0x00, "Back 1 level to ",0,0);
                            interrupt(0x21, 0x00, fileName, 0,0);
                            enter();
                            //popMatriks(matrikscurdir);
                            
                        }     
                    }
                }
                else {
                    //KASUS cd a/b/c/d
                        //Pbuat path jadi split

                        makePathtoMatriks(path,'/',matrikspath);
                        checkmatriks(matrikspath,&curdirtemporal,dirsOrFile,&out);
                        if (out==0) {
                            interrupt(0x21, 0x00, "Gagal... ",0,0);
                            enter();
                        }
                        else {
                            curdir = curdirtemporal;
                            clear(fileName,14);
                            interrupt(0x21, 0x00, "Go to ",0,0);
                            searchFileNameParentbyIndexFromChild(dirsOrFile,&curdir,fileName);
                            interrupt(0x21, 0x00,fileName,0,0);
                            enter();
                            //pushToMatriks(matrikscurdir,fileName);
                        }
                }
            }
        }
        else if(type_masukkan == 112) { // ls
            listAll(dirsOrFile,matriks,curdir);
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
                    printString("Gagal");
                    enter();
                }
            }
        }
        else if(type_masukkan == 115) { // mkdir
            if(matriks[1][0] == 0x00) {
                interrupt(0x21,0x00,"Nama folder tidak ada\n\r",0,0);
            }
            else {
                makeDir(dirsOrFile,matriks,&success,curdir);
                if(success == -2) {
                    interrupt(0x21,0x00,"Entry tidak cukup\n\r",0,0);
                }
                else if(success == -1) {
                    interrupt(0x21,0x00,"Folder sudah ada\n\r", 0,0);
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
        isStringSame(masukkan, "echo", &bol);
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

void pushToMatriks(char matriks[64][14], char *stringInput) {
    int i = 0;
    int j=0;
    while (matriks[i]!=0x00 && i<64) {
        i++;
    }
    
    //masukkan
    while(stringInput[j]!=0x00) {
        matriks[i][j] ==stringInput[j];
        j++;
    }
}


void popMatriks(char matriks[64][14]) {
    int i=0;
    if (matriks[0] != 0x00) {
        while (matriks[i]!= 0x00) {
            i++;
        }
        clear(matriks[i],14);
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

void lengthString(char *stringInput, int *length_String) {
    int panjang = 0;
    while (stringInput[panjang] != 0x00) {
        panjang++;
    }
    *length_String = panjang;
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

void enter() {
    interrupt(0x10, 0xe*256+'\r',0,0,0);
    interrupt(0x10, 0xe*256+'\n',0,0,0);
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

void listAll(char* dir, char matriks[64][14], char parentIndex) {
    int i, j, idx;
    char foldername[14];
    if(matriks[1][0] == 0x00) {
        for(i = 0; i < 64; i++) {
            if(dir[i*16] == parentIndex) {
                clear(foldername,14);
                for(j = 0; j < 14; j++) {
                    foldername[j] = dir[i*16+2+j];
                }
                interrupt(0x21,0x00,foldername,0,0);
                interrupt(0x21,0x00," ", 0, 0);
            }
        }
        interrupt(0x21,0x00, "\n\r",0,0);
        return;
    }
    for(i = 1; matriks[i][0] != 0x00; i++) {
        interrupt(0x21,0x00,matriks[i],0,0);
        interrupt(0x21,0x00,":\n\r",0,0);
        searchIndexbyFileName(dir,matriks[i],parentIndex,&idx);
        if(idx == -1 || idx == -2) {
            continue;
        }
        for(j = 0; j < 64; i++) {
            if(dir[j*16] == idx) {
                clear(foldername,14);
                for(j = 0; j < 14; j++) {
                    foldername[j] = dir[i*16+2+j];
                }
                interrupt(0x21,0x00,foldername,0,0);
                interrupt(0x21,0x00," ",0,0);
            }
        }
        interrupt(0x21,0x00,"\n\r",0,0);
    }
}

void makeDir(char* dir, char matriks[64][14], int* success, char parentIndex) {
    int  i, j, k, foldername_length;
    char idx;
    *success = 0;
    for(k = 1; matriks[k][0] != 0x00; k++) {
        lengthString(matriks[k],&foldername_length);
        searchIndexbyFileName(dir,matriks[k],parentIndex,&idx);
        if(IsStringSameBol(&idx,-1 +'0') || IsStringSameBol(idx,-2 + '0')) {
            for(i = 0; i < 64; i++) {
                if(dir[i*16+2] == 0x00) break;
            }
            if(i == 64) {
                *success = -2;
                return;
            }

            dir[i*16] = parentIndex;
            dir[i*16+1] = 0xFF;

            for(j = 0; j < 14; j++) {
                dir[i*16+2+j] = 0x00;
            }
            for(j = 0; j < foldername_length; j++) {
                dir[i*16+2+j] = matriks[k][j];
            }
            *success = 1;
            return;
        }
        *success = -1;
        return;              
    } 
}


void printString(char *string){
    int i = 0;
    while(string[i] != '\0'){
        interrupt(0x10, (0xe<<8)+string[i], 0, 0, 0);
        i++;
    }
}