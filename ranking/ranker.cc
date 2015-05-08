#include <algorithm>
#include "ranker.h"

namespace ranking {

void Ranker::Rank(std::string query, std::vector<::util::Page>& answers,
                  unsigned model) {
  unsigned i = 0;
  ::util::Page query_page;
  MakePage(query, query_page);
  while (i < answers.size()) {
    double score = 0, s1 = 0, s2 = 0;
    if (!(model % 5)) {
      score = VectorScore(query_page, answers[i], false);
    }
    if (model % 5 == 1) {
      score = VectorScore(query_page, answers[i], true);
    }
    if (model % 5 == 3) {
      s1 = VectorScore(query_page, answers[i], false);
      s2 = VectorScore(query_page, answers[i], true);
      score = beta_ * s1 + (1-beta_) * s2;
    }
    answers[i].score_ = score;
    ++i;
  }
  ::util::PageHash page_sorter;
  std::sort(answers.begin(), answers.end(), page_sorter);
}

void Ranker::GetFrequencies(
    const std::string& query,
    std::unordered_map<unsigned, double>& weights, bool anchor_weighting) const {
  weights.clear();
  unsigned pos = 0;
  while (pos < query.size()) {
    std::string token = "";
    components::Retriever::GetToken(query, pos, token);
    if (!token.empty() && retriever_->vocabulary()->Check(token)) {
      ::parsing::VocabularyEntry entry = retriever_->vocabulary()->GetMappedValue(token);
      unsigned weight_id = entry.first;
      if(anchor_weighting) {
        weight_id = entry.second;
      }
      weights[weight_id]++;
    }
  }
}

void Ranker::MakePage(const std::string& query, ::util::Page& page) const {
  std::unordered_map<unsigned, double> weights;
  GetFrequencies(query, weights, false);
  page.weights_ = weights;
  GetFrequencies(query, weights, true);
  page.anchor_weights_ = weights;
  page.CalculateLength();
}

double Ranker::VectorScore(const ::util::Page& query,
                           const ::util::Page& document,
                           bool anchor_weighting) {
  double dot_prod = 0, length_prod = 0;
  auto weights = query.weights();
  if(anchor_weighting) {
    weights = query.anchor_weights();
  }
  auto i = weights.begin();
  while (i != weights.end()) {
    if(anchor_weighting) {
      if(document.anchor_weights().count(i->first)) {
        std::cout << i->first << ' ' << i->second << ' ' << document.anchor_weights().at(i->first);
        dot_prod += i->second * document.anchor_weights().at(i->first);
      }
    }
    else {
      if(document.weights().count(i->first)) {
        dot_prod += i->second * document.weights().at(i->first);
      }
    }
    ++i;
  }
  length_prod = query.length() * document.length();

  if(anchor_weighting) {
    length_prod = query.anchor_length() * document.anchor_length();
  }

  if(length_prod <= 0) {
    return 0;
  }
  return dot_prod / length_prod;
}

}  // namespace ranking
