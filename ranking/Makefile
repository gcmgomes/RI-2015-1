CC := g++
BINFLAGS := -std=c++11 -Wall -O3
LINKFLAGS := -lz -lgumbo -flto -lhtmlcxx
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
	cd $(UTIL) && $(MAKE) page.o && $(MAKE) page_url.o

ranking_metadata.o: ${ROOT}/ranking_metadata.h ${ROOT}/ranking_metadata.cc
	$(CC) $(CFLAGS) $(ROOT)/ranking_metadata.cc -o $(OBJPATH)/ranking_metadata.o

librankingmetadata.a: dependencies2 ranking_metadata.o
	$(AR) $(ARFLAGS) $(LIBPATH)/librankingmetadata.a $(OBJPATH)/ranking_metadata.o $(UTIL)/obj/page.o $(UTIL)/obj/page_url.o 

ranker.o: ${ROOT}/ranker.h ${ROOT}/ranker.cc
	$(CC) $(CFLAGS) $(ROOT)/ranker.cc -o $(OBJPATH)/ranker.o

libranker.a: dependencies1 ranker.o librankingmetadata.a
	$(AR) $(ARFLAGS) $(LIBPATH)/libranker.a $(OBJPATH)/ranker.o $(COMPONENTS)/obj/retriever.o $(UTIL)/obj/page.o $(UTIL)/obj/page_url.o $(OBJPATH)/ranking_metadata.o $(PARSER)/obj/vocabulary.o $(COMPONENTS)/obj/indexer.o $(COMPONENTS)/obj/compressor.o

page_ranker.o: ${ROOT}/page_ranker.h ${ROOT}/page_ranker.cc
	$(CC) $(CFLAGS) $(ROOT)/page_ranker.cc -o $(OBJPATH)/page_ranker.o

libpageranker.a: dependencies2 page_ranker.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libpageranker.a $(OBJPATH)/page_ranker.o $(UTIL)/obj/page.o $(UTIL)/obj/page_url.o $(OBJPATH)/ranking_metadata.o

ranking_preprocessor.o: ${ROOT}/ranking_preprocessor.h ${ROOT}/ranking_preprocessor.cc
	$(CC) $(CFLAGS) $(ROOT)/ranking_preprocessor.cc -o $(OBJPATH)/ranking_preprocessor.o

librankingpreprocessor.a: dependencies2 libpageranker.a	ranking_preprocessor.o vector_preprocessor.o
	$(AR) $(ARFLAGS) $(LIBPATH)/librankingpreprocessor.a  $(OBJPATH)/vector_preprocessor.o $(OBJPATH)/page_ranker.o $(UTIL)/obj/page.o $(UTIL)/obj/page_url.o $(OBJPATH)/ranking_metadata.o $(OBJPATH)/ranking_preprocessor.o  $(COMPONENTS)/obj/compressor.o $(COMPONENTS)/obj/indexer.o

vector_preprocessor.o: ${ROOT}/vector_preprocessor.h ${ROOT}/vector_preprocessor.cc
	$(CC) $(CFLAGS) $(ROOT)/vector_preprocessor.cc -o $(OBJPATH)/vector_preprocessor.o

libvectorpreprocessor.a: dependencies2 libpageranker.a	vector_preprocessor.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libvectorpreprocessor.a $(UTIL)/obj/page.o $(UTIL)/obj/page_url.o $(OBJPATH)/vector_preprocessor.o $(COMPONENTS)/obj/indexer.o 

query_evaluator.o: ${ROOT}/query_evaluator.h ${ROOT}/query_evaluator.cc
	$(CC) $(CFLAGS) $(ROOT)/query_evaluator.cc -o $(OBJPATH)/query_evaluator.o

libqueryevaluator.a: dependencies1 query_evaluator.o librankingmetadata.a
	$(AR) $(ARFLAGS) $(LIBPATH)/libqueryevaluator.a $(OBJPATH)/query_evaluator.o $(COMPONENTS)/obj/retriever.o $(UTIL)/obj/page.o $(UTIL)/obj/page_url.o $(OBJPATH)/ranking_metadata.o $(PARSER)/obj/vocabulary.o $(COMPONENTS)/obj/indexer.o $(COMPONENTS)/obj/compressor.o



clean:
	@rm -rf $(LIBPATH)
	@rm -rf $(OBJPATH)
	@rm -rf $(BINPATH)
	@rm -rf $(DATAPATH)
	@rm -rf $(ROOT)/ricp/build/

bin: libranker.a librankingpreprocessor.a
	$(CC) $(BINFLAGS) $(SRCPATH)/check_metadata.cc $(LIBPATH)/librankingmetadata.a $(LINKFLAGS) -o $(BINPATH)/check_metadata
	$(CC) $(BINFLAGS) $(SRCPATH)/preprocess_ranking_data.cc $(LIBPATH)/librankingpreprocessor.a $(LINKFLAGS) -o $(BINPATH)/preprocess_ranking_data
