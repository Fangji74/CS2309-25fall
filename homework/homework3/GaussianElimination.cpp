/*
任务描述：
编写程序，使用高斯消去法实现线性方程组求解，系数为整数或小数。
编程要求
（1）读入线性方程组
    程序的输入共有若干行，通过每一行输入，构建一个线性方程（如3 4 10 代表线性方程为3*x1+4*x2=10）。
（2）利用高斯消去法计算
    利用高斯消去法进行求解。
（3）输出结果
    （a）若有唯一解，依次输出所有变量的解，如果解为浮点数，数字最多保留至小数点后4位（截断或者四舍五入的方式均可）。
    （b）若方程组无解（例如，0*x1=5），则输出error1；
    （c）若方程组存在多个解（例如，0*x2=0），则输出error2；
*/


#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

const double EPS = 1e-10;

//格式化数字输出
string format_number(double num) {
  //处理0.0000的情况
  if (fabs(num) < 1e-10) {
    return "0";
  }

  //检查是否为整数
  if (fabs(num - round(num)) < 1e-10) {
    return to_string(static_cast<long long>(round(num)));
  }

  // 截取到4位小数
  double cut = static_cast<long long>(num * 10000) / 10000.0;

  //是小数，先格式化为4位小数
  stringstream temp;
  temp << fixed << setprecision(4) << cut;//此处setprecision会进行四舍五入
  string str = temp.str();

  //删除末尾的0
  size_t last_non_zero = str.find_last_not_of('0');
  if (last_non_zero != string::npos) {
    str = str.substr(0, last_non_zero + 1);
  }
  //删除最后是小数点的情况
  if (!str.empty() && str.back() == '.') {
    str.pop_back();
  }

  return str;
}

//高斯消元法求解线性方程组
string GaussianElimination(vector<vector<double>> &equations) {
  int equation_num = equations.size();        //方程个数
  int variable_num = equations[0].size() - 1; //变量个数

  // 前向消元
  int rank = 0;
  vector<int> pivot_cols; //记录主元列

  for (int col = 0; col < variable_num && rank < equation_num; ++col) {
    //选择剩余列最大值主元
    int pivot_row = rank;
    for (int i = rank + 1; i < equation_num; ++i) {
      if (fabs(equations[i][col]) > fabs(equations[pivot_row][col])) {
        pivot_row = i;
      }
    }

    //如果主元为0，跳过该列
    if (fabs(equations[pivot_row][col]) < EPS) {
      continue;
    }

    //交换行
    if (pivot_row != rank) {
      swap(equations[rank], equations[pivot_row]);
    }

    pivot_cols.push_back(col);

    // 消元
    for (int i = rank + 1; i < equation_num; ++i) {
      double factor = equations[i][col] / equations[rank][col];
      for (int j = col; j <= variable_num; ++j) {
        equations[i][j] -= factor * equations[rank][j];
      }
    }
    ++rank; //注意这里不能放循环++col后面，有continue
  }

  //检查error
  //先检查无解情况
  for (int i = rank; i < equation_num; ++i) {
    if (fabs(equations[i][variable_num]) > EPS) {
      return "error1";
    }
  }
  //然后检查多解情况
  if (rank < variable_num) {
    return "error2"; //有效方程数小于变量数，多解
  }

  //回代求解
  vector<double> solution(variable_num, 0.0);
  for (int i = rank - 1; i >= 0; --i) {
    int col = pivot_cols[i];
    solution[col] = equations[i][variable_num];
    for (int j = col + 1; j < variable_num; ++j) {
      solution[col] -= equations[i][j] * solution[j];
    }
    solution[col] /= equations[i][col];
  }

  //格式化输出
  stringstream ss;
  for (int i = 0; i < variable_num; ++i) {
    ss << format_number(solution[i]);
    if (i < variable_num - 1) {
      ss << " ";
    }
  }

  return ss.str();
}

int main() {
  vector<vector<double>> equations;
  string line;

  while (getline(cin, line)) {
    if (line.empty())
      continue;

    istringstream iss(line);
    vector<double> equation;
    double num;

    while (iss >> num) {
      equation.push_back(num);
    }

    if (!equation.empty()) {
      equations.push_back(equation);
    }
  }

  string result = GaussianElimination(equations);
  cout << result << endl;

  return 0;
}