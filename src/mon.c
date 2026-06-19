
const char* prompt = "> ";

//ZA FAJL SISTEM
void fs_init();
void fs_ls();
int fs_create(char* ime);
int fs_write(char* ime, char* data, unsigned int len);
int fs_read(char* ime);
int fs_delete(char* ime);
int fs_move(char* ime, unsigned long long destination_address);
unsigned long long fs_find(char* ime);
//////////////

#define BASE 0x10000000
#define MAXBUFF 1024
#define INPUT 0x1
#define OUTPUT ((1 << 5) | (1 << 6))


volatile char* UART = (char*)BASE;
volatile char *LSR = (char *)BASE + 0x5;



void busy_wait(char mask) {
    while ((*LSR & mask) == 0)
        ;
}



void print(const char t) {
    busy_wait(OUTPUT);
    *UART = t;
}

void prints(const char* s) {
    int i = 0;
    while (s[i] != '\0') {
        print(s[i]);
        i++;
    }
}


void input(char* buff) {
    int i = 0;
    do {
        busy_wait(INPUT);
        buff[i] = *UART;
        if ((buff[i] == '\b' || buff[i] == 0x7F) && i > 0) {
            print('\b');
            print(' ');
            print('\b');
            i -= 2;
        }
        else {
            print(buff[i]);
        }
    } while (buff[i] != '\r' && i++ < MAXBUFF);
    buff[i] = '\0';
}


int get_read(char* s, unsigned long long* addr, unsigned int* number) {
    unsigned long long address_acc = 0;
    unsigned int number_acc = 0;
    int i = 0;
    while (s[i] != ' ' && s[i] != '\r' && s[i] != '\0') {
        address_acc = address_acc << 4;
        if (s[i] >= '0' && s[i] <= '9') {
            address_acc += s[i] - '0';
        }
        else if (s[i] >= 'a' && s[i] <= 'f') {
            address_acc += s[i] - 'a' + 10;
        }
        else if (s[i] >= 'A' && s[i] <= 'F') {
            address_acc += s[i] - 'A' + 10;
        }
        else {
            return 0;
        }
        i++;
    }
    *addr = address_acc;
    if (s[i] == ' ') {
        i++;
        while (s[i] != ' ' && s[i] != '\r' && s[i] != '\0') {
            number_acc *= 10;
            if (s[i] >= '0' && s[i] <= '9') {
                number_acc += s[i] - '0';
            }
            else {
                return 0;
            }
            i++;
        }
        if (number_acc < 1) {
            return 0;
        }
        *number = number_acc;
        return 1;
    }
    else {
        return 0;
    }
}

int get_write(char* s, unsigned long long* addr, char* data, unsigned int* n) {
    unsigned long long address_acc = 0;
    unsigned char byte_acc = 0;
    int i = 0;
    unsigned int j = 0;
    int bcount = 0;
    *n = 0;
    while (s[i] != ' ' && s[i] != '\r' && s[i] != '\0') {
        address_acc = address_acc << 4;
        if (s[i] >= '0' && s[i] <= '9') {
            address_acc += s[i] - '0';
        }
        else if (s[i] >= 'a' && s[i] <= 'f') {
            address_acc += s[i] - 'a' + 10;
        }
        else if (s[i] >= 'A' && s[i] <= 'F') {
            address_acc += s[i] - 'A' + 10;
        }
        else {
            return 0;
        }
        i++;
    }
    *addr = address_acc;
    while (s[i] != '\r' && s[i] != '\0') {
        if (s[i] == ' ') {
            i++;
            continue;
        }
        byte_acc = byte_acc << 4;
        if (s[i] >= '0' && s[i] <= '9') {
            byte_acc += s[i] - '0';
        }
        else if (s[i] >= 'a' && s[i] <= 'f') {
            byte_acc += s[i] - 'a' + 10;
        }
        else if (s[i] >= 'A' && s[i] <= 'F') {
            byte_acc += s[i] - 'A' + 10;
        }
        else {
            return 0;
        }
        bcount++;
        if (bcount == 2) {
            bcount = 0;
            data[j] = byte_acc;
            byte_acc = 0;
            j++;
        }
        i++;
    }
    if (bcount != 0) {
        return 0;
    }
    *n = j;
    return 1;
}

int get_execute(char* s, unsigned long long* addr) {
    unsigned long long address_acc = 0;
    int i = 0;
    while (s[i] != ' ' && s[i] != '\r' && s[i] != '\0') {
        address_acc = address_acc << 4;
        if (s[i] >= '0' && s[i] <= '9') {
            address_acc += s[i] - '0';
        }
        else if (s[i] >= 'a' && s[i] <= 'f') {
            address_acc += s[i] - 'a' + 10;
        }
        else if (s[i] >= 'A' && s[i] <= 'F') {
            address_acc += s[i] - 'A' + 10;
        }
        else {
            return 0;
        }
        i++;
    }
    *addr = address_acc;
    return 1;
}


