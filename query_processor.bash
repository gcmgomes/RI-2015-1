#!/bin/bash/

ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

# Query processor
${ROOTDIR}/components/bin/query_index data/terms.txt data/index.bin
