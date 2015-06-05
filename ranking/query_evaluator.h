#ifndef _RI_2015_1_RANKING_QUERY_EVALUATOR_H_
#define _RI_2015_1_RANKING_QUERY_EVALUATOR_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "ranking_metadata.h"
#include "../util/page.h"

namespace ranking {

class QueryEvaluator {
 public:
  QueryEvaluator(std::string directory, std::string query_index_file_name);

  const std::unordered_map<std::string, std::unordered_set<std::string>>&
  query_relevants() {
    return query_relevants_;
  }

  // Reads the file |directory_ + query_index_file_name_| and builds
  // |query_relevants_|.
  void ReadRelevants();

  // Reads the file |directory_ + query| and builds
  // |query_relevants_[query]|.
  void ReadRelevants(const std::string& query);

  // Computes the precision and recall values for |query| based on
  // |ranked_answers| and stores them in the appropriate vectors.
  bool ComputeMetrics(const std::string& query,
                      const std::vector<::util::Page>& ranked_answers,
                      std::vector<double>& precision_values,
                      std::vector<double>& recall_values);

 private:
  // The directory containing the needed query information files.
  std::string directory_;

  // The file containing the queries with known relevance information.
  std::string query_index_file_name_;

  // The mapping query -> normalized relevant page urls.
  std::unordered_map<std::string, std::unordered_set<std::string>>
      query_relevants_;
};

}  // namespace ranking

#endif
