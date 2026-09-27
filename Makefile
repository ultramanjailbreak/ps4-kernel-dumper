# Default parameters for deployment testing
PS4_HOST ?= ps4
PS4_PORT ?= 9090  # Updated to match GoldHEN's 9090 layout

# Map environmental variables directly from your workflow setup
ifdef PS4SDK
    include $(PS4SDK)/toolchain/orbis.mk
else ifdef PS4_PAYLOAD_SDK
    include $(PS4_PAYLOAD_SDK)/toolchain/orbis.mk
else
    $(error Neither PS4SDK nor PS4_PAYLOAD_SDK is defined)
endif

# Output properties mapping
TARGET   := pup-installer
PAYLOAD  := ez.bin

# Discovers source configurations automatically
SRCS := $(wildcard *.c)
HDRS := $(wildcard *.h)

CFLAGS := -Wall -Wextra -O2 -g

all: $(PAYLOAD)

# Compiles discovered items directly in the workspace directory
$(PAYLOAD): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(SRCS) -o $@

clean:
	-rm -f *.o *.elf *.bin

test: $(PAYLOAD)
	echo "$(USER)" | $(PS4_DEPLOY) -i -h $(PS4_HOST) -p $(PS4_PORT) $^

.PHONY: all clean test
