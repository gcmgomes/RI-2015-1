#!/bin/bash/

ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

${ROOTDIR}/server/bin/query_server ${ROOTDIR}/server/src/form.html ${ROOTDIR}/server/src/"return".html ${ROOTDIR}/data/remapped_terms.txt ${ROOTDIR}/data/index.bin ${ROOTDIR}/data/anchor_index.bin ${ROOTDIR}/data/enhanced_document_metadata.bin
