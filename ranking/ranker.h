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
  Ranker(::components::Retriever* retriever) : retriever_(retriever){};

  const std::unique_ptr<::components::Retriever>& retriever() const {
    return retriever_;
  }

  // Ranks the collection's documents according to |query|, and stores the
  // sorted results in |answers|.
  // |model| controls which ranking function is used.
  // 0 -> vector model.
  // 1 -> anchor text.
  // 2 -> page rank.
  // 3 -> vector + anchor text.
  // 4 -> vector + page rank.
  void Rank(std::string query, std::vector<::util::Page>& answers,
            unsigned model);

 private:
  // Get the |weights| associated with |query|.
  void GetFrequencies(const std::string query,
                      std::unordered_map<unsigned, double>& weights) const;

  // Make a |page| based on |query|. Primarily used in the vector model.
  void MakePage(const std::string& query, ::util::Page& page) const;

  // Score |document| according to |query|, using the vector model.
  double VectorScore(const ::util::Page& query, const ::util::Page& document);

  std::unique_ptr<::components::Retriever> retriever_;
};

}  // namespace ranking

#endif
