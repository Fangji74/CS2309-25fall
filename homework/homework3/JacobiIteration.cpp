/*
题目：
（接上题高斯消元法求解线性方程组）
使用雅可比迭代法实现线性方程组求解，并比较和高斯消去法的差异。
注意：迭代过程中产生的差异，需要自行判断和处理。
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
  temp << fixed << setprecision(5) << cut;//此处setprecision会进行四舍五入
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

//高斯消元法求解线性方程组（雅可比迭代法无法收敛时改用）
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

//雅可比迭代法求解线性方程组
string JacobiIteration(vector<vector<double>> &equations) {
  int equation_num = equations.size();        //方程个数
  int variable_num = equations[0].size() - 1; //变量个数
  int max_iter_time = 100;//最大迭代次数
  double delta = 1e-5;//精度
  vector<double> solution_old(variable_num, 0.0);//上一次迭代的解集（初始）
  vector<double> solution_new(variable_num, 0.0);//当前解集
  bool iter_flag = true;

  if(equation_num<variable_num) return "error2";//此处不严谨的简单判断，正确判断调用高斯消元法
  if(equation_num>variable_num) return "error1";//此处不严谨的简单判断，正确判断调用高斯消元法

  int t=0;//迭代次数
  for(t=0;t<max_iter_time;t++){
    solution_old = solution_new;
    for(int i=0;i<variable_num;i++){
        //计算累计和
        double sum=0;
        for(int j=0;j<variable_num;j++){
            if(j==i) continue;//无j==i对应的a项
            sum += equations[i][j] * solution_old[j];
        }
        solution_new[i] = (equations[i][equation_num]-sum)/equations[i][i];
    }
    //判断两次迭代间元素差是否都小于delta
    iter_flag = false;
    for(int i=0;i<variable_num;i++){
        if(fabs(solution_new[i]-solution_old[i])>delta){
            iter_flag = true;
            break;
        }
    }
    if(!iter_flag) break;//精度达到要求
  }

  if(t==max_iter_time){
    //未收敛，改用高斯消元法
     return GaussianElimination(equations);
  }



  //格式化输出
  stringstream ss;
  for (int i = 0; i < variable_num; ++i) {
    ss << format_number(solution_old[i]);
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
  //equations = {{1,2,5},{1,1,1}};//手动测试

  string result = JacobiIteration(equations);
  cout << result << endl;

  return 0;
}