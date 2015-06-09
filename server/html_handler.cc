#include <fstream>
#include <unordered_map>
#include "html_handler.h"
#include "../components/retriever.h"

namespace server {

HtmlHandler::HtmlHandler(const std::string& standard_home_page_file,
                         const std::string& standard_results_page_file,
                         const std::string& vocabulary_path,
                         const std::string& index_path,
                         const std::string& anchor_index_path,
                         const std::string& metadata_path) {
  ReadHomePage(standard_home_page_file);

  components::Retriever* retriever =
      new components::Retriever(index_path, anchor_index_path, metadata_path);

  retriever->Init(vocabulary_path);

  ranker.reset(new ::ranking::Ranker(retriever, 0.5));
}

void HtmlHandler::ReadHomePage(const std::string& standard_home_page_file) {
  std::ifstream input_file(standard_home_page_file.c_str(),
                           std::ifstream::binary);
  input_file.seekg(0, input_file.end);
  unsigned size = input_file.tellg();
  input_file.seekg(0, input_file.beg);
  standard_home_page_.clear();
  standard_home_page_.resize(size);
  input_file.read(&standard_home_page_[0], size);
}

static std::string MakeReturnBeginning(const std::string& query, unsigned model,
                                       double beta, bool use_dl_norm) {
  std::vector<double> betas = { 0,    0.05, 0.10, 0.15, 0.20, 0.25, 0.30,
                                0.35, 0.45, 0.50, 0.55, 0.60, 0.65, 0.70,
                                0.75, 0.80, 0.85, 0.90, 0.95, 1.00 };
  std::vector<std::string> models = { "Vector Model", "Anchor Text", "PageRank",
                                      "Vector Model + Anchor Text",
                                      "Vector Model + PageRank" };
  std::vector<std::string> norms = { "Without DL norm", "With DL norm" };
  char num[1000000];
  std::string ret_val =
      "<html>\n\t<head>\n\t\t<title></title>\n\t</head>\n\t<body>\n\t\t<form "
      "action=\"/handle_get_request\" enctype=\"text/plain\" id=\"1\" "
      "method=\"get\" name=\"query_form\">\n";

  // Header.
  sprintf(num,
          "         <p><strong>Super Grep</strong>&nbsp;<input "
          "maxlength=\"60\" name=\"query\" size=\"60\" type=\"text\" "
          "value=\"%s\"/>",
          query.c_str());
  ret_val += num;

  // Beginning of model content.
  ret_val += "<select name=\"model\" size=\"1\">";
  unsigned i = 0;
  while (i < models.size()) {
    if (i == model) {
      sprintf(num, "<option selected=\"selected\" value=\"%u\">", model);
    } else {
      sprintf(num, "<option value=\"%u\">", i);
    }
    ret_val += num;
    ret_val += models[i] + "</option>";
    i++;
  }
  ret_val += "</select>";
  // End of model content.

  // Beginning of normalizing content.
  ret_val += "<select name=\"length_normalize\" size=\"1\">";
  i = 0;
  while (i < norms.size()) {
    if ((i && use_dl_norm) || (!i && !use_dl_norm)) {
      sprintf(num, "<option selected=\"selected\" value=\"%u\">", i);
    } else {
      sprintf(num, "<option value=\"%u\">", i);
    }
    ret_val += num;
    ret_val += norms[i] + "</option>";
    i++;
  }
  ret_val += "</select>";
  // End of normalizing content.

  // Beginning of betas content.
  ret_val += "<select name=\"beta\" size=\"1\">";
  i = 0;
  while (i < betas.size()) {
    if (beta == betas[i]) {
      sprintf(num, "<option selected=\"selected\" value=\"%.2lf\">%.2lf</option>",
              betas[i], betas[i]);
    } else {
      sprintf(num, "<option value=\"%.2lf\">%.2lf</option>", betas[i], betas[i]);
    }
    ret_val += num;
    i++;
  }
  ret_val +=
      "</select><input name=\"submit\" type=\"submit\" value=\"Search\" "
      "/></p>\n</form>\n\n<hr />\n";
  // Ending of betas content.
  return ret_val;
}
void HtmlHandler::MakeResultsPage(const std::vector<::util::Page>& ranked_pages,
                                  unsigned max_pages,
                                  std::string& resulting_html) {
  unsigned i = 0, bound = std::min(max_pages, (unsigned)ranked_pages.size());
  char num[128];
  sprintf(num, "%u de %u", bound, (unsigned)ranked_pages.size());
  resulting_html.clear();
  resulting_html = results_beginning_;
  resulting_html += "<p><span style=\"font-size:12px;\">Resultados: ";
  resulting_html += num;
  resulting_html += "&nbsp;documentos</span></p>\n\n\t\t\t<ol>";
  while (i < bound) {
    resulting_html += "\n\t\t\t\t<li>";
    resulting_html += ranked_pages[i].ToResultString();
    resulting_html += "\n</li><br />";
    i++;
  }
  resulting_html += "\n\t\t\t</ol>\n";
  resulting_html += results_ending_;
}

void HtmlHandler::QueryPages(const std::string& query, unsigned model,
                             double beta, bool use_dl_norm,
                             std::string& resulting_html) {
  std::unordered_set<unsigned> answers;
  std::vector<::util::Page> ranked_answers;
  results_beginning_ = MakeReturnBeginning(query, model, beta, use_dl_norm);
  results_ending_ = "\n\t\t<ol>\n\t\t</ol>\n\t</body>\n</html>";
  std::cout << "Retrieving for query " << query << std::endl;
  ranker->retriever()->Retrieve(query, answers);
  std::cout << "Ranking" << std::endl;
  ranker->Rank(query, answers, ranked_answers,
               ranking::Ranker::RankingModel(model), beta, use_dl_norm);
  MakeResultsPage(ranked_answers, 100, resulting_html);
  std::cout << "Ranked " << query << std::endl;
}

}  // namespace server
