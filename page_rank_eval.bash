#!/bin/bash/
output_file=$1
eval_dir=$2
use_dl_norm=1

ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

rm $output_file

model=2

for q in 0.05 0.1 0.15 0.2 0.25 0.3 0.35 0.4 0.45 0.5 0.55 0.6 0.65 0.7 0.75 0.8 0.85 0.9 0.95 1; do 
  ${ROOTDIR}/ranking/bin/preprocess_ranking_data ${ROOTDIR}/data/document_metadata.bin ${ROOTDIR}/data/enhanced_document_metadata.bin ${ROOTDIR}/data/index.bin ${ROOTDIR}/data/anchor_index.bin $q $q
  bash ${ROOTDIR}/eval.bash $model $q $use_dl_norm $output_file $eval_dir
done

sort ${output_file} -g -k4 -r > ${output_file}".p10"

sort ${output_file} -g -k3 -r > ${output_file}".p5"
