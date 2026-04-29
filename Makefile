LIB := $(shell pkg-config --libs raylib)
CFLAGS := $(shell pkg-config --cflags raylib) -Ilib
CC = cc
APPNAME = app

RAYLIB_WEB ?= vendor/raylib-web
WEB_OUT := web
WEB_APP := mwpc

build:
	$(CC) -arch arm64 -o $(APPNAME) ./src/*.c ./src/*/*.c $(LIB) $(CFLAGS) -Wall -Werror

run:
	./$(APPNAME)

br: build run

web:
	mkdir -p $(WEB_OUT)
	emcc -o $(WEB_OUT)/$(WEB_APP).html ./src/*.c ./src/*/*.c \
		$(RAYLIB_WEB)/libraylib.a \
		-I$(RAYLIB_WEB) -Ilib \
		-DPLATFORM_WEB \
		-Os -Wall \
		-s USE_GLFW=3 \
		-s ALLOW_MEMORY_GROWTH=1 \
		--shell-file $(RAYLIB_WEB)/minshell.html \
		--preload-file textures

wr:
	make web
	cd $(WEB_OUT) && uv run python -m http.server 8080

clean:
	rm $(APPNAME)

.PHONY: build run br web clean
