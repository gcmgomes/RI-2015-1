#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <set>
#include <memory>
#include <unordered_set>
#include "../retriever.h"
#include "../../ranking/ranker.h"
#include "../../ranking/query_evaluator.h"
#include "../../util/page.h"

using namespace std;

int main(int argc, char** argv) {
  if (argc < 7) {
    cout << argv[0] << " [vocabulary file] [binary index file] [binary anchor "
                       "index file] [rankng metadata file] [relevants "
                       "directory] [relevants index] <ranking model (0)> <beta "
                       "weighting(0.5)> <use dl norm (1)>" << endl;
    return 0;
  }

  double beta = 0.5;
  unsigned model = 0;
  bool use_dl_norm = true;
  if (argc > 9) {
    sscanf(argv[9], "%u", &model);
    if (!model) {
      std::cerr << "not using dl norm" << std::endl;
      use_dl_norm = false;
    }
    model = 0;
  }
  if (argc > 7) {
    sscanf(argv[7], "%u", &model);
  }

  if (argc > 8) {
    sscanf(argv[8], "%lf", &beta);
  }

  int query_count = 1;
  components::Retriever* retriever =
      new components::Retriever(argv[2], argv[3], argv[4]);
  ranking::QueryEvaluator evaluator(argv[5], argv[6]);
  evaluator.ReadRelevants();
  retriever->Init(argv[1]);
  ranking::Ranker ranker(retriever, beta);

  while (true) {
    std::string query = "";
    cerr << "\rQuery: " << query_count;
    fflush(stderr);
    std::getline(cin, query);
    if (cin.eof()) {
      break;
    }
    std::unordered_set<unsigned> answers;
    std::vector<::util::Page> ranked_answers;
    ranker.retriever()->Retrieve(query, answers);
    ranker.Rank(query, answers, ranked_answers,
                ranking::Ranker::RankingModel(model), beta, use_dl_norm);
    unsigned i = 0;
    std::unordered_set<std::string> known_documents;
    while (i < ranked_answers.size()) {
      if (!known_documents.count(ranked_answers[i].url())) {
        cout << query_count << ' ' << 0 << ' ' << ranked_answers[i].url() << ' '
             << 1 << ' ' << ranked_answers[i].score() << ' ' << 0 << endl;
        known_documents.insert(ranked_answers[i].url());
      }
      ++i;
    }
    //    vector<double> precisions, recalls;
    //    evaluator.ComputeMetrics(query, ranked_answers, precisions, recalls);
    query_count++;
  }
  cerr << endl << "Goodbye" << endl;
}
