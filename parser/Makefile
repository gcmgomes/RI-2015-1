CC := g++
BINFLAGS := -std=c++11 -Wall -O3
LINKFLAGS := -lz -lgumbo -flto -lhtmlcxx
CFLAGS := $(BINFLAGS) -c

AR := ar
ARFLAGS := rcs

ROOT := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
PROOT := $(realpath $(dir $(ROOT)))

UTILPATH := $(ROOT)/util

LIBPATH := $(ROOT)/lib

OBJPATH := $(ROOT)/obj

SRCPATH := $(ROOT)/src

BINPATH := $(ROOT)/bin

DATAPATH := $(ROOT)/data

RANKINGPATH := $(PROOT)/ranking

GUTIL := $(PROOT)/util

COMPONENTS := $(PROOT)/components

RICPLIB := $(ROOT)/ricp/build/src/libRICPlib.a

deploy:
	@if [ ! -d $(LIBPATH) ]; \
		then \
			mkdir $(LIBPATH);  \
			mkdir $(OBJPATH);  \
			mkdir $(BINPATH);  \
			mkdir $(DATAPATH); \
			mkdir $(ROOT)/ricp/build; \
		else \
			echo "Directories exist"; \
	fi

dependencies:
	cd $(RANKINGPATH) && $(MAKE) ranking_metadata.o
	cd $(GUTIL) && $(MAKE) page.o && $(MAKE) libfilemanager.a
	cd $(COMPONENTS) && $(MAKE) libindexer.a
	cd $(ROOT)/ricp/build && cmake .. && $(MAKE)

htmlEntities.o: $(UTILPATH)/htmlEntities.h $(UTILPATH)/htmlEntities.cc
	$(CC) $(CFLAGS) $(UTILPATH)/htmlEntities.cc -o $(OBJPATH)/htmlEntities.o

libhtmlEntities.a: htmlEntities.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libhtmlEntities.a $(OBJPATH)/htmlEntities.o

page_knowledge.o: $(ROOT)/page_knowledge.h $(ROOT)/page_knowledge.cc
	$(CC) $(CFLAGS) $(ROOT)/page_knowledge.cc -o $(OBJPATH)/page_knowledge.o

parser.o: $(ROOT)/parser.h $(ROOT)/parser.cc $(UTILPATH)/text_processing.h 
	$(CC) $(CFLAGS) $(ROOT)/parser.cc -o $(OBJPATH)/parser.o

libparser.a: dependencies parser.o libhtmlEntities.a page_knowledge.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libparser.a $(OBJPATH)/parser.o $(GUTIL)/obj/page.o $(GUTIL)/obj/page_url.o $(OBJPATH)/htmlEntities.o $(GUTIL)/obj/file_manager.o  $(GUTIL)/obj/tuple.o $(COMPONENTS)/obj/indexer.o	$(COMPONENTS)/obj/compressor.o $(RANKINGPATH)/obj/ranking_metadata.o $(OBJPATH)/page_knowledge.o

vocabulary.o: ${ROOT}/vocabulary.h ${ROOT}/vocabulary.cc
	$(CC) $(CFLAGS) $(ROOT)/vocabulary.cc -o $(OBJPATH)/vocabulary.o

libvocabulary.a: vocabulary.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libvocabulary.a $(OBJPATH)/vocabulary.o $(GUTIL)/obj/page.o $(GUTIL)/obj/page_url.o 

clean:
	@rm -rf $(LIBPATH)
	@rm -rf $(OBJPATH)
	@rm -rf $(BINPATH)
	@rm -rf $(DATAPATH)
	@rm -rf $(ROOT)/ricp/build/

bin: libparser.a libvocabulary.a $(SRCPATH)/collection_parser.cc 
	$(CC) $(BINFLAGS) -liconv $(SRCPATH)/collection_parser.cc $(RICPLIB) $(LIBPATH)/libparser.a $(LIBPATH)/libvocabulary.a $(LINKFLAGS) -o $(BINPATH)/collection_parser
