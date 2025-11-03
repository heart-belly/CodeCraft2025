#pragma GCC optimize("Ofast,unroll-loops")
#pragma GCC option("arch=native", "tune=native")
#pragma GCC optimize("Ofast,unroll-loops")
#pragma GCC option("arch=native", "tune=native")
#pragma GCC optimize "Ofast,omit-frame-pointer,inline,fast-math,unroll-all-loops,tree-loop-vectorize,tree-slp-vectorize"
#pragma GCC option("arch=native","tune=native","no-zero-upper")
#pragma GCC target("avx,avx2,f16c,fma,sse3,ssse3,sse4.1,sse4.2,bmi,bmi2,lzcnt,popcnt")
#pragma GCC optimize("Ofast") 
#pragma GCC target("avx,avx2,fma")
#pragma GCC optimize(1)
#pragma GCC optimize(2)
#pragma GCC optimize(3)
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <numeric> // 包含 std::iota
#include <chrono>
#include <random>
#include <set>

using namespace std;

#define MAX_DISK_NUM (10 + 1)               //最大硬盘数量：3<=N<=10
#define MAX_DISK_SIZE (16384 + 1)           //硬盘最大存储单元个数： 1<=V<=16384
#define MAX_REQUEST_NUM (30000000 + 1)      //输入数据最大总读取次数：读取id
#define MAX_OBJECT_NUM (100000 + 1)         //输入数据总写入、删除次数：写入、删除id
#define REP_NUM (3)                         //副本数量
#define FRE_PER_SLICING (1800)              //预处理时间：1800个时间片
#define EXTRA_TIME (105)                    //105个时间片
#define MAX_TAG_NUM (16)
#define MAX_TIME_SLICING (86400 + EXTRA_TIME + 1)
#define x first
#define y second
#define LOCAL
typedef pair<int, int> PII;
typedef pair<int, double> PID;
typedef pair<double, int> PDI;
//本次数据有𝑇 + 105个时间片,时间片编号为1 ~𝑇 + 105。
//M：代表对象标签数。对象标签编号为1 ~ 𝑀。]
//N：代表存储系统中硬盘的个数，硬盘编号为1 ~𝑁。
//V：代表存储系统中每个硬盘的存储单元个数。存储单元编号为1 ~ 𝑉。
//G：代表每个磁头每个时间片最多消耗的令牌数。输入数据保证64 ≤ 𝐺 ≤ 1000。
int T, M, N, V, G, K; // 磁盘的各种信息
int maxObjectId{};
int data08[1000 + 10];
int nDeletes[MAX_TIME_SLICING];
int nWrites[MAX_TIME_SLICING];
int nReads[MAX_TIME_SLICING];
PII objToTag[ MAX_OBJECT_NUM];
vector<int> requestInObj[MAX_OBJECT_NUM];
vector<int> deleteDatas[MAX_TIME_SLICING];
vector<tuple<int, int, int>> writeDatas[MAX_TIME_SLICING];
vector<tuple<int, int>> readDatas[MAX_TIME_SLICING];


int calculateWalk(int cur, int next);
void dataPrepare(vector<int> wr[],vector<int> del[],vector<int> re[]);
double subScore(int inTime, int outTime, int requestId, int size);
double getScore(int inTime, int outTime, bool hasSize, int requestId, int size);
/****辅助函数*/
std::vector<PII> mergeIntervals(std::vector<PII> intervals);
std::vector<PII> subtractIntervalSets(const std::vector<PII>& include, const std::vector<PII>& exclude);


inline int calculateWalk(int cur, int next) {

    if (cur <= next) {
        return next - cur;
    }
    return V - (cur - next);
} 


void dataPrepare(vector<int> wr[],vector<int> del[],vector<int> re[])
{
    int n = wr[1].size();

    vector<vector<int>> data;
    for(int i = 1;i <= M;i ++)
    {
        vector<int> aaa;
        int t = wr[i][0];
        aaa.push_back(t);
        for(int j = 1;j < n;j ++)
        {
            t = t + wr[i][j] - del[i][j - 1];
            // cerr<<i<< ' '<<j<<' '<<t<<endl;
            aaa.push_back(t);
        }
        data.push_back(aaa);
    }


    for(int i = 0;i < M;i ++)
    {
        for(int j = 0;j < n;j ++)
        {
            printf("%d ",data[i][j]);
        }
        printf("\n");
    }
    fflush(stdout);
    
}



// 合并重叠或相邻的区间
std::vector<PII> mergeIntervals(std::vector<PII> intervals) {
    if (intervals.empty()) return {};
    
    std::sort(intervals.begin(), intervals.end());
    
    std::vector<PII> merged;
    merged.push_back(intervals[0]);
    
    for (size_t i = 1; i < intervals.size(); ++i) {
        if (intervals[i].first <= merged.back().second) {
            merged.back().second = std::max(merged.back().second, intervals[i].second);
        } else {
            merged.push_back(intervals[i]);
        }
    }
    
    return merged;
}
// 从第一个区间集合中减去第二个区间集合
std::vector<PII> subtractIntervalSets(const std::vector<PII>& include, const std::vector<PII>& exclude) {
    
    std::vector<PII> result;
    std::vector<PII> mergedInclude = mergeIntervals(include);
    std::vector<PII> mergedExclude = mergeIntervals(exclude);
    
    for (const auto& inc : mergedInclude) {
        std::vector<PII> temp;
        temp.emplace_back(inc.first, inc.second);
        
        for (const auto& exc : mergedExclude) {
            std::vector<PII> newTemp;
            for (const auto& t : temp) {
                // 无重叠
                if (exc.second <= t.first || exc.first >= t.second) {
                    newTemp.push_back(t);
                } 
                // 完全包含
                else if (exc.first <= t.first && exc.second >= t.second) {
                    // 不加入任何东西，相当于删除
                }
                // 部分重叠
                else {
                    if (t.first < exc.first) {
                        newTemp.emplace_back(t.first, exc.first);
                    }
                    if (exc.second < t.second) {
                        newTemp.emplace_back(exc.second, t.second);
                    }
                }
            }
            temp = newTemp;
            if (temp.empty()) break;
        }
        
        result.insert(result.end(), temp.begin(), temp.end());
    }
    
    return mergeIntervals(result);
}

/**得分函数***/
inline double getScore(int inTime, int outTime, bool hasSize, int requestId, int size) {
    int x = (outTime - inTime);
    double res = 0.0;
    if (0 <= x && x <= 10) {
        res = 1 - 0.005 * x;
    } else if (x <= 105) {
        res = 1.05 - 0.01 * x;
    } else {
        res = 0.0;
    }

    if (hasSize) {
        return res * (size + 1) * 0.5;
    }

    return res;
}
/**惩罚函数***/
inline double subScore(int inTime, int outTime, int requestId, int size) {
    int x = (outTime - inTime);
    double res = 1.0 * x / 105 * size;

    return res;
}

inline void preProcess() {
    for (int i = 1; i <= 1000; ++ i) {
        data08[i] = int(ceil(i * 0.8));
    }
}

double getScoreReally[2];
double loseScore[2];
double loseScoreinTag[MAX_TAG_NUM + 1];

typedef struct Request_ {
    int objectId; //读取对象
    int prevId; //  同一个对象的上一个请求id
    bool isDone; // 有无完成
    bool isBusy; // 是否忙碌
    bool block[6]; // 判断所有对象块是否被读，读入读请求时将其初始化成对象的存储块位置，如果这个块被读过了，置为0
    int cnt; // 该请求已经堆读取的对象块数量
    bool isDelete;
    int inTime; //该请求进入时间片
    int belongToHead;

    Request_() : objectId(-1), prevId(-1), isDone(true), cnt(0), isDelete(false), belongToHead(0){};
    Request_(int _objectId, int _prevId, bool _isDone) : objectId(_objectId), prevId(_prevId), isDone(_isDone), isBusy(false){}; 
} Request;

typedef struct Object_ {
    int useDisk[REP_NUM + 1];      //存储磁盘序号
    vector<int> useDiskBlock[REP_NUM + 1];         //在disk磁盘上存储的下标序号
    unordered_map<int, int> hash;
    int size;                   //对象大小
    int tag;                    //tag
    bool isDelete{0};
    vector<int> requestGroups;

} Object;

typedef struct Head_ {
    int curPos;              //在磁盘中的下标,初始在1
    bool isRead;   //上一次操作是否是read
    int lastUseToken;      //上一次操作消耗令牌数
    int remainToken; // 本次时间片剩余的令牌数量
    int curDealRequestId; // 当前处理请求的id
    Head_(): curPos(1), isRead(false), lastUseToken(0), remainToken(G), curDealRequestId(0){};

    void reset() {
        remainToken = G;
    }


    bool isCanExec(char op) {
        // 是否可以执行该步骤
        switch(op) {
            case 'j' : {
                if (remainToken != G) return false;
                break;
            };
            case 'p' : {
                if (remainToken == 0) return false;
                break;
            };
            case 'r' : {

                int cmp = (isRead ? max(16, data08[lastUseToken]) : 64);

                if (remainToken < cmp) return false;
                break;
            }
        }

        return true;
    }

    void updateNextStep(char op, int pos) {

        // 同步数据结构
        switch (op) {
            case 'j': {
                remainToken = 0;
                curPos = pos;
                lastUseToken = G;
                isRead = false;
                break;
            };
            case 'p' : {
                -- remainToken;
                curPos = curPos % V + 1;
                lastUseToken = 1;
                isRead = false;
                break;
            };
            case 'r' : {
                if (isRead) {
                    int need = max(16, data08[lastUseToken]);;
                    remainToken -= need;
                    lastUseToken = need;

                } else {
                    remainToken -= 64;
                    lastUseToken = 64;
                }
                curPos = curPos % V + 1;
                isRead = true;
                break;
            }
        
            default:
                break;
        }

    }
} Head;
//记录磁盘中空余容量、连续空余编号[left,right]
typedef struct Disk_ {
    int empty{V};   //磁盘剩余空余大小
    int use[MAX_DISK_SIZE];       //磁盘下标是否存储
    Head head[2]; // 两个磁头
    vector<queue<int>> taskList;    //读任务队列
    int readTask = 0;   //读任务数量
}Disk;




