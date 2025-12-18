# NES Tower of Hanoi Makefile
# Uses cc65 toolchain to compile C code to NES ROM

# Project name
PROJECT = hanoi

# Directories
SRC_DIR = src
BUILD_DIR = build

# Compiler and tools
CC = cc65
AS = ca65
LD = ld65

# Flags
CFLAGS = -Oi -t nes
ASFLAGS = -t nes
LDFLAGS = -C nes.cfg

# Source files
C_SOURCES = $(wildcard $(SRC_DIR)/*.c)
ASM_SOURCES = $(wildcard $(SRC_DIR)/*.s)

# Object files
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS = $(patsubst $(SRC_DIR)/%.s,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# Target NES ROM
TARGET = $(BUILD_DIR)/$(PROJECT).nes

.PHONY: all clean

all: $(TARGET)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile C sources to assembly
$(BUILD_DIR)/%.s: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $<

# Assemble to object files
$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.s
	$(AS) $(ASFLAGS) -o $@ $<

# Assemble .s files directly to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s | $(BUILD_DIR)
	$(AS) $(ASFLAGS) -o $@ $<

# Link to create NES ROM
$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) nes.lib

clean:
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	@echo "Run with your favorite NES emulator:"
	@echo "  fceux $(TARGET)"
	@echo "  or"
	@echo "  nestopia $(TARGET)"
