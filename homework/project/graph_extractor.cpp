#include "graph_extractor.hpp"
#include <algorithm>
#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>

GraphExtractor::GraphExtractor() {}

void GraphExtractor::process(const string &path, int min_nodes, int max_nodes,
                             const vector<string> &excluded_props) {
  namespace fs = std::filesystem;
  fs::path target_path(path); //输入路径（目录）
  cout << "Processing path: " << path << endl;
  cout << "Minimum nodes: " << min_nodes << ", Maximum nodes: " << max_nodes
       << endl;
  cout << "Excluded properties: " << excluded_props2str(excluded_props) << endl;
  cout << "--------------------Begin--------------------" << endl;

  try {
    if (fs::is_directory(target_path)) {
      //处理目录下所有json文件，此处支持递归目录
      for (const auto &entry : fs::recursive_directory_iterator(target_path)) {
        if ((entry.is_regular_file()) &&
            (entry.path().extension() == ".json")) { //只处理json文件
          //读取json文件
          ifstream file(entry.path());
          json data = json::parse(file);
          file.close();

          //构建图
          build_graph(data, excluded_props);

          //提取子图
          auto subgraphs = extractSubgraphs(min_nodes, max_nodes);

          //生成子图文件，仍然放置在原目录下
          //使用map记录每个大小的当前索引
          unordered_map<int, int> size_to_index;
          for (int i = 0; i < subgraphs.size(); ++i) {
            int size = subgraphs[i].size();
            //初始化该大小的索引（如果不存在）
            if (size_to_index.find(size) == size_to_index.end()) {
              size_to_index[size] = 1;
            } else {
              size_to_index[size]++;
            }
            cout << size << ":" << size_to_index[size] << endl;
            generateSubgraphFile(subgraphs[i], excluded_props,
                                 entry.path().stem().string(),
                                 subgraphs[i].size(), size_to_index[size]);
          }
        }
      }
    }
    //处理单个json文件
    else {
      ifstream file(target_path);
      json data = json::parse(file);
      file.close();

      //构建图
      build_graph(data, excluded_props);

      //提取子图
      auto subgraphs = extractSubgraphs(min_nodes, max_nodes);
      //生成子图文件，仍然放置在原目录下
      //使用map记录每个大小的当前索引
      unordered_map<int, int> size_to_index;
      for (int i = 0; i < subgraphs.size(); ++i) {
        int size = subgraphs[i].size();
        //初始化该大小的索引（如果不存在）
        if (size_to_index.find(size) == size_to_index.end()) {
          size_to_index[size] = 1;
        } else {
          size_to_index[size]++;
        }
        generateSubgraphFile(subgraphs[i], excluded_props,
                             target_path.stem().string(), subgraphs[i].size(),
                             size_to_index[size]);
      }
    }
    cout << "Output path: " << path << endl;
    cout << "--------------------End--------------------" << endl;
  } catch (const exception &e) {
    cerr << "Error processing path " << path << ": " << e.what() << std::endl;
  }
}

void GraphExtractor::build_graph(const json &nodes,
                                 const vector<string> &excluded_props) {
  //清空之前的数据
  node_map.clear();
  adjacency_list.clear();

  //使用unordered_set存储排除的属性同时去重
  unordered_set<string> excluded_set(excluded_props.begin(),
                                     excluded_props.end());

  //构建节点映射
  for (const auto &item : nodes) {
    if (item.find("id") == item.end() || !item["id"].is_string()) {
      continue; //跳过没有id或id不是字符串的节点
    }
    string node_id = item["id"];
    node_map[node_id] = item;
  }

  //构建邻接表
  for (const auto &node : nodes) {
    if (node.find("id") == node.end() || !node["id"].is_string()) {
      continue; //跳过没有id或id不是字符串的节点
    }
    string node_id = node["id"];
    vector<string> neighbors; //存储当前节点指向的邻居节点ID
    neighbors = getNeighbors(node, excluded_set, node["id"]);
    adjacency_list[node_id] = neighbors;
  }
}

bool GraphExtractor::hasInteraction(const unordered_set<string> &s1,
                                    const unordered_set<string> &s2) {
  // 性能优化：遍历较小的集合
  if (s1.size() > s2.size())
    return hasInteraction(s2, s1);
  for (const auto &item : s1) {
    if (s2.count(item))
      return true;
  }
  return false;
}

// vector<unordered_set<string>> GraphExtractor::extractSubgraphs(int min_nodes,
// int max_nodes) {
//     vector<unordered_set<string>> all_results;
//     unordered_set<string> visited_hashes;

//     //生成所有唯一的单源闭包
//     vector<unordered_set<string>> base_closures;
//     unordered_set<string> base_hashes;

//     for (const auto& [node_id, _] : node_map) {
//         unordered_set<string> closure = getClosure({node_id}, max_nodes);
//         if (!closure.empty()) {//getClosure超限返回空
//             string h = getSetHash(closure);
//             if (base_hashes.find(h) == base_hashes.end()) {
//                 base_hashes.insert(h);
//                 base_closures.push_back(closure);
//             }
//         }
//     }

