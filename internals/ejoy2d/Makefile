.PHONY : mingw ej2d linux undefined

CFLAGS = -g -Wall -Ilib -Ilib/render -Ilua -D EJOY2D_OS=$(OS) -D FONT_EDGE_HASH
LDFLAGS :=

RENDER := \
lib/render/render.c \
lib/render/carray.c \
lib/render/log.c

EJOY2D := \
lib/shader.c \
lib/lshader.c \
lib/ejoy2dgame.c \
lib/fault.c \
lib/screen.c \
lib/texture.c \
lib/ppm.c \
lib/spritepack.c \
lib/sprite.c \
lib/lsprite.c \
lib/matrix.c \
lib/lmatrix.c \
lib/dfont.c \
lib/label.c \
lib/particle.c \
lib/lparticle.c \
lib/scissor.c \
lib/renderbuffer.c \
lib/lrenderbuffer.c \
lib/lgeometry.c

SRC := $(EJOY2D) $(RENDER)

LUASRC := \
lua/lapi.c \
lua/lauxlib.c \
lua/lbaselib.c \
lua/lbitlib.c \
lua/lcode.c \
lua/lcorolib.c \
lua/lctype.c \
lua/ldblib.c \
lua/ldebug.c \
lua/ldo.c \
lua/ldump.c \
lua/lfunc.c \
lua/lgc.c \
lua/linit.c \
lua/liolib.c \
lua/llex.c \
lua/lmathlib.c \
lua/lmem.c \
lua/loadlib.c \
lua/lobject.c \
lua/lopcodes.c \
lua/loslib.c \
lua/lparser.c \
lua/lstate.c \
lua/lstring.c \
lua/lstrlib.c \
lua/ltable.c \
lua/ltablib.c \
lua/ltm.c \
lua/lundump.c \
lua/lutf8lib.c \
lua/lvm.c \
lua/lzio.c

CC=gcc
UNAME=$(shell uname)
SYS=$(if $(filter Linux%,$(UNAME)),linux,\
	    $(if $(filter MINGW%,$(UNAME)),mingw,\
	    $(if $(filter Darwin%,$(UNAME)),macosx,\
	        undefined\
)))

all: $(SYS)

undefined:
	@echo "I can't guess your platform, please do 'make PLATFORM' where PLATFORM is one of these:"
	@echo "      linux mingw macosx"


mingw : OS := WINDOWS
mingw : TARGET := ej2d.exe
mingw : CFLAGS += -I/usr/include
mingw : LDFLAGS += -L/usr/bin -lgdi32 -lglew32 -lopengl32
mingw : SRC += mingw/window.c mingw/winfw.c mingw/winfont.c

mingw : $(SRC) ej2d

winlib : OS := WINDOWS
winlib : TARGET := ejoy2d.dll
winlib : CFLAGS += -I/usr/include -I/usr/local/include --shared
winlib : LDFLAGS += -L/usr/bin -lgdi32 -lglew32 -lopengl32 -L/usr/local/bin -llua53
winlib : SRC += mingw/winfont.c lib/lejoy2dcore.c

winlib : $(SRC) ej2dlib

ej2dlib :
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

linux : OS := LINUX
linux : TARGET := ej2d
linux : CFLAGS += -I/usr/include $(shell freetype-config --cflags)
linux : LDFLAGS +=  -lGLEW -lGL -lX11 -lfreetype -lm
linux : SRC += posix/window.c posix/winfw.c posix/winfont.c

linux : $(SRC) ej2d

macosx : CC := clang
macosx : OS := MACOSX
macosx : TARGET := ej2d
macosx : CFLAGS += -I/usr/include $(shell freetype-config --cflags) -D __MACOSX
macosx : LDFLAGS += -lglfw3  -framework OpenGL -lfreetype -lm -ldl
macosx : SRC += mac/example/example/window.c posix/winfw.c mac/example/example/winfont.c

macosx : $(SRC) ej2d

ej2d :
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LUASRC) $(LDFLAGS)

clean :
	-rm -f ej2d.exe
	-rm -f ej2d