typedef struct Scheduler1_ {

    // 请求
    Request request[MAX_REQUEST_NUM]; 
    // 对象
    Object object[MAX_OBJECT_NUM];
    // 磁盘
    Disk disk[MAX_DISK_NUM];



    /****删除部分******************/
    void doObjectDelete(const vector<int>& useDiskBlock, int* disk_unit, int size) {
        for (int i = 1; i <= size; i++) {
            disk_unit[useDiskBlock[i]] = 0;
        }
    }

    void deleteAction(int tmp) {
        int n_delete;
        vector<int> deleteGroups;
        deleteGroups.reserve(100);
        scanf("%d", &n_delete);
        nDeletes[tmp] = n_delete;


        for (int i = 1; i <= n_delete; ++ i) {
            int id;
            scanf("%d", &id);
            deleteDatas[tmp].push_back(id);
            //同步一下disk.empty
            auto d = object[id].useDisk;
            int size = object[id].size;
            object[id].isDelete = true;

            //把对应disk上对应块上的队列全删除
            {
                int di = d[1];
                for(int j = 1;j <= size;j ++)
                {
                    int b = object[id].useDiskBlock[1][j];
                    disk[di].taskList[b] = {};
                }
            }

            for(int j = 1;j < REP_NUM + 1;j ++)
            {
                if(d[j] != 0)
                {
                    for(int k = 1;k <= size;k ++)
                    {
                        disk[d[j]].use[object[id].useDiskBlock[j][k]] = 0;
                    }

                    disk[d[j]].empty += size;
                }
                
            }

            // 遍历该对象的请求列表
            for (const auto& requestId : object[id].requestGroups) {
                // 这里可以二分查找（因为按照请求到来的时间顺序插入，所以后面来的如果满足可以读取请求，则前面一定满足）
                if (request[requestId].isDone == false && request[requestId].isBusy == false) {
                    request[requestId].isDelete = true;
                    deleteGroups.emplace_back(requestId);
                    disk[d[1]].readTask --;
                    
                }
            }
            // 操作与对象存储相关的数据结构
            for (int j = 1; j <= REP_NUM; ++ j ) {
                doObjectDelete(object[id].useDiskBlock[j], disk[object[id].useDisk[j]].use, object[id].size);
            } 
        }

        printf("%d\n", deleteGroups.size());
        for (auto& rid : deleteGroups) {
            printf("%d\n", rid);
        }

        fflush(stdout);
    }

    /**********写入部分*************/
    //先随便存
    void spacePrepare()
    {
        int storeSum = 6000;    //随便写一个
        for(int i = 1;i <= N;i ++)
        {
            for(int j = 1;j <= storeSum;j ++)
            {
                disk[i].use[j] = -1;    //-1表示被预定
            }
            disk[i].empty = V - storeSum;   //磁盘剩余空间

            int some_size = 0;  // 每行的列数
            // cerr<<"storeSum:"<<storeSum<<endl;
            disk[i].taskList.resize(storeSum + 1);

            // printf("disk:%d size:%d\n",i,disk[i].taskList.size());
        }
    }

    inline void findBestStore(int st,int target,int l,int r,int sted)
    {
        int tag = object[target].tag;
        int size = object[target].size;
        
        // int l = tagfb[tag][st - 1].first;
        // int r = tagfb[tag][st - 1].second;
        
        int space[6];
        int d;
        int idxAll[11];
        int spaceAll[11][6];
        bool isSeries[11];
        memset(isSeries, 0, sizeof isSeries);
        memset(idxAll,1,sizeof(idxAll));
        memset(spaceAll,0,sizeof(spaceAll));
        // bool sted[17];
        // if(st == 1)
        // {
        //     for(int i = 1;i <= M;i ++) sted[i] = sted1[i];
        // }
        // else if(st == 2)
        // {
        //     for(int i = 1;i <= M;i ++) sted[i] = sted2[i];
        // }
        //sted[tag] = true;
        // printf("target:%d tag:%d sted:%d l:%d r:%d\n",target,tag,sted[tag],l,r);

        for(int i = 1;i <= N;i ++)
        {
            idxAll[i] = 1;

            //避开前几个副本的位置
            if(i == object[target].useDisk[1]) continue;
            if(i == object[target].useDisk[2]) continue;

            if(sted == true) //从前往后存
            {
                // 先找连续存的位置
                for(int j = l;j <= r;j ++)
                {
                    int cnt = 0;
                    for (int j1 = j; j1 <= min(r,  j + size - 1); ++ j1) {
                        if(disk[i].use[j1] == -1 || disk[i].use[j1] == 0) cnt ++;
                    }
                    if (cnt == size) {
                        for (int j1 = j; j1 <= min(r,  j +size - 1); ++ j1) {
                                spaceAll[i][idxAll[i]] = j1;
                            if(idxAll[i] == size) break;
                            if(spaceAll[i][idxAll[i]] != 0) idxAll[i] ++;
                        }
                        if (idxAll[i] == size) {
                            isSeries[i] = true;
                            break;
                        }
                    }
                }
                if(isSeries[i] == false)    //不能连续存
                {
                    for(int j = l;j <= r;j ++)
                    {
                        if(disk[i].use[j] == -1 || disk[i].use[j] == 0)
                        {
                            spaceAll[i][idxAll[i]] = j;
                            if(idxAll[i] == size) break;
                            if(spaceAll[i][idxAll[i]] != 0) idxAll[i] ++;
                        }
                    }
                }
            }
            else    //从后往前存
            {
                // 先找连续存的位置
                for(int j = r;j >= l; -- j)
                {
                    int cnt = 0;
                    for (int j1 = j; j1 >= max(l,  j  - size + 1); -- j1) {
                        if(disk[i].use[j1] == -1 || disk[i].use[j1] == 0) cnt ++;
                    }
                    if (cnt == size) {
                        for (int j1 = j; j1 >= max(l,  j  - size + 1); -- j1) {
                                spaceAll[i][idxAll[i]] = j1;
                            if(idxAll[i] == size) break;

                            if(spaceAll[i][idxAll[i]] != 0) idxAll[i] ++;
                        }
                        if (idxAll[i] == size) {
                            isSeries[i] = true;
                            break;
                        }
                    }
                }
                if(isSeries[i] == false)    //不能连续存
                {
                    for(int j = r;j >= l;j --)
                    {
                        if(disk[i].use[j] == -1 || disk[i].use[j] == 0)
                        {
                            spaceAll[i][idxAll[i]] = j;
                            if(idxAll[i] == size) break;
                            if(spaceAll[i][idxAll[i]] != 0) idxAll[i] ++;
                        }
                    }
                }          
            }
        }
        //选择目标磁盘
        int dNow = -1;
        int minLast;
        
        if(sted) //从前往后
        {
            minLast = V;
            for(int i = 1;i <= N;i ++)
            {
                if(spaceAll[i][size] != 0 && spaceAll[i][size] < minLast)
                {
                    minLast = spaceAll[i][size];
                    dNow = i;
                }
            }
            if(dNow == -1)   //10个磁盘都放不下，随机选择一个磁盘可以超出l,r范围放
            {
                // 生成1到10之间的随机数作为目标磁盘
                int dd;
                do {
                    dd = std::rand() % 10 + 1;  // 生成 1 到 10 之间的随机数
                } while (dd == object[target].useDisk[1] || dd == object[target].useDisk[2]);  // 如果时副本1时，1和2的位置都是0，不影响
                dNow = dd;
                //重置space
                for(int j = 1;j <= size;j ++) spaceAll[dd][j] = 0;
                int idx = 1;
                for(int j = l;j <= disk[dd].taskList.size() - 1;j ++)
                {
                    if(disk[dd].use[j] == -1 || disk[dd].use[j] == 0)
                    {
                        spaceAll[dd][idx] = j;
                        if(idx == size) break;
                        if(spaceAll[dd][idx] != 0) idx ++;
                    } 
                }
                if(spaceAll[dd][size] == 0)
                {
                    // if(spaceAll[idxAll[i]] == 0) idxAll[i] --;
                    if(l != 1)
                    {
                        for(int k = l - 1;k >= 1;k --)
                        {
                            if(disk[dd].use[k] == -1 || disk[dd].use[k] == 0)
                            {
                                spaceAll[dd][idx] = k;
                                if(idx == size) break;
                                if(spaceAll[dd][idx] != 0) idx ++;
                            }
                        }
                    }
                    
                }
            }
        }
        else
        {
            minLast = 0;
            for(int i = 1;i <= N;i ++)
            {
                if(spaceAll[i][size] != 0 && spaceAll[i][size] > minLast)
                {
                    //printf("task: %d final:%d\n",i,spaceAll[i][size]);
                    minLast = spaceAll[i][size];
                    dNow = i;
                }
            }
            if(dNow == -1)   //10个磁盘都放不下，随机选择一个磁盘可以超出l,r范围放
            {
                // 生成1到10之间的随机数作为目标磁盘
                int dd;
                do {
                    dd = std::rand() % 10 + 1;  // 生成 1 到 10 之间的随机数
                } while (dd == object[target].useDisk[1] || dd == object[target].useDisk[2]);  // 如果时副本1时，1和2的位置都是0，不影响
                dNow = dd;
                //重置space
                for(int j = 1;j <= size;j ++) spaceAll[dd][j] = 0;
                int idx = 1;
                for(int j = r;j >= 1;j --)
                {
                    if(disk[dd].use[j] == -1 || disk[dd].use[j] == 0)
                    {
                        spaceAll[dd][idx] = j;
                        if(idx == size) break;
                        if(spaceAll[dd][idx] != 0) idx ++;
                    } 
                }
                if(spaceAll[dd][size] == 0)
                {
                    for(int j = r + 1;j <= disk[dd].taskList.size() - 1;j ++)
                    {
                        if(disk[dd].use[j] == -1 || disk[dd].use[j] == 0)
                        {
                            spaceAll[dd][idx] = j;
                            if(idx == size) break;
                            if(spaceAll[dd][idx] != 0) idx ++;
                        } 
                    }
                    
                }
            }
        }


        //printf("\n");
        d = dNow;
        //cerr<<"disk:"<<d<<endl;
        for(int i = 1;i <= size;i ++)
        {
            space[i] = spaceAll[dNow][i];
            //cerr<<space[i]<<' ';
        }

        //cerr<<endl;
        //存完的在磁盘同步一下
        //disk[d].empty -= size;
        //同步object
        object[target].useDisk[st] = d;  //1号磁盘为read盘
        object[target].hash[d] = st;
        object[target].useDiskBlock[st].resize(size + 1);
        int ss = 1;
        for(int t = 1;t <= size;t ++)
        {
            object[target].useDiskBlock[st][ss ++] = space[t];
            disk[d].use[space[t]] = target;
        }
        printf("%d ",d);
        for(int t = 1;t <= size;t ++)
        {
            printf("%d ",space[t]);
        }
        printf("\n");
    }

    void writeOpt(int start,int end)
    {
        for(int i = start;i <= end;i ++)
        {
            int size = object[i].size;
            //输出存储对象序号
            printf("%d\n",i);

            findBestStore(1,i,1,V,true);

            for(int j = 2;j <= 3;j ++)
            {
                findBestStore(j,i,1,V,false);
            }
        }
    }

    void writeAction(int tmp)
    {
        
        int n_write;
        scanf("%d", &n_write);
        nWrites[tmp] = n_write;
        if(n_write == 0) return;
        //统计本次所有输入
        int start = 0,end = 0;
        for (int i = 1; i <= n_write; ++ i) {
            int id, size, tag;
            scanf("%d%d%d", &id, &size, &tag);
            writeDatas[tmp].emplace_back(id, size, tag);
            object[id].size = size, object[id].tag = tag;
            objToTag[id] = make_pair(tag, size);
            maxObjectId = max(maxObjectId, id);

            if(i == 1) start = id;
            if(i == n_write) end = id;
        }
        
        writeOpt(start,end);
        fflush(stdout);
    }

    /************读取部分*******************/
    void readAction(int tmp, int& deadSum) {
        int n_read;
        int requestId, objectId;
        vector<int> readFinish, busyRequest; //本磁盘本次行动任务完成队列
        scanf("%d", &n_read);
        nReads[tmp] = n_read;
        // cerr<<n_read<<endl;
        for (int i = 1; i <= n_read; i++) {
            scanf("%d%d", &requestId, &objectId);
            readDatas[tmp].emplace_back(requestId, objectId);
            requestInObj[objectId].emplace_back(tmp);
            //先判读该请求是否可以直接忽略
            int size = object[objectId].size;
            int tag = object[objectId].tag;
            int tmp1800 = (tmp - 1) / 1800;

            //如果在接下来的1800个时间片上平均每个时间片读块数量小于等于1，则该tag在接下来1800时间片上的读请求全部忽略
            if(1) {
                if (!request[requestId].isBusy) {
                    busyRequest.emplace_back(requestId);
                }
                // 上报忙碌请求
                request[requestId].isBusy = true;
                continue;
            }

            object[objectId].requestGroups.emplace_back(requestId); //方便删除对象时将对应任务删除
            request[requestId].objectId = objectId;
            request[requestId].isDone = false;
            request[requestId].inTime = tmp;    //进入时间

            request[requestId].cnt = 0;     //该请求已经堆读取的对象块数量
            
            for(int j = 1;j <= size;j ++)
            {
                request[requestId].block[j] = object[objectId].useDiskBlock[1][j];  //第一副本存的块
            }
            
            int d = object[objectId].useDisk[1];    //目标读取的磁盘
            disk[d].readTask ++;    //队列任务数量加一
            for(int j = 1;j <= size;j ++)
            {
                int objPos = object[objectId].useDiskBlock[1][j];//在磁盘中出现的位置
                disk[d].taskList[objPos].push(requestId);   //给这些位置的队列加上读取id
            }
        }

        for(int i = 1;i <= N;i ++)
        {

            // 获取磁盘
            auto& dk = disk[i];
            
            for(int j = 1;j <= dk.taskList.size() - 1; ++ j)
            {
                // cerr<<i<<endl;
                // cerr<<"disk:"<<i<<' '<<disk[i].taskList[1].size()<<endl;
                while (!disk[i].taskList[j].empty()) {
                    // cerr<<"disk:"<<i<<' '<<j<<endl;
                    int re = disk[i].taskList[j].front();
                    int time = tmp - request[re].inTime;
                    if ( time >= 105)
                    {
                        deadSum ++;
                        if (!request[re].isBusy) {
                            busyRequest.emplace_back(re);
                        }
                        // 上报忙碌请求
                        request[re].isBusy = true;           
                        disk[i].taskList[j].pop();

                    } else {
                        break;
                    }                 
                }

            }
            // cerr<<tmp<<endl;
            for (int i1 = 0; i1 < 2; ++ i1 ) {
                bool finish = false;    //是否结束本时间片的操作
                //将令牌用完，或者队列中没任务为止
                bool isJump = false;
                auto& head = disk[i].head[i1];
                int& headPos = head.curPos;
                head.reset();
                int sz = dk.taskList.size();
                // int start = (i1 == 0 ? 1 : (int)(sz / 2) + 1);
                // int end = (i1 == 0 ? (int)(sz / 2) : sz - 1);
                int start = (i1 == 0 ? 1 : 3000);
                int end = (i1 == 0 ? 3001 : dk.taskList.size() - 1);
                // cerr << "**********************： " << start << " " << end << endl;
                //删除所有超时的请求和已经读过的请求
                // 删除所有读取的块
                /*************************************************/
                // cerr<<start<<' '<<end<<endl;
                while(head.remainToken > 0)    //剩余令牌数大于0且任务队列中还有任务，继续行动
                {
                    // head = disk[i].head.curPos; 
                    int tokenRemain =head.remainToken; //本轮剩余令牌数
                    char op = '0';        //操作类型
                    int pos = 0;    //目标位置


                    int ne = 0; 
                    // 处理空读
                    int sz = disk[i].taskList.size();
                    if (dk.taskList[headPos].empty()) {
                        // [64, 52, 41, 33, 26,]
                        // 判断后面隔两个空有没有需要读的
                        int situ[7] = {65, 45, 35, 22, 18, 16, 16};
                        // 大于等于8基本都是赚的
                        int maxDistance = 11;                                                                                                                                                  ;
                        for (int j = headPos + 1; j <= min(end, headPos + maxDistance ); ++ j) {
                            int pos = j;
                            int select = 0;
                            if (j - headPos + 1 >= 6 && (tmp > 10000 && tmp < 20000)) {
                                select = 0;
                            } else if (j - headPos + 1 >= 6 && (tmp > 20000 && tmp < 80000)) {
                                if (j - headPos >= 7) {
                                    select = 1; 
                                } else {
                                    select = 1;
                                }
                            } 

                            if (head.isRead && head.lastUseToken <= situ[select] && !disk[i].taskList[pos].empty()) {
                                int sub = j - headPos;
                                if (sub >= 9 && (tmp > 18000 && tmp < 78000)) {
                                    int cnt = 0;
                                    for (int j1 = j + 1; j1 < min(end, j + 10); ++ j1) {
                                        if (disk[i].taskList[j1].size()) cnt += disk[i].taskList[j1].size();
                                    }
                                    if (cnt >= 5 ) {
                                        // 可以连续读
                                        ne = headPos;
                                        break;
                                    }
                                } else if (sub < 9){
                                    ne = headPos;
                                    break;
                                }

                            }
                        }

                    }
                    if (ne == 0) {
                        // 说明前面连读无效
                        static int cnt[10000];

                        for(int j = headPos;j <= end;j ++)
                        {
                            if(!disk[i].taskList[j].empty())
                            {   
                                int cnt = 0;
                                for (int j1 = j + 1; j1 <= min(end, j + 10); ++ j1) {
                                    if (disk[i].taskList[j1].size()) cnt += disk[i].taskList[j1].size();
                                    // else {
                                    //     break;
                                    // }
                                }
                                // // 寻找数量大于等于2的位置(这里还是太粗糙了，可以自适应，比如根据磁盘上连续最大请求的数量，来决定跳过的阈值)
                                bool flag1_0 = (tmp >=  18000 && tmp <= 72000 && disk[i].taskList[j].size() == 1 && cnt < 9 && j != headPos);
                                bool flag1_1 = (tmp >=  18000 && tmp <= 72000  && disk[i].taskList[j].size() == 2 && cnt < 7 && j != headPos);
                                if ( flag1_0 || flag1_1 ) {}
                                else {
                                    ne = j;
                                    break;
                                }

                            }
                        }
                    }

                    //去下一步需要走的距离
                    int step = abs(ne - headPos);
                    fflush(stdout);
                    // printf("remainToken:%d\n",head.remainToken);
                    // printf("step:%d ne:%d\n",step,ne);
                    if(step > 0)    //判断怎么走
                    {
                        if(ne == 0)
                        {                 
                            if(head.isCanExec('j'))//后面没任务了，就回到起点
                            {
                                op = 'j';
                                //pos = 1;
                                int ne = 0; 
                                int posOne = 1;
                                for(int j = start;j <= end ;j ++)
                                {
                                    if(!disk[i].taskList[j].empty())
                                    {
                                        int cnt = 0;
                                        for (int j1 = j + 1; j1 <= min(end, j + 10); ++ j1) {
                                            cnt += disk[i].taskList[j1].size();
                    
                                        }
                                        bool flag = (tmp >= 18000 && tmp <= 78000 && disk[i].taskList[j].size() == 1 && cnt < 3 && j != headPos);
                                        bool flag1 = (tmp >= 18000 && tmp <= 78000 && disk[i].taskList[j].size() == 2 && cnt < 2 && j != headPos);
                                        if (flag || flag1) {}
                                        else {
                                            ne = j;
                                            break;
                                        }
                                    }


                                    if (disk[i].taskList[j].size() > 0 && posOne != 1) {
                                        posOne = j;
                                    }
                                }

                                if(ne == 0) pos = posOne;
                                else pos = ne;
                            }
                            else
                            {
                                finish = true;
                            }
                        }
                        else    //后面有任务，可以走
                        {
 
    
                            

                            if(step <= tokenRemain)     //能走到
                            {
                                op = 'p';
                                pos = step; //p的数量
                            }
                            else if(step < G)//在本轮中走不到目标位置但是下一轮可以走到,那就先走到最远位置
                            {
                                op = 'p';
                                pos = tokenRemain;
                            }
                            else    //本轮走不到
                            {

                                if(head.isCanExec('j'))//直接跳到ne
                                {
                                    op = 'j';
                                    pos = ne;
                                }
                                else    //令牌不够，等一回合
                                {
                                    finish = true;
                                }
                            }
                        }

                        //打印
                        if(op == 'j') 
                        {
                      
    
                        
                            isJump = true;
                            printf("%c %d\n",op,pos);
                            head.updateNextStep(op,pos);    //更新输出与数据
                        }
                        else if(op == 'p')
                        {
                            while(pos --) 
                            {
                                printf("%c",op);
                                head.updateNextStep(op,pos);    //更新输出与数据
                            }
                        }
                    }
                    else    //读（读完这个连续块或者时间用完）
                    {

                        // 可能会空读
                        if (disk[i].taskList[ne].empty()) {
                            if(!head.isCanExec('r'))
                            {
                                finish = true;
                            
                            } else {
                                op = 'r';
                                pos = 1;
                                printf("%c",op);
                                head.updateNextStep(op,pos);    //更新输出与数据       
                            }

                    

                        } else {
                            int idx = ne;   //从目标位置开始
                            //如果这个位置有任务并且位置没有超出上限
                            while(idx <= end && !disk[i].taskList[idx].empty())
                            {
                                // printf("idx:%d\n",idx);
                                //idx块有读取，但是令牌不够进行读取，则跳出循环
                                if(!head.isCanExec('r'))
                                {
                                    finish = true;
                                    break;
                                }
                                op = 'r';
                                pos = 1;
                                printf("%c",op);
                                int read = disk[i].taskList[idx].front();
                                int obj = request[read].objectId;
                                head.updateNextStep(op,pos);    //更新输出与数据
                                // printf("readid:%d\n",read);
                                //处理这个块上的所有任务
                                while(!disk[i].taskList[idx].empty())
                                {
                                    // printf("readsize:%d\n",disk[i].taskList[idx].size());
                                    int read = disk[i].taskList[idx].front();
                                    //printf("read %d\n",read);
                                    //同步任务是否被读完
                                    int obj = request[read].objectId;
                    
                                    for(int l = 1;l <= object[obj].size; ++ l)
                                    {
                                        //该block->idx是属于obj的存储位置，并且request的这个位置没有被读过
                                        if(idx == object[obj].useDiskBlock[1][l] && request[read].block[l] != 0)
                                        {
                                            request[read].cnt ++;
                                            request[read].block[l] = 0;
                                            if(request[read].cnt == object[obj].size)   //该任务所有块都被读完了
                                            {
                                                readFinish.emplace_back(read); //加入任务完成列表
                                                request[read].isDone = true;
                                                disk[i].readTask --;
                                            }
                                        }
                                    }
                                    disk[i].taskList[idx].pop();
                                }


                                idx ++;
                            }

                        
                        }

                    }

                    if(finish == true) break;
                }
                
                if(!isJump) printf("#\n");
            }

        }




        printf("%d\n",readFinish.size());
        for(int i = 0;i < readFinish.size();i ++)
        {
            int readId = readFinish[i];
            int tt = tmp - request[readId].inTime;
            if(tt > 105) deadSum ++;
            printf("%d\n",readFinish[i]);

            // score[object[ request[readId].objectId].tag] += calculateScore(request[readId].inTime,tmp, true, readId);
        }
        printf("%d\n",busyRequest.size());
        for(int i = 0;i < busyRequest.size();i ++)
        {
            int readId = busyRequest[i];
            printf("%d\n", busyRequest[i]);

            // score[object[ request[readId].objectId].tag] += calculateScore(request[readId].inTime,tmp, true, readId);
        }
        fflush(stdout);

    }
    /************磁盘回收部分******************/
    inline void garbageCollectionAction(int fbId) 
    {
        scanf("%*s%*s");
        printf("GARBAGE COLLECTION\n");
        bool sted[MAX_TAG_NUM + 1];

        for (int i = 1; i <= N; ++ i) {
            int sz = disk[i].taskList.size();
            auto& dk = disk[i];
            // 遍历磁盘中的所有空缺位置
            printf("0\n");
        }
        fflush(stdout);
    }




}SchedulerFirst;


