/*
问题描述：
设计一个程序，用于判断一个无向图中存在多少（最大）连通子图。程序需要解析用户输入的结点及边的连接关系，根据这些信息计算并输出图中的（最大）连通子图数量。

连通子图的定义：一个连通子图中，任意两个结点间都存在直接或者间接路径。如果某个结点x不存在直接或者间接的边与另一个连通子图Y相连，则该结点不属于连通子图Y。对于一个连通子图X来说，如果不存在另一个连通子图Y，使得Y包含X，则X为一个最大连通子图。

输入说明：

每个输入用空格分割开，表示一个结点或者一条边，可以是以下两种格式：
一个数字（例如：5）：表示加入一个独立结点。
一个边（例如：u->v）：表示两个结点 u 和 v 之间存在一条无向边。
两种格式的输入顺序没有限制，可以混合出现。
所有输入结点均为整数，且结点编号为正整数。
部分结点可以在输入时被省略，但用户可以在输入边时自行增加该结点。未出现的数字不作为结点出现。例如5 3->5 6->9 4->9 9->5可以构造出5个结点、4条边的一个连通图，其中，3、4、6、9四个结点均被用户输入时省略。
为简化处理，将输入限定为一个可以被解释的图：输入仅包含空格、数字、->；箭头左右仅为数字。
输出要求：

输出图中最大连通子图数量。
*/

#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <queue>

using namespace std;

int main(){
    // 整体获取输入
    string line;
    getline(cin, line);
    stringstream ss(line);
    string token;// 每个输入

    // 图的结构
    unordered_map<int, vector<int>> graph;
    unordered_set<int> nodes;

    // 处理输入
    while(ss >> token){
        // 处理两种格式
        if(token.find("->") != string::npos){
            int pos = token.find("->");
            int u = stoi(token.substr(0, pos));
            int v = stoi(token.substr(pos + 2));
            // 插入节点
            nodes.insert(u);
            nodes.insert(v);
            // 插入边
            graph[u].push_back(v);
            graph[v].push_back(u);
        }
        else{
            // 单个节点的情况
            int u = stoi(token);
            // 插入节点
            nodes.insert(u);
            // 插入边（保证该节点在grapf中有条目）
            if(graph.find(u) == graph.end()){
                graph[u] = vector<int>();// 指向空的边列表
            }
        }
    }

    // 计算最大连通子图数量，通过BFS算法即可
    unordered_set<int> visited;
    int components = 0;

    for(int u:nodes){
        if(visited.find(u) == visited.end()){
            // 未访问过，计数加一，开始BFS
            components++;
            queue<int> q;
            q.push(u);
            visited.insert(u);// 标记已访问
            while(!q.empty()){
                int cur = q.front();
                q.pop();
                for(int neighbor:graph[cur]){
                    if(visited.find(neighbor) == visited.end()){
                        visited.insert(neighbor);
                        q.push(neighbor);
                    }
                }
            }
        }
    }

    cout << components << endl;

    return 0;
}