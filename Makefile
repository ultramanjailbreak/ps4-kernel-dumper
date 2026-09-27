# Target binary output name
TARGET = target_spoofer.bin

# Toolchain definitions
CC      := gcc
OBJCOPY := objcopy
ODIR    := build
SDIR    := .

# AUTOMATICALLY FIND SOURCE FILE:
# This dynamically finds any .c file in your directory so compilation won't crash
SRC_FILE := $(wildcard $(SDIR)/*.c)
OBJS     := $(ODIR)/$(notdir $(SRC_FILE:.c=.o))

# SDK Directory Path mapping
LIBPS4  := $(PS4SDK)/libPS4

# Compilation and Linking flags
CFLAGS  := -I$(LIBPS4)/include -O2 -std=c11 -Wall -Wextra -fno-builtin -nostdlib -fPIC
LFLAGS  := -T $(LIBPS4)/linker.x -Xlinker -odir build -Xlinker -Tdata=0x926200000

all: $(TARGET)

# Rule to compile the discovered source file
$(ODIR)/%.o: $(SDIR)/%.c | $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

# Fallback rule in case your files are inside a "src" folder
$(ODIR)/%.o: $(SDIR)/src/%.c | $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR):
	@mkdir -p $@

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
