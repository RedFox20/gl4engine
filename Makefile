LIBSRCS = $(wildcard src/*.c)
LIBOBJS = $(LIBSRCS:src/%.c=obj/%.o)
LIBOUT  = bin/gl4e.a
CFLAGS  = -DGLEW_STATIC -DFREEGLUT_STATIC -m32 -march=native -msse3 -mavx -std=gnu11 -Iinclude/ -I. -IGL/ -Wall -Wno-unused-result -Wno-missing-braces
ifeq ($(OS),Windows_NT)
 SAMPLE = example1.exe
 OPENGL = GL/libglfw3-mingw32.a GL/libfreetype-mingw32.a GL/libglew.a GL/libsoil.a 
 SYSLIB = -lwinmm -lgdi32 -lmsvcrt -lopengl32 
else
 SAMPLE = example1
 OPENGL = GL/libglfw3-linux32.a GL/libfreetype-linux32.a GL/libglew.a GL/libsoil.a
 SYSLIB = -lGL
endif

#######################################################################
## Entry points for different build/launch options
all: release
release: CFLAGS += -g -DNDEBUG=1 -O3
release: $(LIBOUT)
debug:   CFLAGS += -g -DDEBUG=1 -O1
debug:   $(LIBOUT)
example1: bin/$(SAMPLE)
clean:
	@rm -rf ./obj ./$(LIBOUT) ./bin/$(SAMPLE)
libs: obj GL/libglew.a GL/libsoil.a
cleanlibs:
	@rm -rf ./GL/libglew.a ./GL/libsoil.a
run: release example1
	./$(SAMPLE)
gdb: debug
	@gdb -q -x gdbinit $(LIBOUT)

#######################################################################
## make install - only on linux
ifeq ($(OS),Windows_NT)
install:
else
install:
	sudo apt-get install libgl1-mesa-dev:i386
	sudo apt-get install libfreetype6-dev:i386
endif

#######################################################################
## Examples
bin/$(SAMPLE): $(LIBOUT) example1/example1.c
	@echo " gcc c11 native32  example1.c"
	@gcc $(CFLAGS) -c example1/example1.c -o obj/example1.o -MD
	@echo link bin/$(SAMPLE)
	@gcc -m32 -o bin/$(SAMPLE) obj/example1.o $(LIBOUT) $(SYSLIB)

#######################################################################
## gl4e.a - A flat static library, with all the deps inside.
##
$(LIBOUT): obj GL/libglew.a GL/libsoil.a $(LIBOBJS)
	@echo "  ar $(LIBOUT)    OpenGL 4.3 Library"
	@echo "CREATE $(LIBOUT)" > obj/gl4e.mri
	@for a in $(OPENGL);  do (echo "ADDLIB $$a" >> obj/gl4e.mri); done
	@for a in $(LIBOBJS); do (echo "ADDMOD $$a" >> obj/gl4e.mri); done
	@echo "SAVE" >> obj/gl4e.mri
	@echo "END"  >> obj/gl4e.mri
	@ar -M < obj/gl4e.mri
-include obj/*.d
obj/%.o: src/%.c include/%.h
	@echo " gcc c11 native32  $*.c"
	@gcc $(CFLAGS) -c src/$*.c -o obj/$*.o -MD
obj/%.o: src/%.c
	@echo " gcc c11 native32  $*.c"
	@gcc $(CFLAGS) -c src/$*.c -o obj/$*.o -MD
obj:
	@mkdir obj

#######################################################################
## GLEW 
GL/libglew.a:
	@echo " gcc libglew.a  O3  OpenGL Extension Wrangler"
	@gcc $(CFLAGS) -O3 -c GL/glew.c -o obj/libglew.o
	@ar rcs GL/libglew.a obj/libglew.o

#######################################################################
## SOIL
GL/libsoil.a:
	@echo " gcc libsoil.a  O3  Simple OpenGL Image Library"
	@gcc $(CFLAGS) -O3 -Wno-unused-but-set-variable GL/SOIL/SOIL_unified.c -c -o obj/libsoil.o
	@ar rcs GL/libsoil.a obj/libsoil.o
