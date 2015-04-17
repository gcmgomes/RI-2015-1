#!/bin/bash/

ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

# Query processor
valgrind ${ROOTDIR}/components/bin/query_index data/terms.bin data/index.bin
