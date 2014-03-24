CC=g++
CFLAGS=-c
SOURCES=main.cpp Coordinate.cpp Vector.cpp Entity.cpp
SDLLIBS=-framework SDL2
FRAMEWORKS=-framework Cocoa
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=fast

all: $(SOURCES) $(EXECUTABLE)
		
$(EXECUTABLE): $(OBJECTS) 
	$(CC) -Wall -o $@ $(OBJECTS) $(SDLLIBS) $(FRAMEWORKS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
		rm -rf */*.o *.o fast