//     //定义回溯Lambda函数进行组合搜索
//     //current_set:当前组合成的子图,start_index:base_closures的遍历起点
//     auto backtrack = [&](auto self, unordered_set<string> current_set, int
//     start_index) -> void {
//         //检查大小并记录合法结果
//         if (current_set.size() >= min_nodes && current_set.size() <=
//         max_nodes) {
//             string h = getSetHash(current_set);
//             if (visited_hashes.find(h) == visited_hashes.end()) {
//                 visited_hashes.insert(h);
//                 all_results.push_back(current_set);
//             }
//         } else if (current_set.size() > max_nodes) {
//             return; //剪枝：超过最大节点数
//         }

//         //尝试合并后续的闭包
//         for (int i = start_index; i < base_closures.size(); ++i) {
//             //如果两个封闭子图有交集，它们的并集一定是弱连通且封闭的
//             if (hasInteraction(current_set, base_closures[i])) {
//                 //合并
//                 unordered_set<string> next_set = current_set;
//                 next_set.insert(base_closures[i].begin(),
//                 base_closures[i].end());

//                 //递归（只有在合并后确实增加了新元素，且未超限时继续）
//                 if (next_set.size() > current_set.size() && next_set.size()
//                 <= max_nodes) {
//                     self(self, next_set, i + 1);
//                 }
//             }
//         }
//     };

//     //以每一个基础闭包为起点开始搜索
//     for (int i = 0; i < base_closures.size(); ++i) {
//         backtrack(backtrack, base_closures[i], i + 1);
//     }

//     return all_results;
// }

//使用bitset优化的版本
vector<unordered_set<string>> GraphExtractor::extractSubgraphs(int min_nodes,
                                                               int max_nodes) {
  //减少字符串id匹配，映射为idx（int）
  id_to_idx.clear();
  idx_to_id.clear();
  int n = 0;
  for (const auto &[node_id, _] : node_map) {
    id_to_idx[node_id] = n++;
    idx_to_id.push_back(node_id);
  }

  //得到所有单源闭包(使用bitset方便后续幂集运算)
  vector<bitset<MAX_NODES>> base_closures;
  for (int i = 0; i < n; ++i) {
    bitset<MAX_NODES> closure = getClosureInt(i, max_nodes);
    if (closure.any()) {
      // 简单的去重：如果这个闭包已经存在，就不重复添加
      bool exists = false;
      for (const auto &bc : base_closures) {
        if (bc == closure) {
          exists = true;
          break;
        }
      }
      if (!exists)
        base_closures.push_back(closure);
    }
  }

  //计算所有符合条件的幂集
  vector<bitset<MAX_NODES>> result_bitsets; //用于记录已经发现的组合，方便遍历
  unordered_set<bitset<MAX_NODES>> visited_combinations; //用于快速查重

  //回溯函数
  auto backtrack = [&](auto self, bitset<MAX_NODES> current_set,
                       int start_idx) -> void {
    int sz = current_set.count();
    if (sz >= min_nodes && sz <= max_nodes) {
      if (visited_combinations.find(current_set) ==
          visited_combinations.end()) {
        visited_combinations.insert(current_set);
        result_bitsets.push_back(current_set);
      }
    }

    for (int i = start_idx; i < base_closures.size(); ++i) {
      //弱连通判定：交集不为空
      if ((current_set & base_closures[i]).any()) {
        bitset<MAX_NODES> next_set = current_set | base_closures[i];
        if (next_set.count() <= max_nodes && next_set != current_set) {
          self(self, next_set, i + 1);
        }
      }
    }
  };

  for (int i = 0; i < base_closures.size(); ++i) {
    backtrack(backtrack, base_closures[i], i + 1);
  }

  //映射回string类型的id
  vector<unordered_set<string>> final_output;
  for (const auto &b : result_bitsets) {
    unordered_set<string> res_strings;
    for (int i = 0; i < n; ++i) {
      if (b.test(i))
        res_strings.insert(idx_to_id[i]);
    }
    final_output.push_back(res_strings);
  }

  return final_output;
}

// //参数max_nodes用于提前退出减少寻找次数（原版本）
// unordered_set<string> GraphExtractor::getClosure(const unordered_set<string>&
// start_nodes, int max_nodes) {
//     unordered_set<string> closure = start_nodes;
//     bool changed = true;

//     //不断添加缺失的节点直到封闭
//     while (changed) {
//         //结点数量超过max_nodes的话直接返回空集合
//         if(closure.size()>max_nodes){
//             return unordered_set<string> ();
//         }
//         changed = false;
//         unordered_set<string> to_add;
//         //检查当前closure中每个节点的所有出边
//         for (const auto& node : closure) {
//             if (adjacency_list.find(node) != adjacency_list.end())
//             {//找到了邻接结点
//                 for (const auto& neighbor : adjacency_list[node]) {
//                     //如果邻居不在closure中，并且该邻居在node_map中，需要添加
//                     if (closure.find(neighbor) == closure.end() &&
//                     node_map.find(neighbor) != node_map.end()) {
//                         to_add.insert(neighbor);
//                         changed = true;
//                     }
//                 }
//             }
//         }

