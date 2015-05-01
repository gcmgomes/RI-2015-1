ROOT := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

DATA := $(ROOT)/data

UTIL := $(ROOT)/util

EXTERNALSORTER := $(ROOT)/external_sorter

PARSER := $(ROOT)/parser

COMPONENTS := $(ROOT)/components

RANKING := $(ROOT)/ranking

make: clean deploy bin

clean:
	cd $(UTIL) && $(MAKE) clean;
	cd $(EXTERNALSORTER) && $(MAKE) clean;
	cd $(PARSER) && $(MAKE) clean;
	cd $(COMPONENTS) && $(MAKE) clean;
	cd $(RANKING) && $(MAKE) clean;

deploy:
	@if [ ! -d $(DATA) ]; \
		then \
			mkdir $(DATA); \
	fi
	cd $(UTIL) && $(MAKE) deploy;
	cd $(EXTERNALSORTER) && $(MAKE) deploy;
	cd $(PARSER) && $(MAKE) deploy;
	cd $(COMPONENTS) && $(MAKE) deploy;
	cd $(RANKING) && $(MAKE) deploy;

bin:
	cd $(UTIL) && $(MAKE) bin;
	cd $(EXTERNALSORTER) && $(MAKE) bin;
	cd $(PARSER) && $(MAKE) bin;
	cd $(COMPONENTS) && $(MAKE) bin;
	cd $(RANKING) && $(MAKE) bin;
