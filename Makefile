CC = gcc
CFLAGS = -lcurl
SOURCE_DIR = src
TARGET = cwfuzz
BUILD_DIR = bin

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(SOURCE_DIR)/$(TARGET).c $(SOURCE_DIR)/include/cwfuzz.h
	$(CC) $(CFLAGS) -o $@ $<

install: $(BUILD_DIR)/$(TARGET)
	cp $< /usr/bin

clean:
	rm -f $(BUILD_DIR)/$(TARGET)
