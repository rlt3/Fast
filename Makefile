CC         := clang++

LIBRARIES  :=-I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -std=c++11 -stdlib=libc++ 
FRAMEWORKS :=-framework Cocoa -framework SDL2

SRCDIR     := src
SOURCES    := src/main.cpp src/Coordinate.cpp src/Movement.cpp src/Polygon.cpp

OBJDIR     := obj
OBJECTS    := ${SOURCES:${SRCDIR}/%.cpp=${OBJDIR}/%.o}

EXECUTABLE := fast

all: ${SOURCES} ${EXECUTABLE}

${OBJECTS}: ${OBJDIR}/%.o : ${SRCDIR}/%.cpp
	$(CC) -c $< -o $@ ${LIBRARIES}

${EXECUTABLE}: ${OBJECTS}
	${CC} -Wall -o $@ ${OBJECTS} ${LIBRARIES} ${FRAMEWORKS}

ray: 
	gcc -o ray convert.c polygon.c vertex.c -framework SDL2

clean:
	rm -f ${OBJDIR}/*.o fast
