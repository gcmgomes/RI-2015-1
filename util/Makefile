CC := g++
BINFLAGS := -std=c++11 -Wall -O3
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
	cd $(COMPONENTS) && $(MAKE) libindexer.a

tuple.o: $(ROOT)/tuple.h $(ROOT)/tuple.cc
	$(CC) $(CFLAGS) $(ROOT)/tuple.cc -o $(OBJPATH)/tuple.o

libtuple.a: tuple.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libtuple.a $(OBJPATH)/tuple.o

file_manager.o: libtuple.a $(ROOT)/file_manager.h $(ROOT)/file_manager.cc 
	$(CC) $(CFLAGS) $(ROOT)/file_manager.cc -o $(OBJPATH)/file_manager.o

libfilemanager.a: file_manager.o dependencies
	$(AR) $(ARFLAGS) $(LIBPATH)/libfilemanager.a $(OBJPATH)/file_manager.o $(OBJPATH)/tuple.o $(COMPONENTS)/obj/indexer.o

page.o: $(ROOT)/page.h $(ROOT)/page.cc
	$(CC) $(CFLAGS) $(ROOT)/page.cc -o $(OBJPATH)/page.o

libpage.a: page.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libpage.a $(OBJPATH)/page.o

clean:
	@rm -rf $(LIBPATH)
	@rm -rf $(OBJPATH)
	@rm -rf $(BINPATH)
	@rm -rf $(DATAPATH)

bin: libfilemanager.a  $(SRCPATH)/convert_to_human_readable.cc $(SRCPATH)/optimal_run_counter.cc
	$(CC) $(BINFLAGS) $(SRCPATH)/optimal_run_counter.cc $(LIBPATH)/libfilemanager.a -o $(BINPATH)/optimal_run_counter
	$(CC) $(BINFLAGS) $(SRCPATH)/convert_to_human_readable.cc $(LIBPATH)/libfilemanager.a -o $(BINPATH)/convert_to_human_readable
