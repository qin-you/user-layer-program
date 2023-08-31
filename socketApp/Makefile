SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, ${SRC})
TARGET = $(patsubst %.c, %, ${SRC})

CC = gcc

.PHONY:all clean

all:${TARGET}

%.o:%.c
	${CC} -c $< -o %@

%:%.o
	${CC} $< -o $@

clean:
	#-rm ${OBJ}
	-rm ${TARGET}
