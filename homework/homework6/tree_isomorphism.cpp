/*
问题描述：
我们可以利用0-1序列代表一棵树或者其结点编码，其中，叶子结点的编码为10，其它非叶子结点编码为1XY...0，这里XY...代表所有子结点编号的连接。因此，一个根结点的编码也可以代表整棵树的编码。此外，给定一个上述编码，也可以构建一棵相应的树。

给定两棵树T1和T2。如果T1可以通过若干次子结点的顺序互换就变成T2，则我们称两棵树是“同构”的。

输入两棵树的编码，判断其对应的两棵树是否同构。

输入描述：
输入包含两行，每行一个字符串。每行代表一棵树的结构，字符串仅包含字符 1 和 0，且字符串长度满足：1 ≤ len ≤ 1000。

输出描述：输出判断结果：

如果两棵树同构，输出 "isomorphic"；
否则，输出 "non-isomorphic"。
如果输入不是合法的树，输出error
提示：树的表示形式参照课件树图同构一节，以0-1字符串形式输入，字符串代表由1、0分别替代(、)的树的括号序表示。例如，11011000和11100100可表示一对同构的树。
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// 判断输入是否合法
bool isValid(string& tree_str){
    // 此处判断1和0能否正常匹配
    int bal = 0;
    for(char c:tree_str){
        if(c=='1') bal++;
        else if(c=='0') bal--;
        else return false;// 非0非1直接返回false

        if(bal < 0) return false;
    }
    return bal==0;
}

// 将输入的树转换为按一定条件的表示
string normalize(const string& tree_str){
    string normalize_tree = "(";
    if (tree_str == "10") return "L"; // 叶子节点用L表示

    string inner = tree_str.substr(1, tree_str.length() - 2); // 去掉首尾

    vector<string> children;
    int start = 0;
    int bal = 0;
    for (int i = 0; i < inner.length(); i++) {
        if (inner[i] == '1') bal++;
        else bal--;
        if (bal == 0) {
            // 找到一个完整的子节点编码
            children.push_back(inner.substr(start, i - start + 1));
            start = i + 1;
        }
    }

    // 递归规范化每个子节点
    vector<string> normalizedChildren;
    for (const string& child : children) {
        normalizedChildren.push_back(normalize(child));
    }

    // 处理同构关系，可以简单地通过字符串排序以使顺序无关
    sort(normalizedChildren.begin(), normalizedChildren.end());

    // 拼接为转换好的字符串
    for (const string& nc : normalizedChildren) {
        normalize_tree += nc;
    }
    normalize_tree += ")";
    return normalize_tree;
}

int main(){
    string tree_str1,tree_str2;
    cin>>tree_str1;
    cin>>tree_str2;

    if(isValid(tree_str1) && isValid(tree_str2)){
        string normalize_tree1 = normalize(tree_str1);
        string normalize_tree2 = normalize(tree_str2);
        if(normalize_tree1==normalize_tree2)
            cout<<"isomorphic"<<endl;
        else
            cout<<"non-isomorphic"<<endl;
    }
    else {
        cout<<"error"<<endl;
    }

    return 0;
}