#include <stdio.h>
#include "util.h"

void makeDir(char* folder_name, int parentIndex) {
    char files[1024], map[512], sectors[512];

    interrupt(0x21,0x)
}