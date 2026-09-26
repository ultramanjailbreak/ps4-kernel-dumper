# Ensure the PS4 SDK environment variable is defined
ifeq ($(PS4SDK),)
$(error "Please set your PS4SDK environment variable before running make.")
endif

# Path Configuration
LIBPS4  := $(PS4SDK)/libPS4
ODIR    := build
SDIR    := source

# Compiler & Toolchain Definition
CC      := gcc
OBJCOPY := objcopy

# Target Configurations
TARGET  := $(shell basename "$(CURDIR)").bin
MAPFILE := $(shell basename "$(CURDIR)").map

# Include and Library Directories
IDIRS   := -I$(LIBPS4)/include -Iinclude
LDIRS   := -L$(LIBPS4)

# Compilation & Linker Flags
CFLAGS  := $(IDIRS) -Os -std=c11 -ffunction-sections -fdata-sections -fno-builtin \
           -nostartfiles -nostdlib -Wall -Wextra -masm=intel -march=btver2 -mtune=btver2 \
           -m64 -mabi=sysv -mcmodel=small -fpie -fPIC

LFLAGS  := $(LDIRS) -Xlinker -T $(LIBPS4)/linker.x -Xlinker -Map="$(MAPFILE)" \
           -Wl,--build-id=none -Wl,--gc-sections

LIBS    := -lPS4

# Source Files Discovery
CFILES  := $(wildcard $(SDIR)/*.c)
SFILES  := $(wildcard $(SDIR)/*.s)

# Object Files Mapping
OBJS    := $(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(CFILES)) \
           $(patsubst $(SDIR)/%.s, $(ODIR)/%.o, $(SFILES))

.PHONY: all clean

all: $(TARGET)

# Link the output payload binary
$(TARGET): $(ODIR) $(OBJS)
	$(CC) $(LIBPS4)/crt0.s $(ODIR)/*.o -o temp.t $(CFLAGS) $(LFLAGS) $(LIBS)
	$(OBJCOPY) -O binary temp.t "$(TARGET)"
	@rm -f temp.t
	@echo "Build successful: $(TARGET)"

# Compile C source files into objects
$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

# Assemble assembly source files into objects
$(ODIR)/%.o: $(SDIR)/%.s
	$(CC) -c -o $@ $< $(CFLAGS)

# Create build output directory if missing
$(ODIR):
	@mkdir -p $@

# Clean up all generated build artifacts
clean:
	rm -rf "$(TARGET)" "$(MAPFILE)" $(ODIR)
