/*
题目：判断连线交叉及计算封闭多边形面积

描述：
继续上面题目，我们假设将所有输入点以及预测点依次相连，并且预测点与第一个相连，我们可以形成一个多边形。请采用计算几何的方式，判断该多边形是否存在连线交叉。如果存在连线交叉，则输出cross；如果不存在连线交叉，则计算多边形的面积。
*/

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

const double EPS = 1e-10;

struct Point {
  double x, y;
  Point(double x = 0, double y = 0) : x(x), y(y) {}
};

//向量计算
Point get_vector(const Point& a, const Point& b) {
    return Point(b.x - a.x, b.y - a.y);
}

//向量叉积计算
double cross(const Point& a, const Point& b) {
    return a.x * b.y - a.y * b.x;
}

//判断线段是否相交
bool isSegmentsIntersect(const Point& p1, const Point& p2, const Point& p3, const Point& p4) {
    //快速排斥实验
    if (max(p1.x, p2.x) < min(p3.x, p4.x) || max(p3.x, p4.x) < min(p1.x, p2.x) ||
        max(p1.y, p2.y) < min(p3.y, p4.y) || max(p3.y, p4.y) < min(p1.y, p2.y)) {
        return false;
    }
    
    //跨立实验
    double c1 = cross(get_vector(p1, p3), get_vector(p1, p2));
    double c2 = cross(get_vector(p1, p4), get_vector(p1, p2));
    double c3 = cross(get_vector(p3, p1), get_vector(p3, p4));
    double c4 = cross(get_vector(p3, p2), get_vector(p3, p4));
    
    //处理共线情况
    if (fabs(c1) < EPS && fabs(c2) < EPS && fabs(c3) < EPS && fabs(c4) < EPS) {
        //共线，检查是否有重叠
        return !(max(p1.x, p2.x) < min(p3.x, p4.x) || max(p3.x, p4.x) < min(p1.x, p2.x) ||
                max(p1.y, p2.y) < min(p3.y, p4.y) || max(p3.y, p4.y) < min(p1.y, p2.y));
    }
    
    return (c1 * c2 <= 0) && (c3 * c4 <= 0);
}

// 判断多边形是否自相交
bool isSelfIntersect(const vector<Point>& poly) {
    int n = poly.size();//边数
    
    for(int i=0;i<n;i++){
        Point p1 = poly[i];
        Point p2 = poly[(i+1)%n];//注意首尾

        for(int j=i+2;j<n;j++){//自己和邻边不需要检查
            if(j==(i+1)%n || (j+1)%n==i) continue;//排除两条邻边

            Point p3 = poly[j];
            Point p4 = poly[(j+1)%n];
            if(isSegmentsIntersect(p1, p2, p3, p4))
                return true;
        }
    }
    return false;
}

//通过高斯面积公式计算多边形面积
double CalculateArea(const vector<Point>& poly) {
    int n = poly.size();
    double area = 0.0;
    
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        area += poly[i].x * poly[j].y - poly[j].x * poly[i].y;
    }
    
    return fabs(area) / 2.0;
}

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

  //是小数，先格式化24位小数
  stringstream temp;
  temp << fixed << setprecision(2) << num; //此处setprecision会进行四舍五入
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
/*
7
-1 -3
0 1 
2 3
6 151
*/

double LagrangesInterpolation(const vector<vector<double>> &equations,
                              double x) {
  double result = 0.0;
  int n = equations.size();

  for (int i = 0; i < n; i++) {
    double term = equations[i][1]; // 初始化为y_i

    // 计算拉格朗日函数值
    for (int j = 0; j < n; j++) {
      if (i != j) {
        term *= (x - equations[j][0]) / (equations[i][0] - equations[j][0]);
      }
    }

    result += term;
  }

  return result;
}

int main() {
  vector<vector<double>> equations;
  string line;
  double predict_x;

  getline(cin, line);
  istringstream iss_first(line);
  iss_first >> predict_x;
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

  double predict_y = LagrangesInterpolation(equations, predict_x);

  //将vector<vector<double>>的equations和predict_x,y重组成新的vector<Point>
  vector<Point> points;
  points.push_back(Point(predict_x, predict_y));
  for (int i = 0; i < equations.size(); i++) {
    points.push_back(Point(equations[i][0], equations[i][1]));
  }

  if(isSelfIntersect(points))
    cout<<"cross"<<endl;
  else
    cout<<format_number(CalculateArea(points))<<endl;

  return 0;
}