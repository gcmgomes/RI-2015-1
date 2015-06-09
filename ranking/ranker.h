#ifndef _RI_2015_1_RANKING_RANKER_H_
#define _RI_2015_1_RANKING_RANKER_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include "../components/retriever.h"
#include "../util/page.h"

namespace ranking {

class Ranker {
 public:
  // Takes ownership of |retriever|.
  Ranker(::components::Retriever* retriever, double beta)
      : retriever_(retriever), beta_(beta){};

  const std::unique_ptr<::components::Retriever>& retriever() const {
    return retriever_;
  }

  // Mixed models are balanced as (beta * model_1) + (1-beta) * model_2.
  // That is, higher values of beta are better for model_1.
  enum RankingModel {
    PURE_VECTOR = 0,
    ANCHOR_VECTOR = 1,
    PAGE_RANK = 2,
    PURE_PLUS_ANCHOR_VECTOR = 3,
    PURE_VECTOR_PLUS_PAGE_RANK = 4,
  };

  // Ranks the collection's documents according to |query|, and stores the
  // sorted results in |answers|.
  // |model| controls which ranking function is used.
  void Rank(const std::string& query,
            const std::unordered_set<unsigned>& answers,
            std::vector<::util::Page>& ranked_answers, RankingModel model,
            double beta = -1, bool use_dl_norm = true);

 private:
  // Get the |weights| associated with |query|.
  void GetFrequencies(const std::string& query,
                      std::unordered_map<unsigned, double>& weights,
                      bool anchor_weighting) const;

  // Produce a vectorial scoring for the pages referenced by |answers|, based on
  // |query| and stores them in the mapping |scores|.
  void VectorScore(const std::string& query,
                   const std::unordered_set<unsigned>& answers,
                   std::unordered_map<unsigned, double>& scores,
                   bool anchor_weighting, bool use_dl_norm = true);

  // Produce a page rank scoring for the pages referenced by |answers|, based on
  // |query| and stores them in the mapping |scores|.
  void PageRankScore(const std::unordered_set<unsigned>& answers,
                     std::unordered_map<unsigned, double>& scores);

  std::unique_ptr<::components::Retriever> retriever_;

  // Balancing constant when using 2 or more ranking models (e.g. model = 3).
  double beta_;
};

}  // namespace ranking

#endif
