CC := g++
BINFLAGS := -std=c++11 -Wall -g
CFLAGS := $(BINFLAGS) -c

AR := ar
ARFLAGS := rcs

ROOT := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
PROOT := $(realpath $(dir $(ROOT)))

LIBPATH := $(ROOT)/lib

OBJPATH := $(ROOT)/obj

TESTPATH := $(ROOT)/test

DATAPATH := $(ROOT)/data

UTIL := $(PROOT)/util

dependencies:
	cd $(UTIL) && $(MAKE) libfilemanager.a

sorter.o: $(ROOT)/sorter.h $(ROOT)/sorter.cc
	$(CC) $(CFLAGS) $(ROOT)/sorter.cc -o $(OBJPATH)/sorter.o

libsorter.a: sorter.o dependencies
	$(AR) $(ARFLAGS) $(LIBPATH)/libsorter.a $(UTIL)/obj/tuple.o \
																					$(UTIL)/obj/file_manager.o \
																					$(OBJPATH)/sorter.o

clean:
	rm $(LIBPATH)/*.a
	rm $(OBJPATH)/*.o
	cd $(TESTPATH) && rm sorter gen

test: $(TESTPATH)/tuple_sorter.cc $(TESTPATH)/test_gen.cc libsorter.a
	$(CC) $(BINFLAGS) $(TESTPATH)/tuple_sorter.cc $(LIBPATH)/libsorter.a -o $(TESTPATH)/sorter
	$(CC) $(BINFLAGS) $(TESTPATH)/test_gen.cc $(LIBPATH)/libsorter.a -o $(TESTPATH)/gen
	$(CC) $(BINFLAGS) $(TESTPATH)/test_splitter.cc $(LIBPATH)/libsorter.a -o $(TESTPATH)/splitter
