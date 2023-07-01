src = $(wildcard ./*.c)
target = $(patsubst %.c,%,$(src))

ALL:$(target)

%:%.c
	gcc $< -o $@ -lpthread

clean:
	-rm -rf $(target)

PHONY:ALL clean
