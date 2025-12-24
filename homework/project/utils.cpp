/*
工具函数库
*/

#include "utils.hpp"
#include <algorithm>
#include <string>
#include <unordered_set>
#include <vector>

//识别一个字符串是否为id
bool isId(std::string &str) {
  bool num_flag = 0;  //标记是否出现数字
  bool char_flag = 0; //标记是否出现字母
  for (char c : str) {
    if (!((c <= '9' && c >= '0') || (c <= 'z' && c >= 'a') || (c == '.'))) {
      return 0; //检查到id字符串中不可能出现的值
    }
    if ((c <= '9' && c >= '0')) {
      num_flag = 1;
    } else if ((c <= 'z' && c >= 'a')) {
      char_flag = 1;
    } else if (c == '.') {
      //空语句
    } else {
      return 0; //其他字符直接返回0
    }
  }

  if (num_flag && char_flag &&
      (str.length() == 15 || str.length() == 16)) { //观察样例发现id长度为15或16
    return 1;
  } else {
    return 0;
  }
}

// 将图转换为字符串哈希值
std::string getSetHash(const std::unordered_set<std::string> &node_set) {
  //排序
  std::vector<std::string> sorted_nodes(node_set.begin(), node_set.end());
  sort(sorted_nodes.begin(), sorted_nodes.end());

  //拼接成唯一字符串
  std::string hash;
  for (const auto &node : sorted_nodes) {
    hash += node + "|"; // 用|分隔，避免歧义
  }
  return hash;
}