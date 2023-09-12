SRC = $(wildcard *.c)
TARGET = $(patsubst %.c,%, ${SRC})

all:${TARGET}

${TARGET}:${SRC}
	gcc $< -o $@

clean:
	rm -f ${TARGET}
