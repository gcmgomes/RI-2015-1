#!/bin/bash/

ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

collection=$1
collection_index=$2

# Parse the collection.
${ROOTDIR}/parser/bin/parse $1 $2 parser/stop_words.txt parser/terms.txt data/tuples.bin

# Get optimal number of runs.
RUNCOUNT=$(${ROOTDIR}/util/parser/bin/optimal_run_count data/tuples.bin 40000000)

# Partially sort the runs.
${ROOTDIR}/external_sorter/bin/emplace_partial_sorter ${RUNCOUNT} data/tuples.bin

# Perform multiway merging and generate the index.
${ROOTDIR}/external_sorter/bin/external_sorter ${RUNCOUNT} data/tuples.bin data/index.bin

# Resolve vocabulary linkage
