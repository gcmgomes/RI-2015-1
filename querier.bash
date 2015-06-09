#!/bin/bash/

ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

model=$1
beta=$2
use_dl_norm=$3

${ROOTDIR}/components/bin/retrieve_index ${ROOTDIR}/data/remapped_terms.txt ${ROOTDIR}/data/index.bin ${ROOTDIR}/data/anchor_index.bin ${ROOTDIR}/data/enhanced_document_metadata.bin ${ROOTDIR}/ranking/relevantes-34/ ${ROOTDIR}/queries.txt ${model} ${beta} ${use_dl_norm}
