CC         := clang++

LIBRARIES  :=-std=c++11 -stdlib=libc++ 
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
	gcc -o ray main.c -framework Cocoa -framework SDL2

clean:
	rm -f ${OBJDIR}/*.o fast
