CC := g++
BINFLAGS := -std=c++11 -Wall -O3
CFLAGS := $(BINFLAGS) -c -flto

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

COMPONENTS := $(PROOT)/components

deploy:
	@if [ ! -d $(LIBPATH) ]; \
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
	cd $(COMPONENTS) && $(MAKE) libindexer.a

sorter.o: $(ROOT)/sorter.h $(ROOT)/sorter.cc
	$(CC) $(CFLAGS) $(ROOT)/sorter.cc -o $(OBJPATH)/sorter.o

libsorter.a: sorter.o dependencies
	$(AR) $(ARFLAGS) $(LIBPATH)/libsorter.a $(UTIL)/obj/tuple.o \
																					$(UTIL)/obj/file_manager.o \
																					$(COMPONENTS)/obj/indexer.o \
																					$(COMPONENTS)/obj/compressor.o \
																					$(OBJPATH)/sorter.o

clean:
	@rm -rf $(LIBPATH)
	@rm -rf $(OBJPATH)
	@rm -rf $(BINPATH)
	@rm -rf $(DATAPATH)

bin: libsorter.a $(SRCPATH)/external_sorter.cc $(SRCPATH)/test_tuple_generator.cc $(SRCPATH)/emplace_partial_sorter.cc
	$(CC) $(BINFLAGS) $(SRCPATH)/external_sorter.cc $(LIBPATH)/libsorter.a -o $(BINPATH)/external_sorter
	$(CC) $(BINFLAGS) $(SRCPATH)/test_tuple_generator.cc $(LIBPATH)/libsorter.a -o $(BINPATH)/test_tuple_generator
	$(CC) $(BINFLAGS) $(SRCPATH)/emplace_partial_sorter.cc $(LIBPATH)/libsorter.a -o $(BINPATH)/emplace_partial_sorter
