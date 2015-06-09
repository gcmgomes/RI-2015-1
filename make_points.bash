#!/bin/bash

queries_names[1]="ana maria braga"
queries_names[2]="baixaki"
queries_names[3]="caixa economica federal"
queries_names[4]="casa e video"
queries_names[5]="claro"
queries_names[6]="concursos"
queries_names[7]="detran"
queries_names[8]="esporte"
queries_names[9]="frases de amor"
queries_names[10]="funk"
queries_names[11]="globo"
queries_names[12]="gmail"
queries_names[13]="google"
queries_names[14]="hotmail"
queries_names[15]="ig"
queries_names[16]="jogos de meninas"
queries_names[17]="jogos online"
queries_names[18]="mario"
queries_names[19]="mercado livre"
queries_names[20]="msn"
queries_names[21]="naruto"
queries_names[22]="oi"
queries_names[23]="orkut"
queries_names[24]="panico"
queries_names[25]="poquer"
queries_names[26]="previsao do tempo"
queries_names[27]="receita federal"
queries_names[28]="record"
queries_names[29]="rio de janeiro"
queries_names[30]="terra"
queries_names[31]="uol"
queries_names[32]="vivo"
queries_names[33]="yahoo"
queries_names[34]="youtube"
queries_names[35]="media"

eval_dir=$1
curve_file_prefix=$2
query=$3
query_name=${queries_names[query]}
echo $query_name
if [ $query == 35 ]; then
  query="all"
fi

vat_beta=$4
vpr_beta=$5

ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
rm points.txt

#pure vector
i="pure_vector"
beta=1

  beta=$(grep -m 1 -o -E "${i} [0-9]+(\.[0-9]+)*" ${ROOTDIR}/${eval_dir}/performance.txt.p5 | grep -o -E "[0-9]+(\.[0-9]+)*$");
  echo ${i}
  grep -E "iprec_at_recall_[0-9]\.[0-9]+[[:space:]]+${query}[[:space:]]" ${ROOTDIR}/${eval_dir}/${i}/beta_${beta}/detailed_evaluation.txt | grep -o -E "[0-9]\.[0-9]+$" >> points.txt
  grep -E "iprec_at_recall_[0-9]\.[0-9]+[[:space:]]+${query}[[:space:]]" ${ROOTDIR}/${eval_dir}/${i}/beta_${beta}/detailed_evaluation.txt | grep -o -E "[0-9]\.[0-9]+$"


# Pure plus anchor text (vat)
i="pure_plus_anchor_vector"
beta=$vat_beta

  beta=$(grep -m 1 -o -E "${i} [0-9]+(\.[0-9]+)*" ${ROOTDIR}/${eval_dir}/performance.txt.p5 | grep -o -E "[0-9]+(\.[0-9]+)*$");
  echo ${i}
  grep -E "iprec_at_recall_[0-9]\.[0-9]+[[:space:]]+${query}[[:space:]]" ${ROOTDIR}/${eval_dir}/${i}/beta_${beta}/detailed_evaluation.txt | grep -o -E "[0-9]\.[0-9]+$" >> points.txt
  grep -E "iprec_at_recall_[0-9]\.[0-9]+[[:space:]]+${query}[[:space:]]" ${ROOTDIR}/${eval_dir}/${i}/beta_${beta}/detailed_evaluation.txt | grep -o -E "[0-9]\.[0-9]+$"


# Pure plus page rank (vpr)
i="pure_vector_plus_page_rank"
beta=$vpr_beta

  beta=$(grep -m 1 -o -E "${i} [0-9]+(\.[0-9]+)*" ${ROOTDIR}/${eval_dir}/performance.txt.p5 | grep -o -E "[0-9]+(\.[0-9]+)*$");
  echo ${i}
  grep -E "iprec_at_recall_[0-9]\.[0-9]+[[:space:]]+${query}[[:space:]]" ${ROOTDIR}/${eval_dir}/${i}/beta_${beta}/detailed_evaluation.txt | grep -o -E "[0-9]\.[0-9]+$" >> points.txt
  grep -E "iprec_at_recall_[0-9]\.[0-9]+[[:space:]]+${query}[[:space:]]" ${ROOTDIR}/${eval_dir}/${i}/beta_${beta}/detailed_evaluation.txt | grep -o -E "[0-9]\.[0-9]+$"



  sed "s/query/${query_name}/g" default_plot.r | R --no-save

  mv curves.jpg curves/${query}/${curve_file_prefix}_curves_${query}.jpg
