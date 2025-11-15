#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cmath>

using namespace std;

const double EPS = 1e-10;

//判断字符是否为运算符或分隔符
bool isOperatorOrDelimiter(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || 
           c == '(' || c == ')' || c == '=' || c == ',' ||
           c == ';' || c == ':' || c == '<' || c == '>';
}

// 判断字符是否为token边界（从非数字到数字，或从数字到非数字，或运算符）
bool isTokenBoundary(char current, char next) {
    // 当前字符是数字，下一个字符是非数字且不是小数点
    if (isdigit(current) && !isdigit(next) && next != '.') {
        return true;
    }
    // 当前字符是非数字且不是小数点，下一个字符是数字
    if (!isdigit(current) && current != '.' && isdigit(next)) {
        return true;
    }
    // 遇到运算符
    if (isOperatorOrDelimiter(current) || isOperatorOrDelimiter(next)) {
        return true;
    }
    return false;
}

// 分割字符串为token，正确处理数字和标识符的边界
vector<string> tokenize(const string& str) {
    vector<string> tokens;
    string currentToken;
    
    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];
        char next = (i + 1 < str.length()) ? str[i + 1] : '\0';
        
        // 如果是空白字符，结束当前token
        if (isspace(c) || c == '\t' || c == '\n' || c == '\r') {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            continue;
        }
        
        // 如果是运算符，结束当前token并添加运算符作为单独token
        if (isOperatorOrDelimiter(c)) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            tokens.push_back(string(1, c));
            continue;
        }
        
        // 如果当前token为空，直接添加字符
        if (currentToken.empty()) {
            currentToken += c;
        } 
        // 检查是否需要分割token（数字/标识符边界）
        else if (isTokenBoundary(currentToken.back(), c)) {
            tokens.push_back(currentToken);
            currentToken = c;
        }
        // 否则继续添加到当前token
        else {
            currentToken += c;
        }
    }
    
    // 添加最后一个token
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }
    
    return tokens;
}

// 判断字符串是否为浮点数
bool isFloat(const string& token) {
    if (token.empty()) return false;
    
    bool hasDot = false;
    bool hasDigit = false;
    
    for (char c : token) {
        if (c == '.') {
            if (hasDot) return false; // 多个小数点
            hasDot = true;
        } else if (isdigit(c)) {
            hasDigit = true;
        } else {
            return false; // 包含非数字字符
        }
    }
    
    return hasDigit; // 至少有一个数字
}

// 比较两个token是否满足精度要求
bool compareTokens(const string& token1, const string& token2, double precision) {
    // 如果两个token完全相同，直接返回true
    if (token1 == token2) return true;
    
    // 如果都是浮点数，比较数值
    if (isFloat(token1) && isFloat(token2)) {
        double num1 = stod(token1);
        double num2 = stod(token2);
        
        // 检查是否满足精度要求
        return fabs(fabs(num1 - num2) - precision)<= EPS;
    }
    
    // 其他情况
    return false;
}

// 比较两个字符串是否满足精度要求
bool compareWithPrecision(const string& y, const string& z, double precision) {
    auto tokensY = tokenize(y);
    auto tokensZ = tokenize(z);
    
    // token数量必须相同
    if (tokensY.size() != tokensZ.size()) {
        return false;
    }
    
    // 逐个比较token
    for (size_t i = 0; i < tokensY.size(); i++) {
        if (!compareTokens(tokensY[i], tokensZ[i], precision)) {
            return false;
        }
    }
    
    return true;
}


int main(){
    string input;
    string line;

    while(getline(cin,line)){
        input += line;
    }

    //以#为分隔符提取三段字符串
    size_t pos1 = input.find('#');
    size_t pos2 = input.find('#', pos1 + 1); //查找第二个#
    size_t pos3 = input.find('#', pos2 + 1); //查找第三个#
    std::string x_str = input.substr(pos1 + 1, pos2 - pos1 - 1);
    std::string y_str = input.substr(pos2 + 1, pos3 - pos2 - 1);
    std::string z_str = input.substr(pos3 + 1);

    //解析x
    double precision = stod(x_str);//精度

    if (compareWithPrecision(y_str, z_str, precision)) {
        cout << "Y" << endl;
    } else {
        cout << "N" << endl;
    }

    return 0;
}