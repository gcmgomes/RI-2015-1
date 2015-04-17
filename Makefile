ROOT := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

DATA := $(ROOT)/data

UTIL := $(ROOT)/util

EXTERNALSORTER := $(ROOT)/external_sorter

PARSER := $(ROOT)/parser

COMPONENTS := $(ROOT)/components

make: clean deploy bin

clean:
	@rm -rf $(DATA);
	cd $(UTIL) && $(MAKE) clean;
	cd $(EXTERNALSORTER) && $(MAKE) clean;
	cd $(PARSER) && $(MAKE) clean;
	cd $(COMPONENTS) && $(MAKE) clean;

deploy:
	@if [ ! -d $(DATA) ]; \
		then \
			mkdir $(DATA); \
	fi
	cd $(UTIL) && $(MAKE) deploy;
	cd $(EXTERNALSORTER) && $(MAKE) deploy;
	cd $(PARSER) && $(MAKE) deploy;
	cd $(COMPONENTS) && $(MAKE) deploy;

bin:
	cd $(UTIL) && $(MAKE) bin;
	cd $(EXTERNALSORTER) && $(MAKE) bin;
	cd $(PARSER) && $(MAKE) bin;
	cd $(COMPONENTS) && $(MAKE) bin;
