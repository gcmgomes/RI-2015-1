CC := g++
CFLAGS := -Wall -g -c

AR := ar
ARFLAGS := rcs

ROOT := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

LIBPATH := $(ROOT)/lib

OBJPATH := $(ROOT)/obj

TESTPATH := $(ROOT)/test

tuple.o: $(ROOT)/tuple.h $(ROOT)/tuple.cc
	$(CC) $(CFLAGS) $(ROOT)/tuple.cc -o $(OBJPATH)/tuple.o

libtuple.a: tuple.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libtuple.a $(OBJPATH)/tuple.o

file_manager.o: libtuple.a $(ROOT)/file_manager.h $(ROOT)/file_manager.cc 
	$(CC) $(CFLAGS) $(ROOT)/file_manager.cc -o $(OBJPATH)/file_manager.o

libfilemanager.a: file_manager.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libfilemanager.a $(OBJPATH)/file_manager.o $(OBJPATH)/tuple.o

clean:
	rm $(LIBPATH)/*.a
	rm $(OBJPATH)/*.o
	rm $(TESTPATH)/*bin*

test: libfilemanager.a
	$(CC) -Wall -g $(LIBPATH)/main.cc $(LIBPATH)/libfilemanager.a -o $(LIBPATH)/x
