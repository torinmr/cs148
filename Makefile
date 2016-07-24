CC=g++
CPPFLAGS=-c -Wall --std=c++11 $(shell pkg-config --cflags glfw3)
# For some reason, pkg-config outputs an empty "-l " at the beginning of the
# library list, which makes the linker choke, so I use sed to cut it out.
LDLIBS=-lGLEW -lGL -lglfw3 $(shell pkg-config --static --libs glfw3 | sed s/-l\ //) -lSOIL -lassimp

all: main

main: main.o shader_loader.o camera.o model.o mesh.o portal.o
	$(CC) main.o shader_loader.o camera.o mesh.o model.o portal.o -o main $(LDLIBS)

main.o: main.cpp
	$(CC) $(CPPFLAGS) main.cpp

portal.o: portal.cpp portal.h
	$(CC) $(CPPFLAGS) portal.cpp

model.o: model.cpp model.h
	$(CC) $(CPPFLAGS) model.cpp

mesh.o: mesh.cpp mesh.h
	$(CC) $(CPPFLAGS) mesh.cpp

camera.o: camera.cpp camera.h
	$(CC) $(CPPFLAGS) camera.cpp

shader_loader.o: shader_loader.cpp shader_loader.h
	$(CC) $(CPPFLAGS) shader_loader.cpp

clean:
	rm -f *.o main \#*
