CC = gcc
LD = ld
DD = dd
GDB = gdb
OBJCOPY = objcopy

INCLUDE = -I include/

IMAGE = build/bin/os.img

IGNORE = src/boot_loader

OBJ_DIR = build/obj
DEBUG_DIR = build/debug

C_32_FLAGS  = -m32 -Wall -Werror -nostdinc -nostdinc -ffreestanding
C_32_FLAGS += -Os -ggdb
C_32_FLAGS += -fno-builtin -fno-pic -fno-stack-protector
C_32_FLAGS += -mno-mmx -mno-sse -mno-sse2
C_32_FLAGS += $(INCLUDE) -c

C_64_FLAGS  = -m64 -Wall -Werror -nostdlib -nostdinc -ffreestanding
C_64_FLAGS += -Os -ggdb
C_64_FLAGS += -mno-red-zone -mcmodel=large
C_64_FLAGS += -fno-builtin -fno-pic -fno-stack-protector -fno-strict-aliasing
# need %cr0 and %cr4 setup
C_64_FLAGS += -mno-mmx -mno-sse -mno-sse2
C_64_FLAGS += $(INCLUDE) -c

LD_32_FLAGS = -m elf_i386   -nostdlib --omagic
LD_64_FLAGS = -m elf_x86_64 -nostdlib --omagic

OBJCOPY_FLAGS = -O binary -j .text -j .rodata -j .data -j .bss

QEMU = qemu-system-x86_64
QEMU_FLAGS  = -no-shutdown -no-reboot
QEMU_FLAGS += -m 32M
QEMU_FLAGS += -hda $(IMAGE)

C_SOURCES = $(shell find src -type d -path $(IGNORE) -prune -o -name "*.c" -print)
S_SOURCES = $(shell find src -type d -path $(IGNORE) -prune -o -name "*.S" -print)

OBJECTS  = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(C_SOURCES)))
OBJECTS += $(patsubst %.S, $(OBJ_DIR)/%.o, $(notdir $(S_SOURCES)))

.PHONY : all main run debug clean test
# all: main boot_loader setup build/kernel.elf $(IMAGE)
all: build/kernel.elf

main:
	mkdir -p build/ build/bin/ $(OBJ_DIR) $(DEBUG_DIR)
	echo -e -n "\x55\xaa" > build/mbr_magic

boot_loader:
	$(CC) $(C_32_FLAGS) $(IGNORE)/boot.S -o $(OBJ_DIR)/boot.o
	$(CC) $(C_32_FLAGS) $(IGNORE)/loader.c -o $(OBJ_DIR)/loader.o
	$(LD) $(LD_32_FLAGS) -e real_mode -Ttext 0x7c00 $(OBJ_DIR)/boot.o $(OBJ_DIR)/loader.o -o $(DEBUG_DIR)/boot_loader_32
	$(OBJCOPY) $(OBJCOPY_FLAGS) $(DEBUG_DIR)/boot_loader_32 build/boot_loader_32.bin

setup:
	$(CC) $(C_64_FLAGS) $(IGNORE)/entry_64.S -o $(OBJ_DIR)/entry_64.o
	$(LD) $(LD_64_FLAGS) -e entry_64 -Ttext 0x0700 $(OBJ_DIR)/entry_64.o -o $(DEBUG_DIR)/entry_64
	$(OBJCOPY) $(OBJCOPY_FLAGS) $(DEBUG_DIR)/entry_64 build/entry_64.bin

build/kernel.elf: $(OBJECTS)
	$(LD) $(LD_64_FLAGS) -T script/kernel.ld $^ -o $(DEBUG_DIR)/kernel
	$(OBJCOPY) -S $(DEBUG_DIR)/kernel $@
	$(DD) if=$@ of=$(IMAGE) bs=512 seek=2 conv=notrunc

$(OBJ_DIR)/%.o: src/%.c
	$(CC) $(C_64_FLAGS) $< -o $@

$(OBJ_DIR)/%.o: src/%.S
	$(CC) $(C_64_FLAGS) $< -o $@

$(IMAGE):
	$(DD) if=/dev/zero 		 	      of=$(IMAGE) bs=512 count=2
	$(DD) if=build/boot_loader_32.bin of=$(IMAGE) bs=446 count=1 conv=notrunc
	$(DD) if=build/mbr_magic   	      of=$(IMAGE) bs=510 seek=1  conv=notrunc
	$(DD) if=build/entry_64.bin 	  of=$(IMAGE) bs=512 seek=1  conv=notrunc
	$(DD) if=build/kernel.elf	 	  of=$(IMAGE) bs=512 seek=2  conv=notrunc

run:
	$(QEMU) $(QEMU_FLAGS)

debug:
	$(QEMU) -S -s -serial stdio $(QEMU_FLAGS) &
	gdb -q -x script/.gdbinit

clean:
	rm -rf build/