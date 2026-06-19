#include "fs.h"
void prints(const char* s);
void print(const char t);

unsigned char* get_bitmap() {  //POMOCNA FUNKCIJA - POKAZIVAC NA BITMAP
    return (unsigned char*)(FS_BASE + sizeof(struct superblock) + sizeof(struct file_entry) * MAX_FILES);
}

int alloc_block() {
    unsigned char* bitmap = get_bitmap();
    for(int i = 0; i < MAX_BLOCKS; i++){
        if(bitmap[i] == 0){
            bitmap[i] = 1;
            return i;
        }
    }
    return -1;
}

void free_block(int index){
    unsigned char* bitmap = get_bitmap();
    bitmap[index] = 0;
}

void fs_init() {
    struct superblock* sb = (struct superblock*)FS_BASE;
    struct file_entry* files = (struct file_entry*)(FS_BASE + sizeof(struct superblock));

    if(sb->magic != FS_MAGIC){

        sb->magic = FS_MAGIC;
        sb->file_count = 0;
        sb->data_start = FS_BASE + sizeof(struct superblock) + sizeof(struct file_entry) * MAX_FILES + MAX_BLOCKS;
        for(int i = 0; i<MAX_FILES; i++){
            files[i].used = 0;
        }

        unsigned char* bitmap = get_bitmap();
        for(int i = 0; i < MAX_BLOCKS; i++){
            bitmap[i] = 0;
        }
        
    }
}

void fs_ls() {

    struct file_entry* files = (struct file_entry*)(FS_BASE + sizeof(struct superblock));
    for(int i= 0; i<MAX_FILES; i++){
        if(files[i].used == 1){
            prints(files[i].name);
            prints("\n");
        }
    }

}

int fs_create(char* ime){
    struct superblock* sb = (struct superblock*)FS_BASE;
    struct file_entry* files = (struct file_entry*)(FS_BASE + sizeof(struct superblock));
    for(int i = 0; i<MAX_FILES; i++){
        if(files[i].used == 0){
            
            int j = 0;
            while(ime[j] != '\0' && j<NAME_LEN - 1){
                files[i].name[j] = ime[j];
                j++;
            }
            files[i].name[j] = '\0';

            files[i].used = 1;
            files[i].size = 0;

            sb->file_count++;
            return 1;
        }
    }
    return 0;
}


//POMOCNA FUNKCIJA ZA POREDJENJE DVA STRINGA
int str_eq(char* a, char* b){
    int i = 0;
    while(a[i] != '\0' && b[i] != '\0'){
        if(a[i] != b[i]){
            return 0;       
        }
        i++;
    }
    return a[i] == b[i];     
}

int fs_write(char* ime, char* data, unsigned int len){

    struct file_entry* files = (struct file_entry*)(FS_BASE + sizeof(struct superblock));
    struct superblock* sb = (struct superblock*)FS_BASE;


    for(int i = 0; i<MAX_FILES;i++){

        if(files[i].used == 1 && str_eq(files[i].name, ime)){

            int num_of_blocks = (len + BLOCK_SIZE - 1) / BLOCK_SIZE;  // KOLIKO BLOKOVA JE POTREBNO ZAUZETI, len/velicina bloka zaokruzeno navise

            /*char* destination = (char*) files[i].start;
            for(int j = 0; j<len; j++){                                          STARI KOD
                destination[j] = data[j];
            }
            files[i].size = len;
            return 1;*/

            unsigned int copied = 0; // brojac koji prati koji bajt kopiramo trenutno, koliko ih je prekopirano
            for(int b = 0; b < num_of_blocks; b++){
                int idx = alloc_block();  // NADJES PRVI SLOBODAN BLOK
                files[i].blocks[b] = idx;  // FAJL PAMTI GDE SU NJEGOVI BLOKOVI TJ PODACI TJ NJEGOVI DELOVI
                char* dest = (char*)(sb->data_start + idx*BLOCK_SIZE);

                for(int k = 0; k<BLOCK_SIZE && copied< len; k++){
                    dest[k] = data[copied];
                    copied++;
                }

            }
            files[i].size = len;
            return 1;
        }
    }

    return 0;

}

int fs_read(char* ime){

    struct file_entry* files = (struct file_entry*)(FS_BASE + sizeof(struct superblock));
    struct superblock* sb = (struct superblock*)FS_BASE;


    for(int i = 0; i<MAX_FILES; i++){
        
        if(files[i].used == 1 && str_eq(files[i].name, ime)){

            /*char* data = (char*) files[i].start;
            for(int j = 0; j<files[i].size; j++){
                print(data[j]);
            }*/

            int num_of_blocks = (files[i].size + BLOCK_SIZE - 1) / BLOCK_SIZE;  // KOLIKO BLOKOVA JE POTREBNO ZAUZETI, len/velicina bloka zaokruzeno navise


            int printed = 0;
            for(int b = 0; b < num_of_blocks; b++){
                char* dest = (char*)(sb->data_start + files[i].blocks[b] * BLOCK_SIZE);
                for(int k = 0;k<BLOCK_SIZE && printed < files[i].size; k++){
                    print(dest[k]);
                    printed++;
                }
            }


            prints("\n");
            return 1;
        }
    }

    return 0;
}

int fs_delete(char* ime){

    struct superblock* sb = (struct superblock*)FS_BASE;
    struct file_entry* files = (struct file_entry*)(FS_BASE + sizeof(struct superblock));

     for(int i = 0; i<MAX_FILES; i++){
        
        if(files[i].used == 1 && str_eq(files[i].name, ime)){

            int num_of_blocks = (files[i].size + BLOCK_SIZE - 1) / BLOCK_SIZE;  // KOLIKO BLOKOVA JE POTREBNO ZAUZETI, len/velicina bloka zaokruzeno navise
            for(int b = 0; b<num_of_blocks; b++){
                free_block(files[i].blocks[b]);
            }

            files[i].used = 0;
            sb->file_count--;
            return 1;
        }
    }

    return 0;

}

int fs_move(char* ime, unsigned long long destination_address){
    
    struct superblock* sb = (struct superblock*)FS_BASE;


    struct file_entry* files = (struct file_entry*)(FS_BASE + sizeof(struct superblock));
    for(int i = 0; i<MAX_FILES; i++){

        if(files[i].used == 1 && str_eq(files[i].name, ime)){

            int num_of_blocks = (files[i].size + BLOCK_SIZE - 1) / BLOCK_SIZE;  // KOLIKO BLOKOVA JE POTREBNO ZAUZETI, len/velicina bloka zaokruzeno navise

            //char* src = (char*) files[i].start;
            char* dst = (char*)destination_address;

            int copied = 0;

            for(int b = 0; b<num_of_blocks; b++){
                char* dest = (char*)(sb->data_start + files[i].blocks[b] * BLOCK_SIZE);
                for(int k = 0; k < BLOCK_SIZE && copied < files[i].size; k++){
                    dst[copied] = dest[k];
                    copied++;
                }
            }
            return 1;
        }

    }

    return 0;

}


unsigned long long fs_find(char* ime){

    struct superblock* sb = (struct superblock*)FS_BASE;

    
    struct file_entry* files = (struct file_entry*)(FS_BASE + sizeof(struct superblock));

     for(int i = 0; i<MAX_FILES; i++){
        
        if(files[i].used == 1 && str_eq(files[i].name, ime)){

            return sb->data_start + files[i].blocks[0] * BLOCK_SIZE;

        }
    }

    return 0;

}
