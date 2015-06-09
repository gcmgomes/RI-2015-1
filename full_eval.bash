#!/bin/bash/
output_file=$1
eval_dir=$2
use_dl_norm=$3

ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

rm $output_file

for model in 0 1 2; do
  bash ${ROOTDIR}/eval.bash $model 1 $use_dl_norm $output_file $eval_dir
done

for model in 3 4; do
  for beta in 0 0.05 0.1 0.15 0.2 0.25 0.3 0.35 0.4 0.45 0.5 0.55 0.6 0.65 0.7 0.75 0.8 0.85 0.9 0.95 1; do
    bash ${ROOTDIR}/eval.bash $model $beta $use_dl_norm $output_file $eval_dir
  done
done

sort ${output_file} -g -k4 -r > ${output_file}".p10"

sort ${output_file} -g -k3 -r > ${output_file}".p5"
