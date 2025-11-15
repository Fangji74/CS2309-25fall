#include <algorithm>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

vector<string> readLine(const string &line) {
  vector<string> columns;
  stringstream ss(line);
  string column;

  // 按制表符分割
  while (getline(ss, column, '\t')) {
    columns.push_back(column);
  }

  return columns;
}

bool compareChinesePinyin(const std::string &a, const std::string &b) {
  std::locale loc("zh_CN.UTF-8");
  const std::collate<char> &coll = std::use_facet<std::collate<char>>(loc);
  return coll.compare(a.data(), a.data() + a.size(), b.data(),
                      b.data() + b.size()) < 0;
}


void sort_table(vector<vector<string>> &table, int sort_col,
                const string &sort_order) {
  // 使用匿名函数作为排序依据
  sort(
      table.begin(), table.end(),
      [sort_col, sort_order](const vector<string> &a, const vector<string> &b) {
        if (sort_order == "ascend") {
          return a[sort_col-1] < b[sort_col-1];
        } else {
          return b[sort_col-1] < a[sort_col-1];
        }
      });;
}

// 输出表格
void printTable(const vector<vector<string>> &table) {
  for (const auto &row : table) {
    for (size_t i = 0; i < row.size(); i++) {
      cout << row[i];
      if (i < row.size() - 1) {
        cout << "\t";
      }
    }
    cout << endl;
  }
}

int main() {
  int sort_col;
  string sort_order;
  cin >> sort_col >> sort_order;

  vector<vector<string>> table;
  string line;

  while (getline(cin, line)) {
    if (line.empty())
      continue;
    vector<string> table_line = readLine(line);
    table.push_back(table_line);
  }

  sort_table(table, sort_col, sort_order);
  printTable(table);

  return 0;
}