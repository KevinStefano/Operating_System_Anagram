// loadFile.c
// Michael Black, 2007
//
// Loads a file into the file system
// This should be compiled with gcc and run outside of the OS

#include <stdio.h>

void main(int argc, char* argv[]) {
  int i;

  if (argc < 2) {
    printf("Specify file name to load\n");
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
    if (files[i] == 0) break;
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
  files[dirindex + 1] = sectoridx;
  // fill the name field with 00s first
  for (i = 0; i < 14; i++) files[dirindex + 2 + i] = 0x00;
  // copy the name over
  for (i = 0; i < 12; i++) {
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
// void readSector(char* buffer, FILE* file, int sector) {
//   int i;
//   fseek(file, sector*512, SEEK_SET);
//   for(i = 0; i < 512; i++) {
//     buffer[i] = fgetc(file);
//   }
// }

// void writeSector(char* buffer, FILE* file, int sector) {
//   int i;
//   fseek(file, sector*512, SEEK_SET);
//   for(i = 0; i < 512; i++) {
//     fputc(buffer[i],file);
//   }
// }

// void copySector(FILE* source, int source_sector, FILE* destination, int destination_sector) {
//   int i;
//   fseek(destination, destination_sector*512, SEEK_SET);
//   for(i = 0; i < 512; i++) {
//     if(feof(source)) {
//       fputc(0x00,destination);
//       return;
//     }
//     else {
//       fputc(fgetc(source), destination);
//     }
//   }
// }

// void writeName(char* file, int file_index, char* filename) {
//   int i = 0;
//   while(filename[i] != 0x00) {
//     file[file_index*16+2+i] = filename[i];
//     i++;
//   }
//   while(i < 14) {
//     file[file_index*16+2+i] = 0x00;
//     i++;
//   }
// }

// int findEmptyEntry(char* file) {
//   int file_index;
//   for(file_index = 0; file_index < 64; file_index++) {
//     if(file[file_index*16+2] == 0x00) {
//       return file_index;
//     }
//   }

//   return -2;
// }

// int findEmptySector(char* map) {
//   int sector_idx;
//   for(sector_idx = 0; sector_idx < 256; sector_idx++) {
//     if(map[sector_idx] == 0x00) {
//       return sector_idx;
//     }
//   }

//   return -3;
// }

// int findEmptySector_Sector(char* sectors) {
//   int sector_idx;
//   for(sector_idx = 0; sector_idx < 32; sector_idx++) {
//     if(sectors[sector_idx*16] == 0x00) {
//       return sector_idx;
//     }
//   }

//   return -3;
// }

// void main(int argc, char* argv[]) {
//   char map[512], files[1024], sector_[512];
//   //validasi nama file
//   if(argc < 2) {
//     printf("Nama file tidak ada\n");
//     return;
//   }
//   //buka file
//   FILE* file_loaded;
//   file_loaded = fopen(argv[1], "r");
//   if(file_loaded == 0) {
//     printf("File tidak ditemukan!\n");
//     return;
//   }
//   //buka floppy
//   FILE* floppy;
//   floppy = fopen("floppya.img", "r+");
//   if(floppy == 0) {
//     printf("floppya.img tidak ditemukan!\n");
//     return;
//   }
//   //load semua sector
//   readSector(map,floppy,0x100);
//   readSector(files,floppy,0x101);
//   readSector(files+512,floppy,0x102);
//   readSector(sector_,floppy,0x103);

//   //cari entri kosong di files
//   int file_index = findEmptyEntry(files);
//   if(file_index != -2) {
//     int count_sectorneeded = 0;
//     int sector_idx = findEmptySector_Sector(sector_);
//     if(sector_idx != -3) {
//       while(!feof(file_loaded)) {
//         int empty_sector = findEmptySector(map);
//         if(empty_sector != -3) {
//           copySector(file_loaded,count_sectorneeded,floppy,empty_sector);
//           printf("Loaded %s to sector %d\n",argv[1],empty_sector);
//           map[empty_sector] = 0xFF;
//           sector_[sector_idx*16+count_sectorneeded] = empty_sector;
//           count_sectorneeded++;
//         }
//         else {
//           printf("Sector tidak tersedia!\n");
//           return;
//         }
//       }
//       files[file_index*16] = 0xFF;
//       writeName(files,file_index,argv[1]);
//       writeSector(map,floppy,0x100);
//       writeSector(files,floppy,0x101 );
//       writeSector(files+512,floppy,0x102);
//       writeSector(sector_,floppy,0x103);
//     }
//     else {
//       printf("Entri sektor sudah penuh!\n");
//       return;
//     }
//   }
//   else {
//     printf("Entri sudah penuh!\n");
//     return;
//   }
//   fclose(floppy);
//   fclose(file_loaded);
// }