typedef struct Scheduler2_ {



    pair<int,int> storeStart[17];//每个标签在每个磁盘中所占位置的起点终点
    int storeSum;   //每个磁盘最后被占位置的位置
    vector<pair<int,int>> needGroup;    //需要被聚合的tag
    bool sted[17];  //该tag是从前往后还是从后往前，从前往后是true
    /************************************************/
    int firstStore[17]; //第一个1800时间片每个任务首先存的块数量
    int firstIn[17];    //第一个1800时间片每个任务进来的块数量
    // 调试
    int c1{}, c2{};
    int side[17] = {0, 12, 14, 5, 7, 15, 13, 9, 16, 4, 8, 3, 10, 2,6, 1, 11}; //2173
    /*作聚类*/
    unordered_set<int> clusterLow, clusterHigh, clusterLow1, clusterHigh1;
        // 记录每个周期请求随时间片的累加
    int requestCnt[MAX_TAG_NUM + 1];
    bool isBusy[MAX_TAG_NUM + 1];


    vector<double> md[17], mdReal[17];
    // 请求
    Request request[MAX_REQUEST_NUM]; 
    // 对象
    Object object[MAX_OBJECT_NUM];
    // 磁盘
    Disk disk[MAX_DISK_NUM];
    // 自适应每个磁盘中磁头的位置
    PII headBoard[MAX_DISK_NUM][2];
    PII headLastBoard[MAX_DISK_NUM][2];




    void findClosestTags(vector<int> re[]) {
        int n = re[1].size();
        //计算每个tag的读面积
        int readArea[17];   
        for(int i = 1;i <= M;i ++)
        {
            int t = 0;
            for(int j = 0;j < n;j ++)
            {
                readArea[i] += re[i][j];
            }
        }
        //计算两个tag的重合度
        vector<vector<float>> closeTags(17,vector<float>(17, 0));
        for(int i = 1;i <= M;i ++)
        {
            for(int j = 1;j <= M;j ++)
            {
                if(i == j) continue;
                int area = 0;
                for(int k = 0;k < n;k ++)
                {
                    // cerr<<"min:"<<min(re[i][k],re[j][k])<<endl;
                    area += min(re[i][k],re[j][k]);
                }
                // cerr<<"area:"<<area<<endl;
                // cerr<<"areaall:"<<readArea[i] + readArea[j] - area<<endl;
                float tags = static_cast<float>(area) / (readArea[i] + readArea[j] - area);
                // cerr<<tags<<endl;
                closeTags[i][j] = tags;
            }
        }
        int tag1,tag2;
        float maxtags = 0;
        for(int i = 1;i <= M;i ++)
        {
            for(int j = i;j <= M;j ++)
            {
                //cerr<<closeTags[i][j]<<' ';
                if(closeTags[i][j] > maxtags)
                {
                    maxtags = closeTags[i][j];
                    tag1 = i;
                    tag2 = j;
                }
            }

        }

    }

    void spacePrepare(vector<int> wr[],vector<int> del[],vector<int> re[])
    {
        int n = wr[1].size();  
        //计算每个tag的峰值
        int maxh;   //峰值
        pair<int,int> max_hx[17];   //峰值以及下标
        vector<int> leftx;      //峰值在左边
        vector<int> rightx;
        for(int i = 1;i <= M;i ++)
        {
            maxh = wr[i][0]; 
            int sum =  max(1, wr[i][0]);
            md[i].push_back((1.0 * re[i][0]) / sum);

            for(int j = 1;j < n;j ++)
            {
                sum += wr[i][j] - del[i][j - 1];
                float t = (1.0 * re[i][j]) / sum;
                // cerr<<t<<endl;
                md[i].push_back(t);
                if(sum > maxh)
                {
                    maxh = sum;
                    max_hx[i] = {sum,j};
                }
            }
            cerr<<"tag"<<i<<' '<<"hh:"<<max_hx[i].first<<' '<<"xx:"<<max_hx[i].second<<endl;
            if(max_hx[i].second <= n / 2) leftx.push_back(i);
            else rightx.push_back(i);
        }

        //计算所有tag读取的重合度
        //计算每个tag的读面积
        int readArea[17];   
        for(int i = 1;i <= M;i ++)
        {
            int t = 0;
            for(int j = 0;j < n;j ++)
            {
                readArea[i] += re[i][j];
            }
        }
        //计算两个tag的重合度
        vector<vector<float>> closeTags(17,vector<float>(17, 0));
        priority_queue<pair<float,pair<int,int>>, vector<pair<float,pair<int,int>>>, less<pair<float,pair<int,int>>>> heap;
        for(int i = 1;i <= M;i ++)
        {
            for(int j = 1;j <= M;j ++)
            {
                if(i == j) continue;
                int areamin = 0;
                int areamax = 0;
                for(int k = 0;k < n;k ++)
                {
                    // cerr<<"min:"<<min(re[i][k],re[j][k])<<endl;
                    areamin += min(re[i][k],re[j][k]);
                    areamax += max(re[i][k],re[j][k]);
                }

                float tags = 1.0 * areamin / areamax;

                closeTags[i][j] = tags;
                heap.push({tags,{min(i,j),max(i,j)}});
            }
        }

        

        // needGroup.push_back({side[1],side[2]});
        // needGroup.push_back({side[3],side[4]});
        // needGroup.push_back({side[5],side[6]});
        // needGroup.push_back({side[7],side[8]});
        // needGroup.push_back({side[9],side[10]});
        // needGroup.push_back({side[11],side[12]}); 
        // needGroup.push_back({side[13],side[14]});
        // needGroup.push_back({side[15],side[16]});
        // sted[side[1]] = true;
        // sted[side[3]] = true;
        // sted[side[5]] = true;
        // sted[side[7]] = true;
        // sted[side[9]] = true;
        // sted[side[11]] = true;
        // sted[side[13]] = true;
        // sted[side[15]] = true;
        //计算每个标签总的存储空间
        int wrAll[17];
        storeSum = 0;   //每个磁盘给第一副本预留的空间
        memset(wrAll,0,sizeof(wrAll));

        bool isDealed[17];  //是否被预分配过了
        memset(isDealed,false,sizeof(isDealed));
        // cerr<<"1111111111111111111111111111"<<endl;
        int l,r;
        for(int s = 1;s <= M;s ++)
        {
            int i = side[s];
            //printf("side:%d\n",i);
            int limitDisk[17];
            for(int j = 1;j <= M;j ++) limitDisk[j] = wr[i][0];
            int maxsum = 0;
            for(int j = 1;j < n;j ++)
            {
                limitDisk[i] = limitDisk[i] + wr[i][j] - del[i][j - 1];
                maxsum = max(maxsum,limitDisk[i]);
            }
            //printf("maxsum:%d\n",maxsum);
            maxsum = maxsum / N + 1;
            storeSum += maxsum;

            
            int l = 1,r;
            for(int j = 1;j <= M;j ++)
            {
                if(storeStart[j].second > l)
                {
                    l = storeStart[j].second + 1;
                }
            }

            r = l + maxsum - 1;
            storeStart[i] = {l,r};
            cerr<<"tag:"<<i<<' '<<"l,r:"<<l<<' '<<r<<endl;
        }

    

        // 对每个盘的磁头进行分配
        set<PII> series;
        for (int i = 1; i <= M; ++ i) {
            series.insert(storeStart[i]);
        }


        int lastPos = -1;
        for (auto [l, r] : series) {
            cerr << l<< " " << r << endl;
            lastPos = max(lastPos, r);
        }
        cerr << "**************" << lastPos << endl;
        // 进行平分
        int acc = 0;
        int start = 1, end = 0;
        int overL = 0, overR = 0;
        for (auto [l, r] : series)  {
            end = r;
            acc += (r - l);
            cerr << acc << " " << end << endl;
            if (acc >= lastPos / 2) {
                overL = l;
                overR = r;
                break;
            }
        }
        for (int i = 1; i <= N; ++ i) {




            headBoard[i][0] = make_pair(start, storeStart[side[8]].y);
            headBoard[i][1] = make_pair(storeStart[side[8]].y + 1 , lastPos);

            // headBoard[i][0] = make_pair(start, storeStart[16].x + (storeStart[16].y - storeStart[16].x) / 2);
            // headBoard[i][1] = make_pair(storeStart[16].x + (storeStart[16].y - storeStart[16].x) / 2 + 1, lastPos);

            cerr << headBoard[i][0].x << " " << headBoard[i][0].y << endl;
        }


        fflush(stdout);
        for(int i = 1;i <= N;i ++)
        {
            for(int j = 1;j <= storeSum;j ++)
            {
                disk[i].use[j] = -1;    //-1表示被预定
            }
            disk[i].empty = V - storeSum;   //磁盘剩余空间

            int some_size = 0;  // 每行的列数
            // cerr<<"storeSum:"<<storeSum<<endl;
            disk[i].taskList.resize(storeSum + 1);
            // printf("disk:%d size:%d\n",i,disk[i].taskList.size());
        }
    }


    /****删除部分******************/
    void doObjectDelete(const vector<int>& useDiskBlock, int* disk_unit, int size) {
        for (int i = 1; i <= size; i++) {
            disk_unit[useDiskBlock[i]] = 0;
        }
    }

    void deleteAction(int tmp) {
        int n_delete;
        vector<int> deleteGroups;
        deleteGroups.reserve(100);
        n_delete = nDeletes[tmp];
        for (int i = 1; i <= n_delete; ++ i) {
            int id;
            id = deleteDatas[tmp].at(i - 1);
            //同步一下disk.empty
            auto d = object[id].useDisk;
            int size = object[id].size;
            object[id].isDelete = true;

            //把对应disk上对应块上的队列全删除
            {
                int di = d[1];
                for(int j = 1;j <= size;j ++)
                {
                    int b = object[id].useDiskBlock[1][j];
                    disk[di].taskList[b] = {};
                }
            }

            for(int j = 1;j < REP_NUM + 1;j ++)
            {
                if(d[j] != 0)
                {
                    for(int k = 1;k <= size;k ++)
                    {
                        disk[d[j]].use[object[id].useDiskBlock[j][k]] = 0;
                    }

                    disk[d[j]].empty += size;
                }
                
            }

            // 遍历该对象的请求列表
            for (const auto& requestId : object[id].requestGroups) {
                // 这里可以二分查找（因为按照请求到来的时间顺序插入，所以后面来的如果满足可以读取请求，则前面一定满足）
                if (request[requestId].isDone == false && request[requestId].isBusy == false) {
                    request[requestId].isDelete = true;
                    deleteGroups.emplace_back(requestId);
                    disk[d[1]].readTask --;
                    
                }
            }
            // 操作与对象存储相关的数据结构
            for (int j = 1; j <= REP_NUM; ++ j ) {
                doObjectDelete(object[id].useDiskBlock[j], disk[object[id].useDisk[j]].use, object[id].size);
            } 
        }

        printf("%d\n", deleteGroups.size());
        for (auto& rid : deleteGroups) {
            printf("%d\n", rid);
        }

        fflush(stdout);
    }

    /********写入部分******************/
    inline void findBestStore(int st,int target)
    {
        int tag = object[target].tag;
        int size = object[target].size;
        
        int l = storeStart[tag].first;
        int r = storeStart[tag].second;
        
        int space[6];
        int d;
        int idxAll[11];
        int spaceAll[11][6];
        bool isSeries[11];
        memset(isSeries, 0, sizeof isSeries);
        memset(idxAll,1,sizeof(idxAll));
        memset(spaceAll,0,sizeof(spaceAll));
        //sted[tag] = true;
        // printf("target:%d tag:%d sted:%d l:%d r:%d\n",target,tag,sted[tag],l,r);

        for(int i = 1;i <= N;i ++)
        {
            idxAll[i] = 1;

            //避开前几个副本的位置
            if(i == object[target].useDisk[1]) continue;
            if(i == object[target].useDisk[2]) continue;

            if(sted[tag] == true) //从前往后存
            {
                // 先找连续存的位置
                for(int j = l;j <= r;j ++)
                {
                    int cnt = 0;
                    for (int j1 = j; j1 <= min(r,  j + size - 1); ++ j1) {
                        if(disk[i].use[j1] == -1 || disk[i].use[j1] == 0) cnt ++;
                    }
                    if (cnt == size) {
                        for (int j1 = j; j1 <= min(r,  j +size - 1); ++ j1) {
                                spaceAll[i][idxAll[i]] = j1;
                            if(idxAll[i] == size) break;
                            if(spaceAll[i][idxAll[i]] != 0) idxAll[i] ++;
                        }
                        if (idxAll[i] == size) {
                            isSeries[i] = true;
                            break;
                        }
                    }
                }
                if(isSeries[i] == false)    //不能连续存
                {
                    for(int j = l;j <= r;j ++)
                    {
                        if(disk[i].use[j] == -1 || disk[i].use[j] == 0)
                        {
                            spaceAll[i][idxAll[i]] = j;
                            if(idxAll[i] == size) break;
                            if(spaceAll[i][idxAll[i]] != 0) idxAll[i] ++;
                        }
                    }
                }
            }
            else    //从后往前存
            {
                // 先找连续存的位置
                for(int j = r;j >= l; -- j)
                {
                    int cnt = 0;
                    for (int j1 = j; j1 >= max(l,  j  - size + 1); -- j1) {
                        if(disk[i].use[j1] == -1 || disk[i].use[j1] == 0) cnt ++;
                    }
                    if (cnt == size) {
                        for (int j1 = j; j1 >= max(l,  j  - size + 1); -- j1) {
                                spaceAll[i][idxAll[i]] = j1;
                            if(idxAll[i] == size) break;

                            if(spaceAll[i][idxAll[i]] != 0) idxAll[i] ++;
                        }
                        if (idxAll[i] == size) {
                            isSeries[i] = true;
                            break;
                        }
                    }
                }
                if(isSeries[i] == false)    //不能连续存
                {
                    for(int j = r;j >= l;j --)
                    {
                        if(disk[i].use[j] == -1 || disk[i].use[j] == 0)
                        {
                            spaceAll[i][idxAll[i]] = j;
                            if(idxAll[i] == size) break;
                            if(spaceAll[i][idxAll[i]] != 0) idxAll[i] ++;
                        }
                    }
                }          
            }
        }
        //选择目标磁盘
        int dNow = -1;
        int minLast;
        
        if(sted[tag] == true) //从前往后
        {
            minLast = V;
            for(int i = 1;i <= N;i ++)
            {
                if(spaceAll[i][size] != 0 && spaceAll[i][size] < minLast)
                {
                    minLast = spaceAll[i][size];
                    dNow = i;
                }
            }
            if(dNow == -1)   //10个磁盘都放不下，随机选择一个磁盘可以超出l,r范围放
            {
                // 生成1到10之间的随机数作为目标磁盘
                int dd;
                do {
                    dd = std::rand() % 10 + 1;  // 生成 1 到 10 之间的随机数
                } while (dd == object[target].useDisk[1] || dd == object[target].useDisk[2]);  // 如果时副本1时，1和2的位置都是0，不影响
                dNow = dd;
                //重置space
                for(int j = 1;j <= size;j ++) spaceAll[dd][j] = 0;
                int idx = 1;
                for(int j = l;j <= disk[dd].taskList.size() - 1;j ++)
                {
                    if(disk[dd].use[j] == -1 || disk[dd].use[j] == 0)
                    {
                        spaceAll[dd][idx] = j;
                        if(idx == size) break;
                        if(spaceAll[dd][idx] != 0) idx ++;
                    } 
                }
                if(spaceAll[dd][size] == 0)
                {
                    // if(spaceAll[idxAll[i]] == 0) idxAll[i] --;
                    if(l != 1)
                    {
                        for(int k = l - 1;k >= 1;k --)
                        {
                            if(disk[dd].use[k] == -1 || disk[dd].use[k] == 0)
                            {
                                spaceAll[dd][idx] = k;
                                if(idx == size) break;
                                if(spaceAll[dd][idx] != 0) idx ++;
                            }
                        }
                    }
                    
                }
            }
        }
        else
        {
            minLast = 0;
            for(int i = 1;i <= N;i ++)
            {
                if(spaceAll[i][size] != 0 && spaceAll[i][size] > minLast)
                {
                    //printf("task: %d final:%d\n",i,spaceAll[i][size]);
                    minLast = spaceAll[i][size];
                    dNow = i;
                }
            }
            if(dNow == -1)   //10个磁盘都放不下，随机选择一个磁盘可以超出l,r范围放
            {
                // 生成1到10之间的随机数作为目标磁盘
                int dd;
                do {
                    dd = std::rand() % 10 + 1;  // 生成 1 到 10 之间的随机数
                } while (dd == object[target].useDisk[1] || dd == object[target].useDisk[2]);  // 如果时副本1时，1和2的位置都是0，不影响
                dNow = dd;
                //重置space
                for(int j = 1;j <= size;j ++) spaceAll[dd][j] = 0;
                int idx = 1;
                for(int j = r;j >= 1;j --)
                {
                    if(disk[dd].use[j] == -1 || disk[dd].use[j] == 0)
                    {
                        spaceAll[dd][idx] = j;
                        if(idx == size) break;
                        if(spaceAll[dd][idx] != 0) idx ++;
                    } 
                }
                if(spaceAll[dd][size] == 0)
                {
                    for(int j = r + 1;j <= disk[dd].taskList.size() - 1;j ++)
                    {
                        if(disk[dd].use[j] == -1 || disk[dd].use[j] == 0)
                        {
                            spaceAll[dd][idx] = j;
                            if(idx == size) break;
                            if(spaceAll[dd][idx] != 0) idx ++;
                        } 
                    }
                    
                }
            }
        }


        //printf("\n");
        d = dNow;
        //cerr<<"disk:"<<d<<endl;
        for(int i = 1;i <= size;i ++)
        {
            space[i] = spaceAll[dNow][i];
            //cerr<<space[i]<<' ';
        }

        //cerr<<endl;
        //存完的在磁盘同步一下
        //disk[d].empty -= size;
        //同步object
        object[target].useDisk[st] = d;  //1号磁盘为read盘
        object[target].hash[d] = st;
        object[target].useDiskBlock[st].resize(size + 1);
        int ss = 1;
        for(int t = 1;t <= size;t ++)
        {
            object[target].useDiskBlock[st][ss ++] = space[t];
            disk[d].use[space[t]] = target;
        }
        printf("%d ",d);
        for(int t = 1;t <= size;t ++)
        {
            printf("%d ",space[t]);
        }
        printf("\n");
    }
    


    inline void findStore(int st,int target)
    {
        int tag = object[target].tag;
        int size = object[target].size;

        int start = storeSum;
        int idx = 1;
        int space[6];
        int d;
        
        
        int n = N;
        bool di[11]; //被使用过的磁盘
        memset(di,false,sizeof(di));
        int maxD;
        while(n --)
        {
            int maxEmpty = 0;
            
            
            for(int i = 1;i <= N;i ++)
            {
                //printf("di%d\n",di[i]);
                if(di[i] == true) continue;
                if(maxEmpty < disk[i].empty)
                {
                    maxEmpty = disk[i].empty;
                    maxD = i;
                }
            }
            di[maxD] = true;    //选中
            //printf("maxd:%d\n",maxD);
            bool used = false;
            for(int j = 1;j < st;j ++)
            {
                //printf("usedisk %d\n",object[target].useDisk[j]);
                if(object[target].useDisk[j] == maxD) used = true;    //避开前几个副本
            }
            if(used == true) continue;

            idx = 1;
            d = maxD;
            memset(space,0,sizeof(space));
            for(int j = V;j >= 1;j --)
            {
                if(disk[maxD].use[j] == 0)
                {
                    space[idx] = j;
                    if(idx == size) break;
                    idx ++;
                }
            }
            // if(size == 1 && space[1] == 0) continue;
            if(idx == size && space[idx] != 0) //同步到disk.use中
            {
                break;
            }

        }

        //存完的在磁盘同步一下
        disk[d].empty -= size;
        //同步object
        object[target].useDisk[st] = d;  //1号磁盘为read盘
        object[target].hash[d] = st;
        object[target].useDiskBlock[st].resize(size + 1);
        int ss = 1;
        for(int t = 1;t <= idx;t ++)
        {
            object[target].useDiskBlock[st][ss ++] = space[t];
            disk[d].use[space[t]] = target;
        }
        printf("%d ",d);
        for(int t = 1;t <= idx;t ++)
        {
            printf("%d ",space[t]);
        }
        printf("\n");
    }
    void writeOpt(int start,int end)
    {
        for(int i = start;i <= end;i ++)
        {
            int size = object[i].size;
            //输出存储对象序号
            printf("%d\n",i);

            findBestStore(1,i);

            for(int j = 2;j <= 3;j ++)
            {
                findStore(j,i);
            }
        }
    }

    void writeAction(int tmp)
    {

        static int ask = 0;
        int n_write;
        n_write = nWrites[tmp];
        if (n_write == 0) return;
        //统计本次所有输入
        int start = 0,end = 0;
        for (int i = 1; i <= n_write; ++ i) {
            int id, size, tag;
            const auto& write_tuple = writeDatas[tmp].at(i - 1);
            id = get<0>(write_tuple);
            size = get<1>(write_tuple);
            tag = get<2>(write_tuple);
            // 修改为更替后的tag
            tag = objToTag[id].x;

            object[id].size = size, object[id].tag = tag;

            if(i == 1) start = id;
            if(i == n_write) end = id;
        }
        
        writeOpt(start,end);
        fflush(stdout);
    }

    /*****读取操作****/
    void readAction(int tmp, int& deadSum, int interval)
    {

        int n_read;
        int requestId, objectId;
        vector<int> readFinish, busyRequest; //本磁盘本次行动任务完成队列
        n_read = nReads[tmp];

        for (int i = 1; i <= n_read; i++) {
            const auto& read_tuple = readDatas[tmp].at(i - 1);
            requestId = get<0>(read_tuple);
            objectId = get<1>(read_tuple);

            //先判读该请求是否可以直接忽略
            int size = object[objectId].size;
            int tag = object[objectId].tag;
            int tmp1800 = (tmp - 1) / interval;

            requestCnt[tag] += size;

            //如果在接下来的1800个时间片上平均每个时间片读块数量小于等于1，则该tag在接下来1800时间片上的读请求全部忽略
            if(((tmp >= 45000 && (!clusterHigh.count(tag) || mdReal[tag][tmp1800] < 2))))
            {
                // cerr << "********************: " << read_sum[tag][tmp1800] << endl;
                if (!request[requestId].isBusy) {
                    busyRequest.emplace_back(requestId);
                }
                // 上报忙碌请求
                request[requestId].isBusy = true;
                continue;
            }

            object[objectId].requestGroups.emplace_back(requestId); //方便删除对象时将对应任务删除
            request[requestId].objectId = objectId;
            request[requestId].isDone = false;
            request[requestId].inTime = tmp;    //进入时间

            request[requestId].cnt = 0;     //该请求已经堆读取的对象块数量
            
            for(int j = 1;j <= size;j ++)
            {
                request[requestId].block[j] = object[objectId].useDiskBlock[1][j];  //第一副本存的块
            }
            
            int d = object[objectId].useDisk[1];    //目标读取的磁盘
            disk[d].readTask ++;    //队列任务数量加一
            for(int j = 1;j <= size;j ++)
            {
                int objPos = object[objectId].useDiskBlock[1][j];//在磁盘中出现的位置

                disk[d].taskList[objPos].push(requestId);   //给这些位置的队列加上读取id
            }
        }

        //！！！！！！！！！！！可以根据读的规律来，倾向于靠近最近常读的那些类型所在位置
        for(int i = 1;i <= N;i ++)
        {

            // 获取磁盘
            auto& dk = disk[i];


            for(int j = 1;j <= dk.taskList.size() - 1; ++ j)
            {
                while (!disk[i].taskList[j].empty()) {
                    int re = disk[i].taskList[j].front();
                    int time = tmp - request[re].inTime;
                    if ( time >= 105)
                    {

                        if (j <= headBoard[i][0].y && j >= headBoard[i][0].x) loseScore[0] ++;
                        else loseScore[1] ++;
                        loseScoreinTag[object[request[re].objectId].tag] ++ ;

                        deadSum ++;
                        if (!request[re].isBusy) {
                            busyRequest.emplace_back(re);
                        }
                        // 上报忙碌请求
                        request[re].isBusy = true;           
                        disk[i].taskList[j].pop();

                    } else {
                        break;
                    }                 
                }

            }
            for (int i1 = 0; i1 < 2; ++ i1 ) {
                bool finish = false;    //是否结束本时间片的操作
                //将令牌用完，或者队列中没任务为止
                bool isJump = false;
                auto& head = disk[i].head[i1];
                int& headPos = head.curPos;
                head.reset();
                int sz = dk.taskList.size();
                // int start = (i1 == 0 ? 1 : (int)(sz / 2) + 1);
                // int end = (i1 == 0 ? (int)(sz / 2) : sz - 1);
                int start = (i1 == 0 ? headBoard[i][0].x : headBoard[i][1].x);
                int end = (i1 == 0 ? headBoard[i][0].y : headBoard[i][1].y);
                // cerr << "**********************： " << start << " " << end << endl;
                //删除所有超时的请求和已经读过的请求
                // 删除所有读取的块
                /*************************************************/

                while(head.remainToken > 0)    //剩余令牌数大于0且任务队列中还有任务，继续行动
                {
                    // head = disk[i].head.curPos; 
                    int tokenRemain =head.remainToken; //本轮剩余令牌数
                    char op = '0';        //操作类型
                    int pos = 0;    //目标位置


                    int ne = 0; 
                    // 处理空读
                    int sz = disk[i].taskList.size();
                    if (dk.taskList[headPos].empty()) {
                        // [64, 52, 41, 33, 26,]
                        // 判断后面隔两个空有没有需要读的
                        int situ[7] = {65, 45, 35, 22, 18, 16, 16};
                        // 大于等于8基本都是赚的
                        int maxDistance = 9;                                                                                                                                                  ;
                        for (int j = headPos + 1; j <= min(end, headPos + maxDistance ); ++ j) {
                            int pos = j;
                            int select = 0;
                            if (j - headPos + 1 >= 6 && (tmp > 10000 && tmp < 20000)) {
                                select = 0;
                            } else if (j - headPos + 1 >= 6 && (tmp > 20000 && tmp < 80000)) {
                                if (j - headPos >= 7) {
                                    select = 1; 
                                } else {
                                    select = 1;
                                }
                            } 

                            if (head.isRead && head.lastUseToken <= situ[select] && !disk[i].taskList[pos].empty()) {
                                int sub = j - headPos;
                                if (sub >= 9 && (tmp > 18000 && tmp < 78000)) {
                                    int cnt = 0;
                                    for (int j1 = j + 1; j1 < min(end, j + 10); ++ j1) {
                                        if (disk[i].taskList[j1].size()) cnt += disk[i].taskList[j1].size();
                                    }
                                    if (cnt >= 5 ) {
                                        // 可以连续读
                                        ne = headPos;
                                        break;
                                    }
                                } else if (sub < 9){
                                    ne = headPos;
                                    break;
                                }

                            }
                        }

                    }
                    if (ne == 0) {
                        // 说明前面连读无效
                        static int cnt[10000];

                        for(int j = headPos;j <= end;j ++)
                        {
                            if(!disk[i].taskList[j].empty())
                            {   
                                int cnt = 0;
                                for (int j1 = j + 1; j1 <= min(end, j + 10); ++ j1) {
                                    if (disk[i].taskList[j1].size()) cnt += disk[i].taskList[j1].size();
                                    // else {
                                    //     break;
                                    // }
                                }
                                // // 寻找数量大于等于2的位置(这里还是太粗糙了，可以自适应，比如根据磁盘上连续最大请求的数量，来决定跳过的阈值)
                                bool flag1_0 = (tmp >=  34200 && tmp <= 84000 && disk[i].taskList[j].size() == 1 && cnt < 9 && j != headPos);
                                bool flag1_1 = (tmp >=  34200 && tmp <= 84000  && disk[i].taskList[j].size() == 2 && cnt < 7 && j != headPos);
                                if ( false ) {}
                                else {
                                    ne = j;
                                    break;
                                }

                            }
                        }
                    }

                    //去下一步需要走的距离
                    int step = abs(ne - headPos);
                    fflush(stdout);
                    if(step > 0)    //判断怎么走
                    {
                        if(ne == 0)
                        {
                            
                        if(head.isCanExec('j'))//后面没任务了，就回到起点
                            {
                                op = 'j';
                                //pos = 1;
                                int ne = 0; 
                                int posOne = 1;
                                for(int j = start;j <= end ;j ++)
                                {
                                    if(!disk[i].taskList[j].empty())
                                    {
                                        int cnt = 0;
                                        for (int j1 = j + 1; j1 <= min(end, j + 10); ++ j1) {
                                            cnt += disk[i].taskList[j1].size();
                    
                                        }
                                        bool flag = (tmp >= 18000 && tmp <= 78000 && disk[i].taskList[j].size() == 1 && cnt < 3 && j != headPos);
                                        bool flag1 = (tmp >= 18000 && tmp <= 78000 && disk[i].taskList[j].size() == 2 && cnt < 2 && j != headPos);
                                        if (flag || flag1) {}
                                        else {
                                            ne = j;
                                            break;
                                        }
                                    }


                                    if (disk[i].taskList[j].size() > 0 && posOne != 1) {
                                        posOne = j;
                                    }
                                }

                                if(ne == 0) pos = posOne;
                                else pos = ne;
                            }
                            else
                            {
                                finish = true;
                            }
                        }
                        else    //后面有任务，可以走
                        {
                            if(step <= tokenRemain)     //能走到
                            {
                                op = 'p';
                                pos = step; //p的数量
                            }
                            else if(step < G)//在本轮中走不到目标位置但是下一轮可以走到,那就先走到最远位置
                            {
                                op = 'p';
                                pos = tokenRemain;
                            }
                            else    //本轮走不到
                            {

                                if(head.isCanExec('j'))//直接跳到ne
                                {
                                    op = 'j';
                                    pos = ne;
                                }
                                else    //令牌不够，等一回合
                                {
                                    finish = true;
                                }
                            }
                        }

                        //打印
                        if(op == 'j') 
                        {
                            
                        
                            isJump = true;
                            printf("%c %d\n",op,pos);
                            head.updateNextStep(op,pos);    //更新输出与数据
                        }
                        else if(op == 'p')
                        {
                            while(pos --) 
                            {
                                printf("%c",op);
                                head.updateNextStep(op,pos);    //更新输出与数据
                            }
                        }
                    }
                    else    //读（读完这个连续块或者时间用完）
                    {

                        // 可能会空读
                        if (disk[i].taskList[ne].empty()) {
                            if(!head.isCanExec('r'))
                            {
                                finish = true;
                            
                            } else {
                                op = 'r';
                                pos = 1;
                                printf("%c",op);
                            head.updateNextStep(op,pos);    //更新输出与数据       
                            }

                    

                        } else {
                            int idx = ne;   //从目标位置开始
                            //如果这个位置有任务并且位置没有超出上限
                            while(idx <= end && !disk[i].taskList[idx].empty())
                            {
                                //idx块有读取，但是令牌不够进行读取，则跳出循环
                                if(!head.isCanExec('r'))
                                {
                                    finish = true;
                                    break;
                                }
                                op = 'r';
                                pos = 1;
                                printf("%c",op);
                                int read = disk[i].taskList[idx].front();
                                int obj = request[read].objectId;
                                head.updateNextStep(op,pos);    //更新输出与数据
        
                                //处理这个块上的所有任务
                                while(!disk[i].taskList[idx].empty())
                                {
                                    int read = disk[i].taskList[idx].front();
                                    //printf("read %d\n",read);
                                    //同步任务是否被读完
                                    int obj = request[read].objectId;
                    
                                    for(int l = 1;l <= object[obj].size; ++ l)
                                    {
                                        //该block->idx是属于obj的存储位置，并且request的这个位置没有被读过
                                        if(idx == object[obj].useDiskBlock[1][l] && request[read].block[l] != 0)
                                        {
                                            request[read].cnt ++;
                                            request[read].block[l] = 0;
                                            if(request[read].cnt == object[obj].size)   //该任务所有块都被读完了
                                            {
                                                readFinish.emplace_back(read); //加入任务完成列表
                                                request[read].isDone = true;
                                                disk[i].readTask --;
                                                // getScoreReally[i1] += getScore(request[read].inTime, tmp, true, read);
                                            }
                                        }
                                    }
                                    disk[i].taskList[idx].pop();
                                }


                                idx ++;
                            }
                            
                        
                        }

                    }

                    if(finish == true) break;
                }
                
                if(!isJump) printf("#\n");
            }

        }




        printf("%d\n",readFinish.size());
        for(int i = 0;i < readFinish.size();i ++)
        {
            int readId = readFinish[i];
            int tt = tmp - request[readId].inTime;
            if(tt > 105) deadSum ++;
            printf("%d\n",readFinish[i]);

            // score[object[ request[readId].objectId].tag] += calculateScore(request[readId].inTime,tmp, true, readId);
        }
        printf("%d\n",busyRequest.size());
        for(int i = 0;i < busyRequest.size();i ++)
        {
            int readId = busyRequest[i];
            printf("%d\n", busyRequest[i]);

            // score[object[ request[readId].objectId].tag] += calculateScore(request[readId].inTime,tmp, true, readId);
        }
        fflush(stdout);
    }

    // 垃圾回收机制
    void garbageCollectionAction(int tmp) {
        printf("GARBAGE COLLECTION\n");
        for (int i = 1; i <= N; ++ i) {
            int sz = disk[i].taskList.size();
            auto& dk = disk[i];
            // 遍历磁盘中的所有空缺位置
            printf("0\n");


        }
        fflush(stdout);
    }
    /****处理周期上报*******/
    void makeDensity(int interval) {
        // 以interval为周期整体处理一次上报情况
        for (int i = 1; i <= (T - 1) / interval + 1; ++ i) {
            int startTmp = (i - 1) * interval + 1, endTmp = min(i * interval, T + EXTRA_TIME);
            // 处理这段的密度
            for (int j = 1; j <= M; ++ j) {
                double mdTemp = 0.0;
                for (int j1 = startTmp; j1 <= endTmp; ++ j1) {
                    mdTemp += md[j][j1 - 1];
                }
                mdReal[j].push_back(mdTemp);
            }
        }

    }
    /****kmeans++聚类*/
    void kMeans(int period) {
        // 测试数据
        // 周期重新刷新聚类结果
        vector<PDI> data(M);

        for (int j = 1; j <= M; ++ j) {
            data[j - 1] = make_pair(mdReal[j][period], j);
        }
        sort(data.begin(), data.end());
        // 刷新我的聚类
        clusterLow = {};
        clusterHigh = {};

        for (int i = 0; i < data.size() / 2; ++ i) {
            clusterLow.insert(data[i].y);
        }
        int cnt1 = 0, cnt2 = 0;
        unordered_set<int> q1, q2;

        for (int i = 1; i <= 8; ++ i) {
            q1.insert(side[i]);
        }
        for (int i = 9; i <= M; ++ i) q2.insert(side[i]);

        for (int i = data.size() / 2; i < data.size(); ++ i) {
            clusterHigh.insert(data[i].y);
            int tag = data[i].y;
            if (q1.count(tag)) cnt1 ++;
            else if (q2.count(tag)) cnt2 ++;
        }

        if (cnt1 < cnt2) {

            // 给cnt1的一个类
            for (int i = data.size() - 1; i >= 1; -- i) {
                int tag = data[i].y;
                if (!clusterHigh.count(tag) && q1.count(tag)) {
                    clusterHigh.insert(tag);
                    if (++ cnt1 >= cnt2) break;
                }
            } 
        } else if (cnt2 < cnt1) {
    
            // 给cnt2的一个类
            for (int i = data.size() - 1; i >= 1; -- i) {
                int tag = data[i].y;
                if (!clusterHigh.count(tag) && q2.count(tag)) {
                    clusterHigh.insert(tag);
                    if (++ cnt2 >= cnt1) break;
                }
            }   
        }

    
        c1 = cnt1, c2 = cnt2;

 




    }

}SchedulerSecond;

