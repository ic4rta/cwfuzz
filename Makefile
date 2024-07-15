CC = gcc
CFLAGS = -lcurl -Wall -O2

TARGET = cwfuzz

all: $(TARGET)

$(TARGET): cwfuzz.c
	$(CC) $(CFLAGS) -o $@ $<

install: $(TARGET)
	cp $(TARGET) /usr/bin

clean:
	rm -f $(TARGET)
