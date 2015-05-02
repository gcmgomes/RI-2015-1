#include <algorithm>
#include "ranker.h"

namespace ranking {

void Ranker::Rank(std::string query, std::vector<::util::Page>& answers,
                  unsigned model) {
  unsigned i = 0;
  ::util::Page query_page;
  MakePage(query, query_page);
  while (i < answers.size()) {
    double score = 0;
    if (!(model % 5)) {
      score = VectorScore(query_page, answers[i]);
    }
    answers[i].score_ = score;
    ++i;
  }
  ::util::PageHash page_sorter;
  std::sort(answers.begin(), answers.end(), page_sorter);
}

void Ranker::GetFrequencies(
    const std::string query,
    std::unordered_map<unsigned, double>& weights) const {
  unsigned pos = 0;
  while (pos < query.size()) {
    std::string token = "";
    components::Retriever::GetToken(query, pos, token);
    if (!token.empty() && retriever_->vocabulary()->Check(token)) {
      weights[retriever_->vocabulary()->GetMappedValue(token)]++;
    }
  }
}

void Ranker::MakePage(const std::string& query, ::util::Page& page) const {
  std::unordered_map<unsigned, double> weights;
  GetFrequencies(query, weights);
  page.weights_ = weights;
  page.CalculateLength();
}

double Ranker::VectorScore(const ::util::Page& query,
                           const ::util::Page& document) {
  double dot_prod = 0, length_prod = 0;
  auto i = query.weights().begin();
  while (i != query.weights().end()) {
    dot_prod += i->second * document.weights().at(i->first);
    ++i;
  }
  length_prod = query.length() * document.length();

  return dot_prod / length_prod;
}

}  // namespace ranking