// 第一轮策略
SchedulerFirst schedulerFirst;
// 第二轮策略
SchedulerSecond schedulerSecond;

typedef struct Strategy_{

    // 时间片
    int tmp{};
    int deadSum{};
    // 成员变量
    vector<int> delSum[17];    //对象的对象块操作总次数
    vector<int> writeSum[17];
    vector<int> readSum[17];

    // 第二轮开始的信息
    int nIncre;

    // 初始化函数
    void init() {
        cerr << "Init Start!" << endl;
        preProcess();
        // 初始化空列表
        memset(delSum,0,sizeof(delSum));
        memset(writeSum,0,sizeof(writeSum));
        memset(readSum,0,sizeof(readSum));  
        printf("OK\n");
        fflush(stdout);
    }

    // 时间片对齐函数
    void timestampAction() {
        ++ tmp; // 同步时间片
        int timestamp;
        scanf("%*s%d", &timestamp);
        printf("TIMESTAMP %d\n", timestamp);
        fflush(stdout);
    }

    void midDealData() {
        // 读入第二轮开始给的标签
        scanf("%d", &nIncre);
        for (int i = 1; i <= nIncre; ++ i) {
            int obj, tag;
            scanf("%d%d", &obj, &tag);
            // 讲tag0的类进行更替
            objToTag[obj].x = tag;
        }
        // 每1800为一个周期统计当前周期内与未知obj读请求最接近的类
        static int cnt[MAX_OBJECT_NUM][MAX_TAG_NUM + 1];
        memset(cnt, 0, sizeof cnt);
        int startPeriod = 1, endPeriod = 48;
        for (int period = startPeriod; period <= endPeriod; ++ period) {
            int startTmp = (period - 1) * 1800 + 1, endTmp = period * 1800;
            vector<double> tagAveMd(M + 1, 0);
            unordered_set<int> totalObj[MAX_TAG_NUM + 1];
            for (int j = startTmp; j <= endTmp; ++ j) {
                for (const auto& read_tuple : readDatas[j]) {
                    int requestId = get<0>(read_tuple);
                    int objId = get<1>(read_tuple);
                    int tag = objToTag[objId].x;
                    int size = objToTag[objId].y;
                    if (tag > 0) {
                        tagAveMd[tag] ++;
                        totalObj[tag].insert(objId);
                    }
                }
            }
            // cerr << "*****************************" << endl;
            // 计算密度(这里的密度可以用中位数代替可能效果更好)
            for (int j = 1; j <= M; ++ j) {
                tagAveMd[j] = tagAveMd[j] / max(1.0, 1.0 * totalObj[j].size());
            }
            // 对未知tag的对象进行处理
            for (int j = 1; j <= maxObjectId; ++ j) {
                int tag = objToTag[j].x;
                if (tag == 0) {
                    // 对象tag位置
                    // 二分该对象请求落在上述区间时间片中的读请求数量(startTmp, endTmp), 找到第一个大于等于startTmp的位置,以及一个大于endTmp的位置
                    auto pos1 = lower_bound(requestInObj[j].begin(), requestInObj[j].end(), startTmp);
                    auto pos2 = upper_bound(requestInObj[j].begin(), requestInObj[j].end(), endTmp);
                    int readCnt  = pos2 - pos1;
                    if (readCnt <= 2) continue;
                    double best = 10000;
                    // 遍历所有类的当前轮的读密度
                    for (int j1 = 1; j1 <= M; ++ j1) {
                        // 寻找与这个对象最接近的读请求次数
                        if (abs(tagAveMd[j1] - 1.0 * readCnt) < best) {
                            best = abs(tagAveMd[j1] - 1.0 * readCnt);
                            tag = j1;
                        }

                    }
                    
                    // 找到最好的tag对cnt++
                    cnt[j][tag] ++;
                    // cerr << "objId: " << j << " " << readCnt << " tag:" << tag  << " " << tagAveMd[tag] << endl;
                }
            }
        }

        // 现在最多的cnt进行类的赋值
        for (int i = 1; i <= maxObjectId; ++ i) {
            int tag = objToTag[i].x;
            int size = objToTag[i].y;
            if (tag == 0) {
                int bestCnt = -1;
                for (int j = 1; j <= M; ++ j) {
                    if (cnt[i][j] > bestCnt) {
                        bestCnt = cnt[i][j];
                        tag = j;
                    }
                }
                objToTag[i].x = tag;
            }

            
        }

        static int allTags[MAX_TAG_NUM + 1];
        memset(allTags, 0, sizeof allTags);
        int c1 = 0;
        for (int i = 1; i <= maxObjectId; ++ i) {
            allTags[objToTag[i].x] ++;
            int size = objToTag[i].y;
            if (requestInObj[i].size() * size <= 10) {
                c1 ++;
            }
        }
        cerr << "没有被读的对象数量或者请求块总数小于10: " << c1 << endl;
        for (int i = 0; i <= M; ++ i) {
            cerr << "tag: " << i << " " << allTags[i] << endl;
        }

        // 处理delSum, writeSum, readSum

        for (int i = 1; i <= M; ++ i) {
            delSum[i].resize(T + EXTRA_TIME + 1);
            writeSum[i].resize(T + EXTRA_TIME + 1);
            readSum[i].resize(T + EXTRA_TIME + 1);
        }

        for (int i = 1; i <= T + EXTRA_TIME; ++ i) {
             vector<int> de(M + 1, 0), we(M + 1, 0), re(M + 1, 0);
            // 删除
            for (const int& id : deleteDatas[i]) {
                int tag = objToTag[id].x;
                int size = objToTag[id].y;
                de[tag] += size;
            }
            // 写入
            for (const auto& write_tuple : writeDatas[i]) {
                int id = get<0>(write_tuple);
                int size = get<1>(write_tuple);
                int tag = get<2>(write_tuple);
                tag = objToTag[id].x;
                we[tag] += size;
            }
            // 读取
            for (const auto& read_tuple : readDatas[i]) {
                int requestId = get<0>(read_tuple);
                int objId = get<1>(read_tuple);
                int tag = objToTag[objId].x;
                int size = objToTag[objId].y;
                re[tag] += size;
            }

            for (int j = 1; j <= M; ++ j) {
                delSum[j][i - 1] = de[j];
                writeSum[j][i - 1] = we[j];
                readSum[j][i - 1] = re[j];
            }
        }
    }
    
    // 总调度
    void totalRound() {

        cerr << "firstRound start!" << endl;
        // 第一轮调度
        firstRound();
        cerr << "firstRound over!" << endl;
        cerr << "start information deal" << endl;
        // 中间数据处理
        tmp = 0;
        deadSum  = 0;
        midDealData();
        cerr << "Mid Information Has Done!" << endl;
        cerr << "SecondRound Start!" << endl;
        // 第二轮调度
        secondRound();
    }

    // 第一轮调度
    void firstRound() {
        schedulerFirst.spacePrepare();
        int period = 0;
        for (int t = 1; t <= T + EXTRA_TIME; ++ t) {
            timestampAction();
            schedulerFirst.deleteAction(t);
            schedulerFirst.writeAction(t);
            schedulerFirst.readAction(t, deadSum);
            if (t % FRE_PER_SLICING == 0) {
                // busy机制
                 schedulerFirst.garbageCollectionAction(t);
            }
            #ifdef LOCAL
            if((t - 1 )% 1800 == 0)
            {
                cerr << "No." << (t - 1) / 1800 <<' '<< "period" << endl;
                cerr << "disk 1 score: " << getScoreReally[0] << endl;
                cerr << "disk 2 score: " << getScoreReally[1] << endl;
                cerr<<"deadSum:" << deadSum <<endl;
            }
            #endif
        }
    }

    // 第二轮调度
    void secondRound() {
        schedulerSecond.findClosestTags(readSum);
        schedulerSecond.spacePrepare(writeSum, delSum, readSum);

        // 预处理平分划分的密度
        int interval = 180;
        schedulerSecond.makeDensity(interval);
        int period1 = 0, period = 0;
        for (int t = 1; t <= T + EXTRA_TIME; ++ t) {

            if((t - 1) % 180 == 0)
            {
                
                schedulerSecond.kMeans(period1);
                ++ period1;
            } 
            timestampAction();
            schedulerSecond.deleteAction(t);
            schedulerSecond.writeAction(t);
            schedulerSecond.readAction(t, deadSum, interval);
            if (t % FRE_PER_SLICING == 0) {
                // busy机制
                 schedulerSecond.garbageCollectionAction(t);
            }
            #ifdef LOCAL
            if((t - 1 )% 1800 == 0)
            {
                ++ period1;
                cerr << "No." << (t - 1) / 1800 <<' '<< "period" << endl;
                cerr << "disk 1 score: " << getScoreReally[0] << endl;
                cerr << "disk 2 score: " << getScoreReally[1] << endl;
                cerr<<"deadSum:" << deadSum <<endl;

            }
            #endif

        }
    }

}Strategy;



int main() {

    if (fopen("in.in", "r") != nullptr) {
        freopen("in.in", "r", stdin);
        // 重定向输出流到 result.txt 文件
        freopen("test.txt", "w", stdout);
    } else {
        cerr << "open file uncorrectly" << endl;
    }
    scanf("%d%d%d%d%d%d", &T, &M, &N, &V, &G, &K); 
    static Strategy strategy;
    strategy.init();
    // 总调度
    strategy.totalRound();
}