# Define variables
SRC_DIR = override
LKH_DIR = lkh
INCLUDE_DIR = $(LKH_DIR)/INCLUDE
LIB_NAME = liblkh.a

# Define source files
C_SOURCES = $(wildcard $(SRC_DIR)/*.c)
H_SOURCES = $(wildcard $(SRC_DIR)/*.h)

# Define all .c files in lkh directory
ALL_C_SOURCES = $(wildcard $(LKH_DIR)/*.c)

# Define object files
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(LKH_DIR)/%.o, $(C_SOURCES)) $(patsubst $(LKH_DIR)/%.c, $(LKH_DIR)/%.o, $(ALL_C_SOURCES))

# Default target
all: $(LIB_NAME)

# Copy header files to INCLUDE directory
$(INCLUDE_DIR)/%.h: $(SRC_DIR)/%.h
	@$(CP) $< $@

# Copy source files to lkh directory
$(LKH_DIR)/%.c: $(SRC_DIR)/%.c
	@$(CP) $< $@

# Ensure headers are copied before building object files
$(LKH_DIR)/%.o: $(LKH_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

# Create static library
$(LIB_NAME): $(C_OBJECTS)
	ar rcs $@ $(C_OBJECTS)
#	@$(RM) $(C_OBJECTS)

# Clean target
clean:
	$(RM) $(LKH_DIR)/*.o $(LIB_NAME)

# Phony targets
.PHONY: all clean

# Compiler and flags
CC = gcc
CFLAGS = -I$(INCLUDE_DIR)

# Cross-platform cp command
CP = cp
ifeq ($(OS),Windows_NT)
CP = copy
endif

# Cross-platform rm command
RM = rm -rf
ifeq ($(OS),Windows_NT)
RM = del /F /Q
endif