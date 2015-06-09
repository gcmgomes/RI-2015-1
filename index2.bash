#!/bin/bash/

ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

collection=$1
collection_index=$2

# Parse the collection.
echo "collection_parser $1 $2 parser/stop_words.txt data2/terms.txt data2/tuples.bin"
time ${ROOTDIR}/parser/bin/collection_parser $1 $2 ${ROOTDIR}/parser/stop_words.txt ${ROOTDIR}/data2/terms.txt ${ROOTDIR}/data2/document_metadata.bin ${ROOTDIR}/data2/tuples.bin ${ROOTDIR}/data2/anchor_tuples.bin

# Get optimal number of runs.
echo "optimal_run_counter data2/tuples.bin 40000000"
RUNCOUNT=$(${ROOTDIR}/util/bin/optimal_run_counter ${ROOTDIR}/data2/tuples.bin 40000000)

# Get optimal number of runs for anchor text.
echo "optimal_run_counter data2/anchor_tuples.bin 40000000"
RUNCOUNT_ANCHOR=$(${ROOTDIR}/util/bin/optimal_run_counter ${ROOTDIR}/data2/anchor_tuples.bin 40000000)

# Partially sort the runs.
echo "emplace_partial_sorter ${RUNCOUNT} data2/tuples.bin"
time ${ROOTDIR}/external_sorter/bin/emplace_partial_sorter ${RUNCOUNT} ${ROOTDIR}/data2/tuples.bin

# Partially sort the runs for anchor text.
echo "emplace_partial_sorter ${RUNCOUNT_ANCHOR} data2/anchor_tuples.bin"
time ${ROOTDIR}/external_sorter/bin/emplace_partial_sorter ${RUNCOUNT_ANCHOR} ${ROOTDIR}/data2/anchor_tuples.bin

# Perform multiway merging and generate the index.
echo "external_sorter ${RUNCOUNT} data2/tuples.bin data2/index.bin"
time ${ROOTDIR}/external_sorter/bin/external_sorter ${RUNCOUNT} ${ROOTDIR}/data2/tuples.bin ${ROOTDIR}/data2/index.bin

# Perform multiway merging and generate the index.
echo "external_sorter ${RUNCOUNT_ANCHOR} data2/anchor_tuples.bin data2/anchor_index.bin"
time ${ROOTDIR}/external_sorter/bin/external_sorter ${RUNCOUNT_ANCHOR} ${ROOTDIR}/data2/anchor_tuples.bin ${ROOTDIR}/data2/anchor_index.bin

echo "ranking/bin/preprocess_ranking_data data2/document_metadata.bin data2/enhanced_document_metadata.bin data2/index.bin data2/anchor_index.bin"
${ROOTDIR}/ranking/bin/preprocess_ranking_data ${ROOTDIR}/data2/document_metadata.bin ${ROOTDIR}/data2/enhanced_document_metadata.bin ${ROOTDIR}/data2/index.bin ${ROOTDIR}/data2/anchor_index.bin

echo "components/bin/preprocess_vocabulary data2/terms.txt data2/index.bin data2/anchor_index.bin data2/remapped_terms.txt"
${ROOTDIR}/components/bin/preprocess_vocabulary ${ROOTDIR}/data2/terms.txt ${ROOTDIR}/data2/index.bin ${ROOTDIR}/data2/anchor_index.bin ${ROOTDIR}/data2/remapped_terms.txt
