#ifndef _RI_2015_1_RANKING_VECTOR_RANKER_H_
#define _RI_2015_1_RANKING_VECTOR_RANKER_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include "../components/retriever.h"
#include "../util/page.h"

namespace ranking {

class VectorRanker {
 public:
  // Takes ownership of |retriever|.
  VectorRanker(::components::Retriever* retriever) : retriever_(retriever){};

  // Ranks the collection's documents according to |query|, and stores the
  // sorted results in |answers|. This subclass implements the vector model.
  virtual void Rank(std::string query, std::vector<::util::Page>& answers);

 protected:
  std::unique_ptr<::components::Retriever> retriever_;
};

}  // namespace ranking

#endif
