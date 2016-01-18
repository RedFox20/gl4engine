LIBSRCS = $(wildcard src/*.c)
LIBOBJS = $(LIBSRCS:src/%.c=obj/%.o)
LIBOUT  = bin/gl4e.a
CFLAGS  = -DGLEW_STATIC -DFREEGLUT_STATIC -m32 -march=native -mfpmath=sse -std=gnu11 -Iinclude/ -I. -IGL/ -Wall -Wno-unused-result -Wno-missing-braces
ifeq ($(OS),Windows_NT)
 SAMPLE = example1.exe
 FORMAT = win32
 OPENGL = GL/libglfw3-mingw32.a GL/libfreetype-mingw32.a GL/libglew.a GL/libsoil.a 
 SYSLIB = -lwinmm -lgdi32 -lmsvcrt -lopengl32 
else
 SAMPLE = example1
 FORMAT = elf32
 OPENGL = GL/libglfw3-linux32.a GL/libfreetype-linux32.a GL/libglew.a GL/libsoil.a
 SYSLIB = -lGL
endif

all: release
release: CFLAGS += -g -DNDEBUG=1 -O3
release: $(LIBOUT)
debug:   CFLAGS += -g -DDEBUG=1 -O1
debug:   $(LIBOUT)
example1: bin/$(SAMPLE)
clean:
	@rm -rf ./obj ./$(LIBOUT) ./bin/$(SAMPLE)
libs: GL/libglew.a GL/libsoil.a
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
	
# declare autodeps
-include obj/*.d

#######################################################################
## Examples
bin/$(SAMPLE): $(LIBOUT) example1/example1.c
	@echo " gcc c11 native32  example1.c"
	@gcc $(CFLAGS) -c example1/example1.c -o obj/example1.o -MD
	@echo link bin/$(SAMPLE)
	@gcc -m32 -o bin/$(SAMPLE) obj/example1.o $(LIBOUT) $(SYSLIB)

#######################################################################
## gl4e.a - A flat static library, with all the deps inside.
$(LIBOUT): obj GL/libglew.a GL/libsoil.a $(LIBOBJS)
	@echo "  ar $(LIBOUT)    OpenGL 4.3 Library"
	@echo "CREATE $(LIBOUT)" > obj/gl4e.mri
	@for a in $(OPENGL);  do (echo "ADDLIB $$a" >> obj/gl4e.mri); done
	@for a in $(LIBOBJS); do (echo "ADDMOD $$a" >> obj/gl4e.mri); done
	@echo "SAVE" >> obj/gl4e.mri
	@echo "END"  >> obj/gl4e.mri
	@ar -M < obj/gl4e.mri
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
	@echo " gcc libglew.a   OpenGL Extension Wrangler"
	@gcc $(CFLAGS) -c GL/glew.c -o obj/glew.o
	@ar rcs GL/libglew.a obj/glew.o

#######################################################################
## SOIL
SOILSRC = $(wildcard GL/SOIL/*.c)
SOILOBJ = $(SOILSRC:GL/SOIL/%.c=obj/SOIL/%.o)
obj/%.o: GL/SOIL/%.c GL/SOIL/%.h
	@gcc $(CFLAGS) -Wno-unused-but-set-variable -c GL/SOIL/$*.c -o obj/SOIL/$*.o -MD
.PHONY: libsoil libsoilcl
libsoil:
	@echo " gcc libsoil.a   Simple OpenGL Image Library"
libsoilcl: $(SOILOBJ)
GL/libsoil.a: libsoil $(SOILOBJ)
#@libsoilcl
	@ar rcs GL/libsoil.a $(SOILOBJ)
