/*
任务描述：
判断所给的一组<x, y>是否满足线性关系，若满足线性关系，使用一元线性回归法拟合出x，y的线性回归方程，并通过回归方程预测x=n（n为给出值）时对应的y值。

编程要求
已知输入数据皆不超过小数点后4位，浮点数类型默认为double，计算结果采用截取方式，输出保留小数点后4位小数（如果末位为0，则可以省略，具体不做要求）。

用户输入输出规范如下：

输入：输入总共n+1行。第一行为待计算的x值，接下来的n行为n个点坐标，每行为两个数字，中间使用空格隔开，分别表示坐标的x,y值。输入行数不超过11行。输入的坐标集至少由1个坐标组成，每行一个坐标。
例如 第一行输入0.4235，第二行输入0.4 0.3894，表示f(0.4)=0.3894。

输出：输出分为三行。第一行为相关系数值，第二行为回归方程，第三行为x=n时带入回归方程求出的y值。

例如，第一行输出相关系数为0.88，第二行输出回归方程为y=3x+1，第三行为x=1时的预测值输出 4。

0.88
y=3*x+1
4

若相关系数r的绝对值大于等于0.75时认为两个变量有很强的线性相关关系，若相关系数小于0.75则无法求出一元线性拟合方程，第二行输出error，第三行输出error：
0.5
error
error
注意：

输出结果保留四位小数（如果浮点数最后位为0，则可以被省略，具体不做要求）。
测试说明
平台会对你编写的代码进行测试：

测试输入：
0.4235
0.4 0.3894
0.5 0.4794

预期输出：
1
y=0.9*x+0.0294
0.4105
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

  // 四舍五入到5位小数（根据答案样例推测的精度保留方式）
  double rounded = round(num * 100000) / 100000.0;

  // 截取到4位小数
  double cut = static_cast<long long>(rounded * 10000) / 10000.0;

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

string expr_cat(string&& b,string&& a){
    string expr;
    if(b=="0"){
        if(a=="0") return "y=0";
        else{
            return "y="+a;
        }
    }
    else{
        if(a=="0") return "y="+b+"*x";
        else{
            if(a[0]=='-'){
                return "y="+b+"*x"+a;
            }
            else return "y="+b+"*x+"+a;
        }
    }
}

string LinearRegression(double predict_x, vector<vector<double>> &equations) {
  string result_expr;       //结果表达式
  double a;                 //系数a
  double b;                 //系数b
  string result_pred;       //结果预测值
  double r;                 //相关系数
  string result_r;          //结果相关系数
  int n = equations.size(); //数据组数
  double sum_x = 0;         // x的一次和
  double sum_y = 0;         // y的一次和
  double sum_x2 = 0;        // x的二次和
  double sum_y2 = 0;        // y的二次和
  double sum_xy = 0;        // xy的和

  for (int i = 0; i < n; i++) {
    sum_x += equations[i][0];
    sum_y += equations[i][1];
    sum_x2 += equations[i][0] * equations[i][0];
    sum_y2 += equations[i][1] * equations[i][1];
    sum_xy += equations[i][0] * equations[i][1];
  }

  double Lxx = sum_x2 - sum_x * sum_x / n;
  double Lyy = sum_y2 - sum_y * sum_y / n;
  double Lxy = sum_xy - sum_x * sum_y / n;

  b = Lxy / Lxx;
  a = (sum_y  - b * sum_x)/n;
  r = Lxy / sqrt(Lxx * Lyy);
  result_r = format_number1(r);

  //根据相关系数判断
  if (fabs(r) < 0.75) {
    result_expr = "error";
    result_pred = "error";
  } else {
    result_expr = expr_cat(format_number(b),format_number(a));
    result_pred = format_number(b * predict_x + a);
  }

  return result_r + "\n" + result_expr + "\n" + result_pred;
}

int main() {
  vector<vector<double>> equations; //输入的一系列x,y
  double predict_x;                 //待预测的x值
  string line;

  cin >> predict_x;
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

  string result = LinearRegression(predict_x, equations);
  cout << result << endl;

  return 0;
}