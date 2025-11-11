LIB := $(shell pkg-config --libs raylib)
CFLAGS := $(shell pkg-config --cflags raylib)
CC = cc
APPNAME = app

build:
	$(CC) -arch arm64 -o $(APPNAME) ./src/*.c ./src/*/*.c $(LIB) $(CFLAGS) -Wall -Werror

run:
	./$(APPNAME)

br: build run

clean:
	rm $(APPNAME)
