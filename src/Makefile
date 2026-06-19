# 1. Popravljeno: Dodat unknown da sistem prepozna linker
LD := riscv64-unknown-elf-ld 
# 2. Popravljeno: Isto dodat unknown
AS := riscv64-unknown-elf-as
CC := riscv64-unknown-elf-gcc
QEMU := qemu-system-riscv64
NAME = mon 
EXE = $(addsuffix .elf,$(NAME)) 
LDSCRIPT := $(addsuffix .ld,$(NAME)) 
LDFLAGS := --no-dynamic-linker -m elf64lriscv -static -nostdlib
# 3. POPRAVLJENO: Dodato -march=rv64g_zifencei koje rešava grešku sa fence.i instrukcijom
CCFLAGS := -ffreestanding -mcmodel=medany -march=rv64g_zifencei
OBJS := $(addsuffix .o,$(NAME)) start.o fs.o

ifeq ($(DEBUG), 1)
	CCFLAGS += -g -O0 
else 
	CCFLAGS += -O2 
	LDFLAGS += -s 
endif 

$(EXE): $(OBJS) 
	$(LD) $(LDFLAGS) -T $(LDSCRIPT) $(OBJS) -o $(EXE) 

start.o: start.s 
	riscv64-unknown-elf-as start.s -o start.o  

%.o: %.c 
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f *.o 
	rm -f *.elf  

qemu: $(EXE) 
	$(QEMU) -machine virt -bios $(EXE) -serial stdio -m 128M  

qemu-debug: $(EXE)  
	$(QEMU) -machine virt -bios $(EXE) -serial stdio -m 128M -s -S  
