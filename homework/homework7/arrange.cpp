#include <iostream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

struct event{
    int b;
    int e;
};

bool cmp(event& a,event& b){
    return a.e<b.e;
}

int main(){
    vector<event> events;
    string line;
    int num;
    int dp[100] = {0};// dp数组
    int prefix_max[100] = {0};// 预存dp数组前i+1个元素最大值

    while(getline(cin,line)){
        istringstream iss(line);
        event cur;
        iss>>num;
        iss>>cur.b>>cur.e;
        events.push_back(cur);
    }

    // 按结束时间排序
    sort(events.begin(),events.end(),cmp);

    dp[0] = events[0].e-events[0].b;
    prefix_max[0] = dp[0];

    for(int i=1;i<events.size();i++){
        // 找到ei<=bj的最大的j
        int j = i-1;
        while((events[j].e > events[i].b) && j>=0) j--;
        // 都不相容的情况
        if(j<0) dp[i] = events[i].e - events[i].b;
        // dp[i] = li + max{ dp[j] | ej ≤ bi, j < i } 
        else dp[i] = events[i].e - events[i].b + prefix_max[j];
        prefix_max[i] = max(prefix_max[i-1],dp[i]);
    }

    int min_time = 24 - prefix_max[events.size()-1];

    cout<<min_time<<endl;

}