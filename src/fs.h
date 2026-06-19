#define FS_BASE 0x81000000
#define MAX_FILES 64
#define NAME_LEN 28
#define FS_MAGIC 0xF11E5A5A
//#define FILE_DATA_SIZE 4096

#define BLOCK_SIZE 4096
#define BLOCKS_PER_FILE 12
#define MAX_BLOCKS 768

struct file_entry{

    char name[NAME_LEN];  // IME FAJLA
    unsigned long long blocks[BLOCKS_PER_FILE]; // INDEKSI BLOKOVA TOG FAJLA
    unsigned int size; // VELIČINA U BAJTOVIMA
    unsigned char used; // 1 = ZAUZET SLOT

};

struct superblock{

    unsigned int magic;   // BROJ KOJI GOVORI O ISPRAVNOSTI SISTEMA
    unsigned int file_count;  // TRENUTNI BROJ FAJLOVA
    unsigned long long data_start;   //POČETAK BLOKOVA PODATAKA

};