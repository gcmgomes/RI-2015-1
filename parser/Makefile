CC := g++
BINFLAGS := -std=c++11 -Wall -O3
LINKFLAGS := -lz -lgumbo
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

GUTIL := $(PROOT)/util

RICPLIB := $(ROOT)/ricp/src/libRICPlib.a

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
	cd $(GUTIL) && $(MAKE) page.o && $(MAKE) libfilemanager.a

htmlEntities.o: $(UTILPATH)/htmlEntities.h $(UTILPATH)/htmlEntities.cc
	$(CC) $(CFLAGS) $(UTILPATH)/htmlEntities.cc -o $(OBJPATH)/htmlEntities.o

libhtmlEntities.a: htmlEntities.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libhtmlEntities.a $(OBJPATH)/htmlEntities.o

parser.o: $(ROOT)/parser.h $(ROOT)/parser.cc $(UTILPATH)/text_processing.h 
	$(CC) $(CFLAGS) $(ROOT)/parser.cc -o $(OBJPATH)/parser.o

libparser.a: dependencies parser.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libparser.a $(OBJPATH)/parser.o $(GUTIL)/obj/page.o $(OBJPATH)/htmlEntities.o $(GUTIL)/obj/file_manager.o  $(GUTIL)/obj/tuple.o  

vocabulary.o: ${ROOT}/vocabulary.h ${ROOT}/vocabulary.cc
	$(CC) $(CFLAGS) $(ROOT)/vocabulary.cc -o $(OBJPATH)/vocabulary.o

libvocabulary.a: vocabulary.o
	$(AR) $(ARFLAGS) $(LIBPATH)/libvocabulary.a $(OBJPATH)/vocabulary.o

clean:
	rm $(LIBPATH)/*.a
	rm $(OBJPATH)/*.o

test: libparser.a libvocabulary.a
	$(CC) $(BINFLAGS) $(SRCPATH)/collection_parser.cc $(RICPLIB) $(LIBPATH)/libparser.a $(LIBPATH)/libvocabulary.a $(LINKFLAGS) -o $(BINPATH)/parser
