CC=cc
CFLAGS= -Wall -Wextra -L/usr/local/lib -lallegro -lallegro_main -lallegro_image -lallegro_dialog -lallegro_primitives

SOURCES:=$(shell echo *.c)
OBJECTS:=$(patsubst %.c,%.o,${SOURCES})
TARGETS:=$(patsubst %.c,%,${SOURCES})

all: $(TARGETS)

clean:
	rm -f -R $(OBJECTS) $(TARGETS) *.dSYM