char nybble_to_hex(char n) {
    if (n < 10) {
        return '0' + n;
    }
    else {
        return 'A' + n - 10;
    }
}


void print_addr(unsigned long long addr){
    prints("0x");
    for(int i = 60; i>=0; i-=4){
        print(nybble_to_hex((addr>>i) & 0xF));
    }
    print('\n');
}


void print_bytes(char* data, unsigned int n) {
    int gcount = 0;
    int rcount = 0;
    for (int i = 0; i < n; i++) {
        print(nybble_to_hex((data[i] >> 4) & 0xF));
        print(nybble_to_hex((data[i]) & 0xF));
        gcount++;
        rcount++;
        if (gcount == 2) {
            gcount = 0;
            print(' ');
            print(' ');
        }
        if (rcount == 8) {
            rcount = 0;
            print('\n');
        }
    }
}


//"MAIN"

void monitor() {
    char buff[MAXBUFF + 2];
    char databuff[MAXBUFF];
    unsigned long long addr = 0;
    unsigned int number = 0;

    //FAJL SISTEM
    fs_init();
    /////////////


    while (1) {
        prints(prompt);
        input(buff);
        print('\n');
        switch (buff[0]) {
        case 'r':
            if (get_read(buff + 2, &addr, &number)) {
                if (number > MAXBUFF) {
                    prints("Broj bajtova je prevelik.\n");
                }
                else {
                    print_bytes((char*)addr, number);
                }
				print('\n');
			}
            else {
                prints("Neispravna adresa ili broj bajtova.\n");
            }
            break;
        case 'w':
            if (get_write(buff + 2, &addr, databuff, &number)) {
                char* ptr = (char*)addr;
                for (int i = 0; i < number; i++) {
                    *(ptr + i) = databuff[i];
                }
                print('\n');
            }
            else {
                prints("Neispravna adresa ili bajtovi.\n");
            }
            break;
        case 'x':
            get_execute(buff + 2, &addr);
            asm volatile("fence.i" ::: "memory");
            asm("jr %0\n\t" : : "r"(addr) : "memory");
            break;
		case 't':
			if (get_read(buff + 2, &addr, &number)) {
				char *ptr = (char *)addr;
				for (int i = 0; i < number; i++) {
					print(ptr[i]);
				}
				print('\n');
			} else {
				prints("Neispravna adresa ili broj bajtova.\n");
			}
			break;

        case 'l':
            
            fs_ls();
            break;
        
        case 'c':

            fs_create(buff + 2);
            break;
    
        case 'p':
            
            if(!fs_read(buff+2)) {
                prints("Fajl ne postoji.\n");
            }
            break;
    
        case 's':{

            char* p = buff + 2;
            int k = 0; 

            while(p[k] != ' ' && p[k] != '\r' && p[k] != '\0'){
                k++;
            }

            if (p[k] != ' ') {        
                prints("Format: s ime podaci\n");
                break;
            }
            
            p[k] = '\0';                 
            char* ime = p;               
            char* data = p + k + 1;

            unsigned int len = 0;
            while(data[len] != '\r' && data[len] != '\0'){
                len++;
            }

            if(!fs_write(ime, data, len)){
                prints("Fajl ne postoji.\n");
            }

            break;
        }

        case 'd':

            if(!fs_delete(buff + 2)){
                prints("Fajl ne postoji.\n");
            }
            break;

        

        case 'm':{

            
            char* p = buff + 2;
            int k = 0;
            
            while(p[k] != ' ' && p[k] != '\r' && p[k] != '\0'){
                
                k++;

            } 
            
            if(p[k] != ' '){
                prints("Format: m ime adresa\n");
                break;
            }

            p[k] = '\0';

            char* ime = p;

            unsigned long long dest = 0;

            if(!get_execute(p + k + 1, &dest)){
                prints("Neispravna adresa.\n");
                break;
            }

            if(!fs_move(ime,dest)){
                prints("Fajl ne postoji.\n");
            }

            break;

        }

        case 'e':{

            unsigned long long target = fs_find(buff + 2);
            if(target == 0){
                prints("Fajl ne postoji.\n");
                break;
            }

            asm volatile("fence.i" ::: "memory");
            asm("jr %0\n\t" : : "r"(target) : "memory");
            break;

        }

        case 'a':{

            unsigned long long a = fs_find(buff + 2);
            if(a == 0){
                prints("Fajl ne postoji.");
                break;
            }

            print_addr(a);
            break;

        }

    }

  }

}