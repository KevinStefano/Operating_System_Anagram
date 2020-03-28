//LIBRARY MATEMATIKA
//isi : fungsi mod dan div

#ifndef __LIBMATEMATIKA_H
#define __LIBMATEMATIKA_H

int mod(int bil1, int bil2);
int div(int bil1, int bil2);


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