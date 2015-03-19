
all:
	gcc -Wall -Wextra -ggdb -o deadstrip src/*.c

doxygen:
	doxygen

clean:
	rm -rf docs deadstrip
