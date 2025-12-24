#include "graph_extractor.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 4) {
    cerr << "Usage: " << argv[0]
         << " <target_path> <min_nodes> <max_nodes> [excluded_props...]"
         << std::endl;
    cerr << "Example: " << argv[0] << " ./data 3 5 z type" << std::endl;
    return 1;
  }

  std::string target_path = argv[1];
  int min_nodes = std::stoi(argv[2]);
  int max_nodes = std::stoi(argv[3]);

  std::vector<std::string> excluded_props;
  for (int i = 4; i < argc; ++i) {
    excluded_props.push_back(argv[i]);
  }

  GraphExtractor extractor;
  extractor.process(target_path, min_nodes, max_nodes, excluded_props);

  cout << "Processing completed." << endl;

  return 0;
}