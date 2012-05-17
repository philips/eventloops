OS_NAME=$(shell uname -s)
LIBS+=-lm -ldl -lpthread
ifeq (${OS_NAME},Darwin)
all: osx
else ifeq (${OS_NAME},Linux)
all: epoll
endif

libuv/uv.a:
	$(MAKE) -C libuv


osx.o: osx.c
	$(CC) osx.c  -c -O -o osx.o

osx: osx.o libuv/uv.a
	$(CC) -all_load -foce_load -ObjC -framework CoreFoundation -framework CoreServices osx.o libuv/uv.a -o osx

epoll: epoll.c

clean:
	rm -f osx osx.o
	$(MAKE) -C libuv clean
