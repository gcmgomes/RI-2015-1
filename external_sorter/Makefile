CC := g++
BINFLAGS := -std=c++11 -Wall -g
CFLAGS := $(BINFLAGS) -c

AR := ar
ARFLAGS := rcs

ROOT := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
PROOT := $(realpath $(dir $(ROOT)))

LIBPATH := $(ROOT)/lib

OBJPATH := $(ROOT)/obj

SRCPATH := $(ROOT)/src

BINPATH := $(ROOT)/bin

DATAPATH := $(ROOT)/data

UTIL := $(PROOT)/util

deploy:
	if [ ! -d $(LIBPATH) ]; \
		then \
			mkdir $(LIBPATH);  \
			mkdir $(OBJPATH);  \
			mkdir $(BINPATH);  \
			mkdir $(DATAPATH); \
		else \
			echo "Directories exist"; \
	fi


dependencies:
	cd $(UTIL) && $(MAKE) libfilemanager.a

sorter.o: $(ROOT)/sorter.h $(ROOT)/sorter.cc
	$(CC) $(CFLAGS) $(ROOT)/sorter.cc -o $(OBJPATH)/sorter.o

libsorter.a: sorter.o dependencies
	$(AR) $(ARFLAGS) $(LIBPATH)/libsorter.a $(UTIL)/obj/tuple.o \
																					$(UTIL)/obj/file_manager.o \
																					$(UTIL)/obj/indexer.o \
																					$(OBJPATH)/sorter.o

clean:
	rm $(LIBPATH)/*.a
	rm $(OBJPATH)/*.o
	cd $(SRCPATH) && rm sorter gen

test: $(SRCPATH)/tuple_sorter.cc $(SRCPATH)/test_gen.cc libsorter.a
	$(CC) $(BINFLAGS) $(SRCPATH)/tuple_sorter.cc $(LIBPATH)/libsorter.a -o $(BINPATH)/external_sorter
	$(CC) $(BINFLAGS) $(SRCPATH)/tuple_gen.cc $(LIBPATH)/libsorter.a -o $(BINPATH)/tuple_gen
	$(CC) $(BINFLAGS) $(SRCPATH)/emplace_partial_sorter.cc $(LIBPATH)/libsorter.a -o $(BINPATH)/emplace_partial_sorter
