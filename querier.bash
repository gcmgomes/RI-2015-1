#!/bin/bash/

ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

model=$1
beta=$2

${ROOTDIR}/components/bin/retrieve_index ${ROOTDIR}/data/remapped_terms.txt ${ROOTDIR}/data/index.bin ${ROOTDIR}/data/anchor_index.bin ${ROOTDIR}/data/document_metadata2.bin ${ROOTDIR}/ranking/relevantes-34/ ${ROOTDIR}/queries.txt ${model} ${beta}
