CC = gcc
CFLAGS = -lcurl
SOURCE_DIR = src
TARGET = cwfuzz
BUILD_DIR = $(SOURCE_DIR)

all: $(TARGET)

$(TARGET): $(SOURCE_DIR)/$(TARGET).c
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $<

install: $(TARGET)
	cp $(BUILD_DIR)/$(TARGET) /usr/bin

clean:
	rm -f $(BUILD_DIR)/$(TARGET)
