// loadFile.c
// Michael Black, 2007
//
// Loads a file into the file system
// This should be compiled with gcc and run outside of the OS

#include <stdio.h>
int div(int bil1, int bil2){
    int z = 0;
    while(bil2 * z<=bil1){
        z++;
    }
    return z-1;
}

int main(int argc, char* argv[]) {
  int i;

  if (argc < 2) {
    printf("Specify file name \n");
    return;
  }

  // open the source file
  FILE* loadFil;
  loadFil = fopen(argv[1], "r");
  if (loadFil == 0) {
    printf("File not found\n");
    return;
  }

  // open the floppy image
  FILE* floppy;
  floppy = fopen("system.img", "r+");
  if (floppy == 0) {
    printf("system.img not found\n");
    return;
  }

  // load the disk map
  char map[512];
  fseek(floppy, 512 * 0x100, SEEK_SET);
  for (i = 0; i < 512; i++) map[i] = fgetc(floppy);

  // load the directory
  char files[1024];
  fseek(floppy, 512 * 0x101, SEEK_SET);
  for (i = 0; i < 1024; i++) files[i] = fgetc(floppy);

  //load the sectors
  char sectors[512];
  fseek(floppy, 512 * 0x103, SEEK_SET);
  for(i = 0; i < 512; i++) sectors[i] = fgetc(floppy);

  // find a free entry in the directory
  for (i = 0; i < 1024; i += 16)
    if (files[i+2] == 0) break;
  if (i == 1024) {
    printf("Not enough room in directory\n");
    return;
  }
  int dirindex = i;

  // fill the parent with 0xFF
  files[dirindex] = 0xFF;
  //find a free sector in the sectors
  for(i = 0; i < 512; i += 16)
    if(sectors[i] == 0) break;
  if(i == 512) {
    printf("Not enough sector\n");
    return;
  }
  int sectoridx = i;
  // fill the sector entry
  files[dirindex + 1] = div(sectoridx,16);
  // fill the name field with 00s first
  for (i = 0; i < 14; i++) files[dirindex + 2 + i] = 0x00;
  // copy the name over
  for (i = 0; i < 14; i++) {
    if (argv[1][i] == 0) break;
    files[dirindex + 2 + i] = argv[1][i];
  }

  // find free sectors and add them to the file
  int sectcount = 0;
  while (!feof(loadFil)) {
    if (sectcount == 16) {
      printf("Not enough space in directory entry for file\n");
      return;
    }

    // find a free map entry
    for (i = 0; i < 256; i++)
      if (map[i] == 0) break;
    if (i == 256) {
      printf("Not enough room for file\n");
      return;
    }

    // mark the map entry as taken
    map[i] = 0xFF;

    // mark the sector in the directory entry
    sectors[sectoridx+sectcount] = i;
    sectcount++;

    printf("Loaded %s to sector %d\n", argv[1], i);

    // move to the sector and write to it
    fseek(floppy, i * 512, SEEK_SET);
    for (i = 0; i < 512; i++) {
      if (feof(loadFil)) {
        fputc(0x0, floppy);
        break;
      } else {
        char c = fgetc(loadFil);
        fputc(c, floppy);
      }
    }
  }

  // write the map and directory and sectors back to the floppy image
  fseek(floppy, 512 * 0x100, SEEK_SET);
  for (i = 0; i < 512; i++) fputc(map[i], floppy);

  fseek(floppy, 512 * 0x101, SEEK_SET);
  for (i = 0; i < 1024; i++) fputc(files[i], floppy);

  fseek(floppy, 512 * 0x103, SEEK_SET);
  for(i = 0; i < 512; i++) fputc(sectors[i],floppy);

  fclose(floppy);
  fclose(loadFil);
}