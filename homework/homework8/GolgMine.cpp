#include <iostream>
#include <vector>

using namespace std;

const int MAX_WORKER = 10000;//工人数

vector<vector<int>> memo;//备忘录 memo[i][w]代表前i个金矿使用w个工人时的最大值，-1表示未计算

int calc(const vector<int>& gold,const vector<int>& peopleNeeded,int i,int w){
    // 基本情况
    if(i==0 || w==0) return 0;

    // 已经计算过
    if(memo[i][w] != -1) return memo[i][w];

    // 迭代逻辑
    // 超过可用人数
    if(peopleNeeded[i-1] > w){
        memo[i][w] = calc(gold,peopleNeeded,i-1,w);
    }
    // 未超过可用人数
    else{
        // 如果不挖当前金矿
        int notmine = calc(gold,peopleNeeded,i-1,w);
        // 如果挖当前金矿
        int mine = calc(gold,peopleNeeded,i-1,w-peopleNeeded[i-1])+gold[i-1];
        memo[i][w] = max(notmine,mine);
    }

    return memo[i][w];
}

int maxGold(const vector<int>& gold,const vector<int>& peopleNeeded,int worker){
    int n = gold.size();
    // 初始化备忘录
    memo = vector<vector<int>>(n+1,vector<int>(worker+1,-1));
    return calc(gold,peopleNeeded,n,worker);
}

int main(){
    int worker,n;
    cin >> worker >> n;

    vector<int> gold(n);
    vector<int> peopleNeeded(n);

    for(int i=0;i<n;i++){
        cin >> gold[i];
    }

    for(int i=0;i<n;i++){
        cin >> peopleNeeded[i];
    }

    cout << maxGold(gold,peopleNeeded,worker) << endl;

    return 0;
}