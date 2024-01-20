.PHONY: all build hd clean
BUILD_DIR=./build
ENTRY_POINT=0xc0001500
SRC_DIRS=./kernel ./device ./lib ./lib/kernel ./lib/user
AS=nasm
CC=gcc
LD=ld
LIB= -I ./lib/kernel -I ./lib -I ./kernel -I ./device -I ./lib/user
ASFLAGS= -f elf
ASBINLIB= -I ./boot/include/

CFLAGS= $(LIB) -m32 -c -O0 -fno-builtin -fno-stack-protector -Wno-format-security \
		-Wall -W -Wstrict-prototypes  -Wmissing-prototypes

LDFLAGS=-Ttext  $(ENTRY_POINT) -e main  -m elf_i386 -Map $(BUILD_DIR)/kernel.map


SRCS=$(notdir $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c) $(wildcard $(dir)/*.S)))
SRCS_H_D=$(wildcard $(BUILD_DIR)/*.d)
SRCS_BUILD_PATH=$(addprefix $(BUILD_DIR)/,$(SRCS))
OBJS =$(SRCS_BUILD_PATH:.c=.o)
OBJS :=$(OBJS:.S=.o)
OBJS := $(filter $(BUILD_DIR)/main.o, $(OBJS)) $(filter-out $(BUILD_DIR)/main.o, $(OBJS))
-include SRCS_H_D

$(BUILD_DIR)/%.bin:./boot/%.S
	$(AS) $(ASBINLIB) $< -o $@
$(BUILD_DIR)/kernel.bin:$(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/print.o:./lib/kernel/print.S
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/kernel.o:./kernel/kernel.S
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o:./device/%.c
	$(CC) $(CFLAGS) $< -o $@ -MMD -MF $(BUILD_DIR)/$*.d -MP
$(BUILD_DIR)/%.o:./kernel/%.c
	$(CC) $(CFLAGS) $< -o $@ -MMD -MF $(BUILD_DIR)/$*.d -MP

build:$(BUILD_DIR)/kernel.bin $(BUILD_DIR)/mbr.bin $(BUILD_DIR)/loader.bin

hd:build
	@dd if=$(BUILD_DIR)/mbr.bin of=./boot/hd60M.img bs=512 count=1 conv=notrunc
	@dd if=$(BUILD_DIR)/loader.bin of=./boot/hd60M.img bs=512 count=4  seek=2 conv=notrunc
	@dd if=$(BUILD_DIR)/kernel.bin of=./boot/hd60M.img bs=512 count=200  seek=9 conv=notrunc

all:hd
	@objcopy --remove-section=.note.gnu.property $(BUILD_DIR)/kernel.bin

clean:
	rm $(BUILD_DIR)/*

