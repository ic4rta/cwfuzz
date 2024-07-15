CC = gcc
CFLAGS = -lcurl -Wall -O2

TARGET = cfuzz

all: $(TARGET)

$(TARGET): cfuzz.c
	$(CC) $(CFLAGS) -o $@ $<

install: $(TARGET)
	cp $(TARGET) /usr/bin

clean:
	rm -f $(TARGET)
