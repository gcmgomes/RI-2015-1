CC := g++
BINFLAGS := -std=c++11 -Wall -O3
LINKFLAGS := -lz -lgumbo
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

COMPONENTS := $(PROOT)/components

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

dependencies1:
	cd $(COMPONENTS) && $(MAKE) libretriever.a

dependencies2:
	cd $(UTIL) && $(MAKE) page.o

ranking_metadata.o: ${ROOT}/ranking_metadata.h ${ROOT}/ranking_metadata.cc
	$(CC) $(CFLAGS) $(ROOT)/ranking_metadata.cc -o $(OBJPATH)/ranking_metadata.o

librankingmetadata.a: dependencies2 ranking_metadata.o
	$(AR) $(ARFLAGS) $(LIBPATH)/librankingmetadata.a $(OBJPATH)/ranking_metadata.o $(UTIL)/obj/page.o

ranker.o: ${ROOT}/ranker.h ${ROOT}/ranker.cc
	$(CC) $(CFLAGS) $(ROOT)/ranker.cc -o $(OBJPATH)/ranker.o

libranker.a: dependencies1 ranker.o librankingmetadata.a
	$(AR) $(ARFLAGS) $(LIBPATH)/libranker.a $(OBJPATH)/ranker.o $(COMPONENTS)/obj/retriever.o $(UTIL)/obj/page.o $(OBJPATH)/ranking_metadata.o $(PARSER)/obj/vocabulary.o $(COMPONENTS)/obj/indexer.o $(COMPONENTS)/obj/compressor.o

clean:
	@rm -rf $(LIBPATH)
	@rm -rf $(OBJPATH)
	@rm -rf $(BINPATH)
	@rm -rf $(DATAPATH)
	@rm -rf $(ROOT)/ricp/build/

bin: libranker.a
	$(CC) $(BINFLAGS) $(SRCPATH)/check_metadata.cc $(LIBPATH)/librankingmetadata.a $(LINKFLAGS) -o $(BINPATH)/check_metadata