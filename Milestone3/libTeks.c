
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
                if (i>=1) {
                    interrupt(0x10, (0xe<<8)+0x8, 0, 0, 0); //backspace 1x
                    interrupt(0x10, (0xe<<8)+0x0, 0, 0, 0); //jadiin nul
                    interrupt(0x10, (0xe<<8)+0x8, 0, 0, 0); //backspace 1x
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
 }