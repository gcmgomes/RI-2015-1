#include <algorithm>
#include "ranker.h"

namespace ranking {

static void BalanceScores(
    double beta, const std::unordered_set<unsigned> answers,
    std::unordered_map<unsigned, double>& aux_scores,
    std::unordered_map<unsigned, double>& scores) {
  auto i = answers.begin();
  while (i != answers.end()) {
    double val = aux_scores[*i];
    scores[*i] = beta * scores[*i] + (1 - beta) * val;
    ++i;
  }
}

void Ranker::Rank(const std::string& query,
                  const std::unordered_set<unsigned>& answers,
                  std::vector<::util::Page>& ranked_answers,
                  RankingModel model) {
  ::util::PageHash page_sorter;
  std::unordered_map<unsigned, double> scores, aux_scores;
  switch (model) {
    case PURE_VECTOR:
      VectorScore(query, answers, scores, false);
      break;
    case ANCHOR_VECTOR:
      VectorScore(query, answers, scores, true);
      break;
    case PAGE_RANK:
      PageRankScore(answers, scores);
      break;
    case PURE_PLUS_ANCHOR_VECTOR:
      VectorScore(query, answers, scores, false);
      VectorScore(query, answers, aux_scores, true);
      BalanceScores(beta_, answers, aux_scores, scores);
      break;
    case PURE_VECTOR_PLUS_PAGE_RANK:
      VectorScore(query, answers, scores, false);
      PageRankScore(answers, aux_scores);
      BalanceScores(beta_, answers, aux_scores, scores);
      break;
  }
  retriever_->ExtractAnswerPages(scores, ranked_answers);
  std::sort(ranked_answers.begin(), ranked_answers.end(), page_sorter);
}

static void ExtractQueryText(std::string query,
                             std::unordered_set<std::string>& text) {
  unsigned pos = 0;
  std::transform(query.begin(), query.end(), query.begin(), ::tolower);
  while (pos < query.size()) {
    std::string token;
    components::Retriever::GetToken(query, pos, token);
    text.insert(token);
  }
}

void Ranker::VectorScore(const std::string& query,
                         const std::unordered_set<unsigned>& answers,
                         std::unordered_map<unsigned, double>& scores,
                         bool anchor_weighting) {
  double N = retriever_->PageCount();

  std::unordered_set<std::string> query_text;
  ExtractQueryText(query, query_text);
  double query_length = 0;

  auto term = query_text.begin();
  while (term != query_text.end()) {
    ::components::IndexEntry entry;
    if(!retriever_->GetIndexEntry(*term, entry, anchor_weighting)) {
      ++term;
      continue;
    }
    double term_df = entry.occurrences().size();
    double query_term_score = log10(N / term_df);
    query_length += query_term_score * query_term_score;

    auto page_id = answers.begin();
    while (page_id != answers.end()) {
      if (entry.occurrences().count(*page_id)) {
        double tf = entry.occurrences().at(*page_id).size();
        double doc_term_score = (1 + log10(tf)) * log10(N / term_df);
        scores[*page_id] += query_term_score * doc_term_score;
      }
      ++page_id;
    }
    ++term;
  }

  query_length = sqrt(query_length);
  auto page_id = scores.begin();
  while (page_id != scores.end()) {
    double length_prod = 0;

    if (anchor_weighting) {
      length_prod =
          query_length * retriever_->PageLengths(page_id->first).second;
    } else {
      length_prod =
          query_length * retriever_->PageLengths(page_id->first).first;
    }

    if (length_prod <= 0) {
      page_id->second = 0;
    } else {
      //page_id->second /= length_prod;
    }

    ++page_id;
  }
}

void Ranker::PageRankScore(const std::unordered_set<unsigned>& answers,
                           std::unordered_map<unsigned, double>& scores) {
  auto page_id = answers.begin();
  while (page_id != answers.end()) {
    auto page = retriever_->LoadPage(*page_id);
    scores[*page_id] = page->page_rank();
    ++page_id;
  }
}

}  // namespace ranking
