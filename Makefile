OS_NAME=$(shell uname -s)
CFLAGS=-I libuv/include
LIBS+=-lm -ldl -lpthread
ifeq (${OS_NAME},Darwin)
LDFLAGS+=-all_load -foce_load -ObjC -framework CoreFoundation -framework CoreServices
all: osx
else ifeq (${OS_NAME},Linux)
all: epoll
endif

libuv/uv.a:
	$(MAKE) -C libuv


osx.o: osx.c
	$(CC) osx.c  -c -O -o osx.o

osx: osx.o libuv/uv.a
	$(CC) $(CFLAGS) osx.o $(LDFLAGS) libuv/uv.a -o osx

epoll: epoll.c

clean:
	rm -f osx osx.o
	$(MAKE) -C libuv clean
