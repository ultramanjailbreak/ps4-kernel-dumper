# Default parameters for optional command line deployment testing
PS4_HOST ?= ps4
PS4_PORT ?= 9090  # Synced to GoldHEN's 9090 default layout

# Output properties mapping
TARGET   := pup-installer
PAYLOAD  := ez.bin

# SDK Directory Path mapping (Derived from your GitHub environment run)
LIBPS4  := $(PS4SDK)/libPS4

# Toolchain definitions
CC      := gcc
OBJCOPY := objcopy

# Discovers source configurations automatically from your workspace root
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

# Compilation and Linking flags for raw flat payloads on Scene-Collective SDK
CFLAGS  := -I$(LIBPS4)/include -O2 -std=c11 -Wall -Wextra -fno-builtin -nostdlib -fPIC
LFLAGS  := -T $(LIBPS4)/linker.x -Xlinker -Tdata=0x926200000

all: $(PAYLOAD)

# Compiles discovered items directly in the workspace directory
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

# Chains the assembly bootstrap crt0 and compilation objects into a raw flat binary binary
$(PAYLOAD): $(OBJS)
	$(CC) $(LIBPS4)/crt0.s $(OBJS) -o temp.elf $(CFLAGS) $(LFLAGS) -L$(LIBPS4) -lPS4
	$(OBJCOPY) -O binary temp.elf $@
	@rm -f temp.elf *.o
	@echo "---------------------------------------"
	@echo "Flat binary successfully compiled: $(PAYLOAD)"
	@echo "---------------------------------------"

clean:
	-rm -f *.o *.elf *.bin

.PHONY: all clean
