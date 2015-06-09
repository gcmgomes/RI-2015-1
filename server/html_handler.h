#ifndef _RI_2015_1_SERVER_HTML_HANDLER_H_
#define _RI_2015_1_SERVER_HTML_HANDLER_H_

#include <iostream>
#include <memory>
#include <queue>
#include <vector>
#include <string>
#include "../ranking/ranker.h"
#include "../util/page.h"

namespace server {

class HtmlHandler {
 public:
  HtmlHandler(const std::string& standard_home_page_file,
              const std::string& standard_results_page_file,
              const std::string& vocabulary_path, const std::string& index_path,
              const std::string& anchor_index_path,
              const std::string& metadata_path);

  std::string standard_home_page() {
    return standard_home_page_;
  }

  // Constructs the results page using |ranked_pages|. Stores the results in
  // |resulting_html|.
  void MakeResultsPage(const std::vector<::util::Page>& ranked_pages,
                       unsigned max_pages, std::string& resulting_html);

  // Queries the known pages according to |query| and returns a page in
  // |resulting_html|.
  void QueryPages(const std::string& query, unsigned model, double beta,
                  bool use_dl_norm, std::string& resulting_html);

 private:
  // Read the homepage provided by |standard_home_page_file|.
  void ReadHomePage(const std::string& standard_home_page_file);

  // The page where the user is redirected.
  std::string standard_home_page_;

  // Everything before the string containing "Resultados".
  std::string results_beginning_;

  // Everything after the </ol>.
  std::string results_ending_;

  std::unique_ptr<ranking::Ranker> ranker;
};

}  // namespace server

#endif
