#include "query_evaluator.h"
#include <iomanip>

namespace ranking {

QueryEvaluator::QueryEvaluator(std::string directory,
                               std::string query_index_file_name)
    : directory_(directory), query_index_file_name_(query_index_file_name){};

void QueryEvaluator::ReadRelevants(const std::string& query) {
  std::string file_path = directory_ + query;
  std::ifstream input_file(file_path.c_str());
  while (!input_file.eof() && input_file.peek() != EOF) {
    std::string url;
    std::getline(input_file, url);
    ::util::Page page(0, url, "");
    query_relevants_[query].insert(page.url());
  }
}

void QueryEvaluator::ReadRelevants() {
  std::string file_path = query_index_file_name_;
  std::ifstream input_file(file_path.c_str());
  while (!input_file.eof() && input_file.peek() != EOF) {
    std::string query;
    std::getline(input_file, query);
    ReadRelevants(query);
  }
}

bool QueryEvaluator::ComputeMetrics(
    const std::string& query, const std::vector<::util::Page>& ranked_answers,
    std::vector<double>& precision_values, std::vector<double>& recall_values) {
  if (!query_relevants_.count(query)) {
    return false;
  }

  std::vector<double> dummy_p, dummy_r;
  unsigned retrieved_relevants = 0, retrieved_count = 0;
  const auto& relevants = query_relevants_.at(query);

  auto page = ranked_answers.begin();
  while (page != ranked_answers.end()) {
    double precision = 0, recall = 0;

    retrieved_count++;
    if (relevants.count(page->url())) {
      retrieved_relevants++;
      recall = (double)retrieved_relevants / (double)relevants.size();
      precision = (double)retrieved_relevants / (double)retrieved_count;
      precision_values.push_back(precision);
      recall_values.push_back(recall);
      std::cout << std::fixed << std::setprecision(3) << retrieved_count << "\t\t"
             << precision << "\t\t" << recall << ' ' << page->url() << std::endl;
    }

    ++page;
  }
  return true;
}

}  // namespace ranking
