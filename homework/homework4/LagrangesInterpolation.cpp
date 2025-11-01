/*
题目：利用插值法进行预测

描述：
现在给出 f(x) 上的一组点，例如(−1, −3), (0, 1), (1, 1), (2, 3), (6, 151)。
请使用拉格朗日插值法，根据这些点构造插值多项式 P (x)（即 P (x) 会通过所有给定点）。使用构造的插值多项式 P (x)，输入整数xx，预测 x = xx 时的函数值。

注意：
（1）给定n个点，则多项式的最高项为x^(n-1)；
（2）输入若干行，第一行为xx，其余若干行均为输入的若干点值x y；
（3）xx比输入的点的x坐标值都大；
（4）输入的点数不超过10个。
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

string LagrangesInterpolation(const vector<vector<double>>& equations,double x){
    double result = 0.0;
    int n = equations.size();
    
    for (int i = 0; i < n; i++) {
        double term = equations[i][1];  // 初始化为y_i
        
        // 计算拉格朗日函数值
        for (int j = 0; j < n; j++) {
            if (i != j) {
                term *= (x - equations[j][0]) / (equations[i][0] - equations[j][0]);
            }
        }
        
        result += term;
    }
    
    return format_number(result);
}

int main() {
  vector<vector<double>> equations;
  string line;
  double predict_x;//待预测的值
  cin>>predict_x;

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

  string result = LagrangesInterpolation(equations,predict_x);
  cout << result << endl;

  return 0;
}