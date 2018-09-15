.SUFFIXES:
.PHONY: tags clean

#CFLAGS=-g -O0 -Wall -Wformat -I/usr/include/ImageMagick-6 -I/usr/include/arm-linux-gnueabihf/ImageMagick-6/
# sudo apt-get install libmagickcore-dev
CFLAGS=-g -O0 -Wall -Wformat `pkg-config --cflags cairomm-1.0` -std=c++11
CFLAGS_DEP=-MMD -MF $(dir $@).$(notdir $(basename $@)).d

CPP=g++
LDFLAGS=-g -lpthread -lstdc++ -lrt -rdynamic `pkg-config --libs cairomm-1.0`

all: main


clean:
	rm -f *.o .*.d Makefile.config

%.o : %.cpp %.h
	${CPP} ${CFLAGS_DEP} ${CFLAGS} -c -o $@  $<

main: main.o Framebuffer.o utils.o
	${CPP} ${LDFLAGS} $+ -o main
