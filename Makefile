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

lambda:
	clang++ -std=c++11 -stdlib=libc++ -framework SDL2 lambda.cpp -o test

clean:
		rm -rf */*.o *.o fast test
