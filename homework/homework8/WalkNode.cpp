#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main() {
  // 处理输入
  vector<vector<int>> nodes;
  string line;

  while (getline(cin, line)) {
    if (line.empty())
      continue;

    istringstream iss(line);
    vector<int> nodeline;
    int num;

    while (iss >> num) {
      nodeline.push_back(num);
    }

    if (!nodeline.empty()) {
      nodes.push_back(nodeline);
    }
  }

  int n = nodes.size(); // 行数

  int memo[n][n]; // 备忘录 -1代表未记录
  // 初始化备忘录
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      memo[i][j] = -1;
    }
  }

  // 采用循环方式实现
  for (int i = 0; i < n; i++) {
    for (int j = 0; j <= i; j++) {
      // 基本情况
      if (i == 0 && j == 0) {
        memo[i][j] = nodes[i][j];
      } else {
        // 边界情况
        int left_parent = (j - 1 >= 0) ? memo[i - 1][j - 1] : 0;
        int right_parent = (j <= i - 1) ? memo[i - 1][j] : 0;
        memo[i][j] = max(left_parent, right_parent) + nodes[i][j];
      }
    }
  }

  // 找到最大值
  int max_value = 0;
  for (int j = 0; j < n; j++) {
    if (memo[n - 1][j] > max_value) {
      max_value = memo[n - 1][j];
    }
  }

  cout << max_value << endl;

  return 0;
}