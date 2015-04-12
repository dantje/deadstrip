CC=gcc
CFLAGS=-c -Wall -Wextra -ffunction-sections -fdata-sections -Wextra
LDFLAGS=
SOURCES=src/main.c src/graph.c src/hashmap.c src/list.c src/objectFile.c
OBJECTS=$(SOURCES:.c=.o)
TARGET=deadstrip

all: $(SOURCES) $(TARGET)

clean:
	rm -rf $(TARGET) $(OBJECTS) test 

doxygen:
	doxygen docs/Doxyfile

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

dogfood:
	mkdir -p test && cd test && i686-w64-mingw32-gcc $(CFLAGS) ../src/*.c
	./deadstrip --dmap -o test/deadstrip test/*.o -lmingw32 -lmoldname -lmingwex -lmsvcrt -ladvapi32 -lshell32 -luser32 -lkernel32 >map.xml
	python tographviz.py >test/deadstrip.gv
	dot -Tpdf test/deadstrip.gv -o test/deadstrip.pdf
