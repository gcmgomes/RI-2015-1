#!/bin/bash/

ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

model=$1
beta=$2
output_file=$3
eval_dir=$4

names[0]="pure_vector"
names[1]="anchor_vector"
names[2]="page_rank"
names[3]="pure_plus_anchor_vector"
names[4]="pure_vector_plus_page_rank"

model_name=${names[model]}
echo $model_name $beta

if [ ! -d ${ROOTDIR}/${eval_dir}/$model_name ]; then
  mkdir ${ROOTDIR}/${eval_dir}/$model_name
fi

if [ ! -d ${ROOTDIR}/${eval_dir}/${model_name}/beta_${beta} ]; then
  mkdir ${ROOTDIR}/${eval_dir}/${model_name}/beta_${beta}
fi
rm -rf ${ROOTDIR}/${eval_dir}/${model_name}/beta_${beta}/*

echo "Performing ranking"

bash ${ROOTDIR}/querier.bash ${model} ${beta} < ${ROOTDIR}/queries.txt > ${ROOTDIR}/${eval_dir}/${model_name}/beta_${beta}/results.txt

echo "Performing trec_eval"
${ROOTDIR}/trec_eval/trec_eval ${ROOTDIR}/ranking/judgement.txt ${ROOTDIR}/${eval_dir}/${model_name}/beta_${beta}/results.txt -q > ${ROOTDIR}/${eval_dir}/${model_name}/beta_${beta}/detailed_evaluation.txt

p_at_5=$(grep -E "P_5[[:space:]]+all" ${ROOTDIR}/${eval_dir}/${model_name}/beta_${beta}/detailed_evaluation.txt | grep -o -E "[0-9]\.[0-9]+") 
p_at_10=$(grep -E "P_10[[:space:]]+all" ${ROOTDIR}/${eval_dir}/${model_name}/beta_${beta}/detailed_evaluation.txt | grep -o -E "[0-9]\.[0-9]+") 
echo ${model_name} ${beta} ${p_at_5} ${p_at_10} >> ${output_file}
