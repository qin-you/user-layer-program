SRC = ping.c
TARGET  = ping
CCFLAGS = -o ${TARGET} -lpthread

all:${TARGET}

${TARGET}:${SRC}
	gcc  $< ${CCFLAGS}

clean:
	rm ${TARGET}