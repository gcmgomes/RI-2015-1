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

UTIL := $(PROOT)/util

PARSER := $(PROOT)/parser

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
	cd $(UTIL) && $(MAKE) libtuple.a
	cd $(PARSER) && $(MAKE) libvocabulary.a

indexer.o: $(ROOT)/indexer.h $(ROOT)/indexer.cc
	$(CC) $(CFLAGS) $(ROOT)/indexer.cc -o $(OBJPATH)/indexer.o

libindexer.a: dependencies indexer.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libindexer.a $(OBJPATH)/indexer.o $(UTIL)/obj/tuple.o

query_processor.o: $(ROOT)/query_processor.h $(ROOT)/query_processor.cc
	$(CC) $(CFLAGS) $(ROOT)/query_processor.cc -o $(OBJPATH)/query_processor.o

libqueryprocessor.a: dependencies query_processor.o indexer.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libqueryprocessor.a $(OBJPATH)/query_processor.o $(UTIL)/obj/tuple.o $(PARSER)/obj/vocabulary.o $(OBJPATH)/indexer.o

clean:
	@rm -rf $(LIBPATH)
	@rm -rf $(OBJPATH)
	@rm -rf $(BINPATH)
	@rm -rf $(DATAPATH)

bin: libqueryprocessor.a  $(SRCPATH)/query_index.cc
	$(CC) $(BINFLAGS) $(SRCPATH)/query_index.cc $(LIBPATH)/libqueryprocessor.a -o $(BINPATH)/query_index