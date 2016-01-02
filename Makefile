SRCS   = $(wildcard src/*.c)
OBJS   = $(SRCS:src/%.c=obj/%.o)
CFLAGS = -DGLEW_STATIC -m32 -march=native -mfpmath=sse -std=gnu11 -I. -IGL/ -Wall -Wno-unused-result -Wno-missing-braces
ifeq ($(OS),Windows_NT)
 OUT    = gl3engine.exe
 FORMAT = win32
 OPENGL = GL/libglfw3-mingw32.a GL/libfreetype-mingw32.a GL/libglew.a GL/libsoil.a -lopengl32 
 SYSLIB = -lwinmm -lgdi32 -lmsvcrt
else
 OUT    = gl3engine
 FORMAT = elf32
 OPENGL = GL/libglfw3-linux32.a GL/libfreetype-linux32.a GL/libglew.a GL/libsoil.a -lGL 
 SYSLIB = 
endif

all: debug
release: CFLAGS += -g -DNDEBUG=1 -O3
release: $(OUT)
debug:   CFLAGS += -g -DDEBUG=1 -O1
debug:   $(OUT)
clean:
	@rm -rf ./obj ./$(OUT)
run: all
	./$(OUT)
gdb: debug
	@gdb -q -x gdbinit $(OUT)
	
## make install - only on linux
ifeq ($(OS),Windows_NT)
install:
else
install:
	sudo apt-get install libgl1-mesa-dev:i386
	sudo apt-get install libglu1-mesa-dev:i386
	sudo apt-get install freeglut3-dev:i386
	sudo apt-get install libfreetype6-dev:i386
endif
	
# declare autodeps
-include obj/*.d

$(OUT): obj GL/libglew.a GL/libsoil.a $(OBJS)
	@echo link $(OUT)
	@gcc -m32 -o $(OUT) obj/*.o $(OPENGL) $(SYSLIB)
obj/%.o: src/%.c src/%.h
	@echo " gcc c11 native32  $*.c"
	@gcc $(CFLAGS) -c src/$*.c -o obj/$*.o -MD
obj:
	@mkdir obj
	
## GLEW 
GL/libglew.a:
	@echo " gcc libglew.a   OpenGL Extension Wrangler"
	@gcc $(CFLAGS) -c GL/glew.c -o obj/glew.o
	@ar rcs GL/libglew.a obj/glew.o

## SOIL
SOILSRC = $(wildcard GL/SOIL/*.c)
SOILOBJ = $(SOILSRC:GL/SOIL/%.c=obj/%.o)
obj/%.o: GL/SOIL/%.c GL/SOIL/%.h
	@gcc $(CFLAGS) -Wno-unused-but-set-variable -c GL/SOIL/$*.c -o obj/$*.o -MD
.PHONY: libsoil
libsoil:
	@echo " gcc libsoil.a   Simple OpenGL Image Library"
GL/libsoil.a: obj $(SOILOBJ)
	@ar rcs GL/libsoil.a $(SOILOBJ)
