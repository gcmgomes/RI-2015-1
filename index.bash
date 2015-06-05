#!/bin/bash/

ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

collection=$1
collection_index=$2

# Parse the collection.
echo "collection_parser $1 $2 parser/stop_words.txt data/terms.txt data/tuples.bin"
time ${ROOTDIR}/parser/bin/collection_parser $1 $2 ${ROOTDIR}/parser/stop_words.txt ${ROOTDIR}/data/terms.txt ${ROOTDIR}/data/document_metadata.bin ${ROOTDIR}/data/tuples.bin ${ROOTDIR}/data/anchor_tuples.bin

# Get optimal number of runs.
echo "optimal_run_counter data/tuples.bin 40000000"
RUNCOUNT=$(${ROOTDIR}/util/bin/optimal_run_counter ${ROOTDIR}/data/tuples.bin 40000000)

# Get optimal number of runs for anchor text.
echo "optimal_run_counter data/anchor_tuples.bin 40000000"
RUNCOUNT_ANCHOR=$(${ROOTDIR}/util/bin/optimal_run_counter ${ROOTDIR}/data/anchor_tuples.bin 40000000)

# Partially sort the runs.
echo "emplace_partial_sorter ${RUNCOUNT} data/tuples.bin"
time ${ROOTDIR}/external_sorter/bin/emplace_partial_sorter ${RUNCOUNT} ${ROOTDIR}/data/tuples.bin

# Partially sort the runs for anchor text.
echo "emplace_partial_sorter ${RUNCOUNT_ANCHOR} data/anchor_tuples.bin"
time ${ROOTDIR}/external_sorter/bin/emplace_partial_sorter ${RUNCOUNT_ANCHOR} ${ROOTDIR}/data/anchor_tuples.bin

# Perform multiway merging and generate the index.
echo "external_sorter ${RUNCOUNT} data/tuples.bin data/index.bin"
time ${ROOTDIR}/external_sorter/bin/external_sorter ${RUNCOUNT} ${ROOTDIR}/data/tuples.bin ${ROOTDIR}/data/index.bin

# Perform multiway merging and generate the index.
echo "external_sorter ${RUNCOUNT_ANCHOR} data/anchor_tuples.bin data/anchor_index.bin"
time ${ROOTDIR}/external_sorter/bin/external_sorter ${RUNCOUNT_ANCHOR} ${ROOTDIR}/data/anchor_tuples.bin ${ROOTDIR}/data/anchor_index.bin

echo "ranking/bin/preprocess_ranking_data data/document_metadata.bin data/document_metadata2.bin data/index.bin data/anchor_index.bin"
${ROOTDIR}/ranking/bin/preprocess_ranking_data ${ROOTDIR}/data/document_metadata.bin ${ROOTDIR}/data/document_metadata2.bin ${ROOTDIR}/data/index.bin ${ROOTDIR}/data/anchor_index.bin

echo "components/bin/preprocess_vocabulary data/terms.txt data/index.bin data/anchor_index.bin data/remapped_terms.txt"
${ROOTDIR}/components/bin/preprocess_vocabulary ${ROOTDIR}/data/terms.txt ${ROOTDIR}/data/index.bin ${ROOTDIR}/data/anchor_index.bin ${ROOTDIR}/data/remapped_terms.txt
