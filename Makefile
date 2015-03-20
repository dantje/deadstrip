CC=gcc
CFLAGS=-c -Wall -Wextra
LDFLAGS=
SOURCES=src/main.c src/graph.c src/hashmap.c src/list.c src/objectFile.c
OBJECTS=$(SOURCES:.c=.o)
TARGET=deadstrip

all: $(SOURCES) $(TARGET)

clean:
	rm -rf $(TARGET) $(OBJECTS) docs/html

doxygen:
	doxygen docs/Doxyfile

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
