CC := g++
BINFLAGS := -std=c++11 -Wall -O3 -lhtmlcxx
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

RANKING := $(PROOT)/ranking

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
	cd $(RANKING) && $(MAKE) libranker.a

html_handler.o: $(ROOT)/html_handler.h $(ROOT)/html_handler.cc
	$(CC) $(CFLAGS) $(ROOT)/html_handler.cc -o $(OBJPATH)/html_handler.o

libhtmlhandler.a: html_handler.o dependencies
	$(AR) $(ARFLAGS) $(LIBPATH)/libhtmlhandler.a $(OBJPATH)/html_handler.o $(UTIL)/obj/page.o $(UTIL)/obj/page_url.o $(RANKING)/obj/ranking_metadata.o $(PARSER)/obj/vocabulary.o $(COMPONENTS)/obj/indexer.o $(COMPONENTS)/obj/compressor.o $(COMPONENTS)/obj/retriever.o $(RANKING)/obj/ranker.o


clean:
	@rm -rf $(LIBPATH)
	@rm -rf $(OBJPATH)
	@rm -rf $(BINPATH)
	@rm -rf $(DATAPATH)

bin: libhtmlhandler.a 
	$(CC) $(BINFLAGS) $(SRCPATH)/query_server.cc $(ROOT)/mongoose.c $(LIBPATH)/libhtmlhandler.a -o $(BINPATH)/query_server
	
