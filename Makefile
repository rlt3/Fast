CC         := clang

LIBRARIES  :=-I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks
FRAMEWORKS :=-framework Cocoa -framework SDL2 -framework OpenGL

SRCDIR     := src
SOURCES    := src/main.c src/vertex.c src/polygon.c

OBJDIR     := obj
OBJECTS    := ${SOURCES:${SRCDIR}/%.cpp=${OBJDIR}/%.o}

EXECUTABLE := fast

all: ${SOURCES} ${EXECUTABLE}

${OBJECTS}: ${OBJDIR}/%.o : ${SRCDIR}/%.cpp
	$(CC) -c $< -o $@ ${LIBRARIES}

${EXECUTABLE}: ${OBJECTS}
	${CC} -Wall -o $@ ${OBJECTS} ${LIBRARIES} ${FRAMEWORKS}

open:
	clang -o test open.c vertex.c polygon.c -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2 -framework OpenGL

clean:
	rm -f ${OBJDIR}/*.o fast
