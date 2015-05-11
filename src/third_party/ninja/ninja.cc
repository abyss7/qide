#include <third_party/ninja/ninja.h>

#include <third_party/ninja/exported/src/graph.h>
#include <third_party/ninja/exported/src/manifest_parser.h>
#include <third_party/ninja/exported/src/util.h>

#include <QDir>
#include <QSet>

namespace {

struct RealFileReader : public ManifestParser::FileReader {
  virtual bool ReadFile(const string& path, string* content, string* err) {
    return ::ReadFile(path, content, err) == 0;
  }
};

}  // namespace

Ninja::Ninja(const QString& build_dir) : build_dir_(build_dir) {
}

Ninja::InputList Ninja::QueryAllInputs(const std::string& target) {
  InputList results;

  string err;
  RealFileReader file_reader;
  ManifestParser parser(&state_, &file_reader, false);
  if (!parser.Load("build.ninja", &err)) {
    throw std::runtime_error("Failed to parse Ninja files: " + err);
  }

  QList<Node*> pending_nodes{state_.LookupNode(target)};
  QSet<Node*> visited_nodes;

  while (!pending_nodes.empty()) {
    auto* node = pending_nodes.front();
    pending_nodes.pop_front();

    if (visited_nodes.contains(node) || node == nullptr) {
      continue;
    }
    visited_nodes.insert(node);

    auto handle_edge = [this, &pending_nodes, &results](Edge* edge) {
      if (edge != nullptr) {
        if (edge->rule_->name() == "cxx") {
          for (const auto* input : edge->inputs_) {
            results.push_back(qMakePair(
                build_dir() + QDir::separator() +
                    QString::fromStdString(input->path()),
                QString::fromStdString(edge->EvaluateCommand()).split(' ')));
          }
        }
        for (auto* node : edge->inputs_) {
          pending_nodes.push_back(node);
        }
      }
    };

    handle_edge(node->in_edge());

    auto edges = node->out_edges();
    for (auto* edge : edges) {
      handle_edge(edge);
    }
  }

  return results;
}