//         //添加缺失的节点
//         closure.insert(to_add.begin(), to_add.end());
//     }

//     return closure;
// }

//使用bitset优化的找单源闭包函数，参数max_nodes用于提前退出减少寻找次数
bitset<MAX_NODES> GraphExtractor::getClosureInt(int start_node_idx,
                                                int max_nodes) {
  bitset<MAX_NODES> closure;
  vector<int> q;
  q.push_back(start_node_idx); //仍需遍历结点的队列
  closure.set(start_node_idx);

  // BFS
  int head = 0;
  while (head < q.size()) {
    if (closure.count() > max_nodes)
      return bitset<MAX_NODES>(); //返回空表示无效

    int curr = q[head++];
    string curr_id = idx_to_id[curr];
    if (adjacency_list.count(curr_id)) {
      for (const auto &neighbor_id : adjacency_list.at(curr_id)) {
        int n_idx = id_to_idx[neighbor_id];
        if (!closure.test(n_idx)) { //检查对应位数是否为1
          closure.set(n_idx);       //置1
          q.push_back(n_idx);
        }
      }
    }
  }
  return closure;
}

void GraphExtractor::generateSubgraphFile(
    const unordered_set<string> &subgraph_nodes,
    const vector<string> &excluded_props, const string &base_filename,
    int node_count, int index) {
  json output = json::array();
  //填充节点数据
  for (const auto &node_id : subgraph_nodes) {
    if (node_map.find(node_id) != node_map.end()) {
      output.push_back(node_map[node_id]);
    } else {
      cout << node_id << endl;
    }
  }

  //创建目录（如果不存在）
  filesystem::path output_path = filesystem::path("result");
  filesystem::create_directories(output_path);
  //生成子图JSON文件名
  string output_filename =
      "module_" + base_filename + "_" + excluded_props2str(excluded_props) +
      to_string(node_count) + "_" + to_string(index) + ".json";
  filesystem::path full_path = output_path / output_filename;
  //写入文件
  ofstream outfile(full_path);
  outfile << output.dump(4); //格式化输出，缩进4个空格
  outfile.close();

  cout << "Generated subgraph file: " << full_path << endl;
}

vector<string>
GraphExtractor::getNeighbors(const json &node,
                             const unordered_set<string> &excluded_set,
                             const string &currentNodeId) {
  vector<string> neighbors;

  if (!node.is_object())
    return neighbors;

  // 深度优先遍历整个 JSON，寻找所有字符串形式的 id
  findIdsDeep(node, neighbors, excluded_set, currentNodeId);

  // 去重（因为可能在不同位置引用同一个 id）
  sort(neighbors.begin(), neighbors.end());
  neighbors.erase(unique(neighbors.begin(), neighbors.end()), neighbors.end());

  return neighbors;
}

//递归查找所有id
void GraphExtractor::findIdsDeep(const json &j, vector<string> &neighbors,
                                 const unordered_set<string> &excluded_set,
                                 const string &currentNodeId) {

  if (j.is_object()) {
    for (auto &[key, value] : j.items()) {
      //如果这个 key 在被排除的属性集合中，跳过整个子树
      if (excluded_set.find(key) != excluded_set.end()) {
        continue; // 跳过这个属性及其所有子属性
      }

      //处理当前值
      if (value.is_string()) {
        string str = value.get<string>();
        //先进行提取，将可能是id的字符串提取出来
        for(auto &it:node_map){
          if(str.find(it.first) != string::npos){
            str = it.first;
            break;
          }
        }
        //检查是否是id且不是自身且在node_map中
        if (isId(str) && str != currentNodeId &&
            node_map.find(str) != node_map.end()) {
          neighbors.push_back(str);
        }
      }
      //递归处理嵌套结构
      else if (value.is_object() || value.is_array()) {
        findIdsDeep(value, neighbors, excluded_set, currentNodeId);
      }
    }
  } else if (j.is_array()) {
    for (auto &elem : j) {
      if (elem.is_object() || elem.is_array()) {
        findIdsDeep(elem, neighbors, excluded_set, currentNodeId);
      } else if (elem.is_string()) {
        string str = elem.get<string>();
        //先进行提取，将可能是id的字符串提取出来
        for(auto &it:node_map){
          if(str.find(it.first) != string::npos){
            str = it.first;
          }
        }
        if (isId(str) && str != currentNodeId) {
          neighbors.push_back(str);
        }
      }
    }
  }
  //基本类型（非对象、非数组）不需要进一步处理
}

//输出排除属性，下划线分隔
string
GraphExtractor::excluded_props2str(const vector<string> &excluded_props) {
  if (excluded_props.empty()) {
    return "";
  }
  string result;
  for (const auto &prop : excluded_props) {
    result += prop + "_";
  }
  return result;
}