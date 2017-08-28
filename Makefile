XC = "/opt/microchip/xc8/v1.43/bin/xc8"

TARGET_DEVICE = 16F1938
TARGET_DIR = bin
TARGET_NAME = target
TARGET = $(TARGET_DIR)/$(TARGET_NAME).hex

# Source-file directory
SRC_DIR = Src
SRC = $(wildcard $(SRC_DIR)/*.c)

# Header-file directory
INCLUDE_DIR = Inc
INCLUDE = $(wildcard $(INCLUDE_DIR)/*.h)

# Object-file directory
OBJ_DIR = obj
OBJ = $(addprefix $(OBJ_DIR)/,$(notdir $(SRC:%.c=%.p1)))

# Warning levl
WARN_LEVEL = -3

XC_COMPILE_OPTIONS = --pass1 --chip=$(TARGET_DEVICE) -I$(INCLUDE_DIR) -Q -P --asmlist --opt=default --addrqual=ignore
XC_LINK_OPTIONS = --chip=$(TARGET_DEVICE) --warn=$(WARN_LEVEL) --runtime=default,-keep,-osccal --output=default,-inhx032 --summary=default

.PHONY: all clean

all:  $(TARGET)

clean:
	rm -rf $(TARGET_DIR) $(OBJ_DIR)

# Build step: Compile
$(OBJ_DIR)/%.p1:  $(SRC_DIR)/%.c $(INCLUDE)
	mkdir -p $(OBJ_DIR)
	$(XC) $(XC_COMPILE_OPTIONS) -o$@ $<

# Build step: Link
$(TARGET):  $(OBJ)
	mkdir -p $(TARGET_DIR)
	$(XC) $(XC_LINK_OPTIONS) -o$@ $^
