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

RANKING := $(PROOT)/ranking

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
	cd $(RANKING) && $(MAKE) librankingmetadata.a
	cd $(UTIL) && $(MAKE) libtuple.a
	cd $(PARSER) && $(MAKE) libvocabulary.a

bin_dependencies:
	cd $(RANKING) && $(MAKE) libranker.a

indexer.o: $(ROOT)/indexer.h $(ROOT)/indexer.cc
	$(CC) $(CFLAGS) $(ROOT)/indexer.cc -o $(OBJPATH)/indexer.o

libindexer.a: dependencies indexer.o compressor.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libindexer.a $(OBJPATH)/indexer.o $(UTIL)/obj/tuple.o $(OBJPATH)/compressor.o

query_processor.o: $(ROOT)/query_processor.h $(ROOT)/query_processor.cc
	$(CC) $(CFLAGS) $(ROOT)/query_processor.cc -o $(OBJPATH)/query_processor.o

libqueryprocessor.a: dependencies query_processor.o indexer.o compressor.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libqueryprocessor.a $(OBJPATH)/query_processor.o $(UTIL)/obj/tuple.o $(PARSER)/obj/vocabulary.o $(OBJPATH)/indexer.o $(OBJPATH)/compressor.o

compressor.o: $(ROOT)/compressor.h $(ROOT)/compressor.cc
	$(CC) $(CFLAGS) $(ROOT)/compressor.cc -o $(OBJPATH)/compressor.o

libcompressor.a: compressor.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libcompressor.a $(OBJPATH)/compressor.o

retriever.o: $(ROOT)/retriever.h $(ROOT)/retriever.cc
	$(CC) $(CFLAGS) $(ROOT)/retriever.cc -o $(OBJPATH)/retriever.o

libretriever.a: dependencies retriever.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libretriever.a $(OBJPATH)/retriever.o $(UTIL)/obj/page.o $(RANKING)/obj/ranking_metadata.o $(PARSER)/obj/vocabulary.o $(OBJPATH)/indexer.o $(OBJPATH)/compressor.o


clean:
	@rm -rf $(LIBPATH)
	@rm -rf $(OBJPATH)
	@rm -rf $(BINPATH)
	@rm -rf $(DATAPATH)

bin: bin_dependencies libqueryprocessor.a $(SRCPATH)/query_index.cc libcompressor.a libretriever.a
	$(CC) $(BINFLAGS) $(SRCPATH)/query_index.cc $(LIBPATH)/libqueryprocessor.a -o $(BINPATH)/query_index
	$(CC) $(BINFLAGS) $(SRCPATH)/compressor.cc $(LIBPATH)/libcompressor.a -o $(BINPATH)/compressor
	$(CC) $(BINFLAGS) $(SRCPATH)/retrieve_index.cc $(LIBPATH)/libretriever.a $(RANKING)/lib/libranker.a -o $(BINPATH)/retrieve_index
