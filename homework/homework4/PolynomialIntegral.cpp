/*
题目：计算定值积分

描述：
针对上一题，选用任意方式定值积分，即计算P(x)从a积分到b的值，其中，a为输入点中最小的x，b为输入点中最大的x，P(x)为构造的插值多项式。 

注意，检查时因为积分计算方式的不一样，我们将手工检查浮点数的误差，不以测试数据是否通过为准；为便于结果比较，输出中，小数后取两位，四舍五入即可。
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

  //是小数，先格式化为2位小数
  stringstream temp;
  temp << fixed << setprecision(2) << num;//此处setprecision会进行四舍五入
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

string LagrangesIntergral(const vector<vector<double>>& equations){
    int n = equations.size();
    double min_x=equations[0][0];//最小的x
    double max_x=equations[0][0];//最大的x
    double integral = 0.0;//积分值

    for(int i=0;i<n;i++){
        min_x = min(min_x,equations[i][0]);
        max_x = max(max_x,equations[i][0]);
    }
    
    //对拉格朗日函数每个子式进行积分并累加
    for (int i = 0; i < n; i++) {
        double y_i = equations[i][1];
        
        //计算第i个拉格朗日基函数的系数
        vector<double> coef(n, 0.0);//每个子式系数列表，coef[0]为常数项
        coef[0] = 1.0;
        
        double denominator = 1.0;//分母
        
        //求子式的多项式系数 多项式乘法：(x - x_j) * poly = x * poly - x_j * poly
        for (int j = 0; j < n; j++) {
            if (i != j) {
                denominator *= (equations[i][0] - equations[j][0]);
                vector<double> temp(n, 0.0);
                for (int k = 0; k < n-1; k++) {
                    temp[k+1] = coef[k];  // x * poly
                }
                for (int k = 0; k < n; k++) {
                    temp[k] -= equations[j][0] * coef[k];  // -x_j * poly
                }
                coef = temp;
            }
        }
        
        //对子式进行积分并乘以y_i/denominator
        for (int k = 0; k < n; k++) {
            if (abs(coef[k]) > EPS) {  //为0时
                // x^k积分为x^(k+1)/(k+1)
                double term = coef[k] / denominator * y_i;
                integral += term * (pow(max_x, k+1) - pow(min_x, k+1)) / (k+1);
            }
        }
    }
    
    return format_number(integral);
}

int main() {
  vector<vector<double>> equations;
  string line;
  double predict_x;

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

  string result = LagrangesIntergral(equations);
  cout << result << endl;

  return 0;
}