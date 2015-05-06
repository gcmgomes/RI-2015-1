#!/bin/bash/

ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

collection=$1
collection_index=$2

# Parse the collection.
echo "collection_parser $1 $2 parser/stop_words.txt data/terms.txt data/tuples.bin"
valgrind --leak-check=full ${ROOTDIR}/parser/bin/collection_parser $1 $2 ${ROOTDIR}/parser/stop_words.txt ${ROOTDIR}/data/terms.txt ${ROOTDIR}/data/document_lengths.bin ${ROOTDIR}/data/tuples.bin ${ROOTDIR}/data/anchor_tuples.bin

# Get optimal number of runs.
echo "optimal_run_counter data/tuples.bin 40000000"
RUNCOUNT=$(${ROOTDIR}/util/bin/optimal_run_counter ${ROOTDIR}/data/tuples.bin 40000000)

# Partially sort the runs.
echo "emplace_partial_sorter ${RUNCOUNT} data/tuples.bin"
${ROOTDIR}/external_sorter/bin/emplace_partial_sorter ${RUNCOUNT} ${ROOTDIR}/data/tuples.bin

# Perform multiway merging and generate the index.
echo "external_sorter ${RUNCOUNT} data/tuples.bin data/index.bin"
${ROOTDIR}/external_sorter/bin/external_sorter ${RUNCOUNT} ${ROOTDIR}/data/tuples.bin ${ROOTDIR}/data/index.bin
