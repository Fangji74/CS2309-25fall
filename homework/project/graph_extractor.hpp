#pragma once

#include <bitset>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "utils.hpp"

using json = nlohmann::json;
using namespace std;

const int MAX_NODES = 128;

class GraphExtractor {
public:
  GraphExtractor();
  //外部接口 path:输入路径 min_nodes:最小节点数 max_nodes:最大节点数
  // excluded_props:排除的属性列表
  void process(const string &path, int min_nodes, int max_nodes,
               const vector<string> &excluded_props = {});

private:
  //从json构建图
  void build_graph(const json &nodes, const vector<string> &excluded_props);
  //提取满足节点数要求的子图
  vector<unordered_set<std::string>> extractSubgraphs(int min_nodes,
                                                      int max_nodes);
  // 生成子图JSON文件
  void generateSubgraphFile(const unordered_set<string> &subgraph_nodes,
                            const vector<string> &excluded_props,
                            const string &base_filename, int node_count,
                            int index);
  //工具函数
  //获取节点的邻居节点ID列表，排除指定属性
  vector<string> getNeighbors(const json &node,
                              const unordered_set<string> &excluded_set,
                              const string &currentNodeId);
  //获取节点的邻居节点ID列表函数的递归函数
  void findIdsDeep(const json &j, vector<string> &neighbors,
                   const unordered_set<string> &excluded_set,
                   const string &currentNodeId);
  //将排除属性列表转换为字符串（用于文件命名）
  string excluded_props2str(const vector<string> &excluded_props);
  //从start_nodes中的结点开始找到其闭包
  unordered_set<string> getClosure(const unordered_set<string> &start_nodes,
                                   int max_nodes);
  //辅助函数：判定两个集合是否具有弱连通关系（通过交集判定，因为它们都是封闭的）
  bool hasInteraction(const unordered_set<string> &s1,
                      const unordered_set<string> &s2);

  bitset<MAX_NODES> getClosureInt(int start_node_idx, int max_nodes);

  //图的数据结构
  unordered_map<string, json> node_map; //节点ID到节点数据的映射
  unordered_map<string, vector<string>> adjacency_list; //邻接表表示图
  unordered_map<string, int> id_to_idx; //节点ID到ID索引的映射
  vector<string> idx_to_id;             // ID索引到节点ID到映射
};