# Target binary output name
TARGET = target_spoofer.bin

# Toolchain definitions (Scene-Collective relies on standard gcc/binutils)
CC      := gcc
OBJCOPY := objcopy
ODIR    := build
SDIR    := .

# Target objects matching compilation setup
OBJS    := $(ODIR)/main.o

# SDK Directory Path mapping (dynamically linked via the workflow environment)
LIBPS4  := $(PS4SDK)/libPS4

# Compilation and Linking flags for raw flat payloads
CFLAGS  := -I$(LIBPS4)/include -O2 -std=c11 -Wall -Wextra -fno-builtin -nostdlib -fPIC
LFLAGS  := -T $(LIBPS4)/linker.x -Xlinker -odir build -Xlinker -Tdata=0x926200000

# Compilation Instructions
all: $(TARGET)

$(ODIR)/%.o: $(SDIR)/%.c | $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR):
	@mkdir -p $@

# Chains the assembly bootstrap crt0 and compilation objects into a raw flat binary binary
$(TARGET): $(OBJS)
	$(CC) $(LIBPS4)/crt0.s $(OBJS) -o $(ODIR)/temp.t $(CFLAGS) $(LFLAGS) -L$(LIBPS4) -lPS4
	$(OBJCOPY) -O binary $(ODIR)/temp.t $(TARGET)
	@rm -f $(ODIR)/temp.t
	@echo "---------------------------------------"
	@echo "Flat binary successfully compiled: $(TARGET)"
	@echo "---------------------------------------"

clean:
	rm -rf $(ODIR) $(TARGET)

.PHONY: all clean
