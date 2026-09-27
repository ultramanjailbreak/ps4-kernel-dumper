# Target binary output name
TARGET = ez.bin

# Toolchain definitions
CC      := gcc
OBJCOPY := objcopy
ODIR    := build
SDIR    := .

# Automatically find any .c file in the current directory
SRC_FILE := $(wildcard $(SDIR)/*.c)
OBJS     := $(ODIR)/$(notdir $(SRC_FILE:.c=.o))

# SDK Directory Path mapping
LIBPS4  := $(PS4SDK)/libPS4

# Compilation and Linking flags for raw flat payloads
CFLAGS  := -I$(LIBPS4)/include -O2 -std=c11 -Wall -Wextra -fno-builtin -nostdlib -fPIC
LDFLAGS := -T $(LIBPS4)/linker.x -Xlinker -odir build -Xlinker -Tdata=0x926200000

all: $(TARGET)

# Rule to compile the source file (explicitly makes build directory first)
$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

# Fallback rule in case files are inside a "src" folder
$(ODIR)/%.o: $(SDIR)/src/%.c
	@mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

# Chains objects into raw flat binary format
$(TARGET): $(OBJS)
	$(CC) $(LIBPS4)/crt0.s $(OBJS) -o $(ODIR)/temp.t $(CFLAGS) $(LDFLAGS) -L$(LIBPS4) -lPS4
	$(OBJCOPY) -O binary $(ODIR)/temp.t $(TARGET)
	@rm -f $(ODIR)/temp.t
	@echo "---------------------------------------"
	@echo "Flat binary successfully compiled: $(TARGET)"
	@echo "---------------------------------------"

clean:
	rm -rf $(ODIR) $(TARGET)

.PHONY: all clean
