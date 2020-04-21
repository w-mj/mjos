#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cassert>

using namespace std;
using Element = pair<uint64_t, int>;

// 返回数组的下标i满足：
// i <= list.size() : list[i - 1].first < target.first && list[i].first >= target.first
// i == 0           : list[0] >= target.first
int find_between(const vector<Element> &list, const Element &target) {
    int e = list.size();
    int s = 0, m;
    while (s < e) {
        m = (s + e) / 2;
        if (list[m].first < target.first) {
            s = m + 1;
        } else if (list[m].first > target.first) {
            e = m;
        } else {
            return m;
        }
    }
    return s;
}

void printList(const vector<Element> &list) {
    cout << "Inner malloc list" << endl;
    for (auto &x: list) {
        cout << hex << x.first << " "<< x.second << "     " << x.first + x.second << endl;
    }
}

void insert(vector<Element> &list, const Element &target) {
    int i = find_between(list, target);
    uint64_t frontEnd = 0, backStart = 0, targetEnd = target.first + target.second;
    if (i > 0) {
        frontEnd = list[i - 1].first + list[i - 1].second;
    }
    if (i < list.size()) {
        backStart = list[i].first;
    }
    if (target.first < frontEnd || (backStart != 0 && targetEnd > backStart)) {
        // 重叠错误
        cout << "ERROR ! insert " << hex << target.first << " " << dec << target.second << endl;
        printList(list);
        exit(-1);
    }
    if (target.first == frontEnd && targetEnd == backStart) {
        // 双向合并
        list[i - 1].second += target.second + list[i].second;
        list.erase(list.begin() + i);
    } else if (target.first == frontEnd) {
        // 向前合并
        list[i - 1].second += target.second;
    } else if (targetEnd == backStart) {
        // 向后合并
        list[i].first = target.first;
        list[i].second += target.second;
    } else {
        // 插入
        list.insert(list.begin() + i, target);
    }
}

void remove(vector<Element> &list, const Element &target) {
    int i = find_between(list, target);
    uint64_t frontEnd = 0, backStart = 0, targetEnd = target.first + target.second;
    if (i == list.size()) {
        cout << "ERROR i == list.size()" << endl;
        exit(-1);
    }
    if (i == 0 && list[0].first != target.first) {
        cout << "ERROR i == 0 && list[0].first != target.first" << endl;
        exit(-1);
    }
    if (i == 0) {
        assert(target.second <= list[0].second);
        if (target.second == list[0].second) {
            list.erase(list.begin());
        } else {
            list[0].second -= target.second;
        }
        return ;
    }
    frontEnd = list[i - 1].first + list[i - 1].second;
    backStart = list[i].first;
    if (targetEnd > frontEnd && target.first < backStart) {
        // 不存在
        cout << "ERROR ! remove" << target.first << " " << target.second << endl;
        exit(-1);
    }
    if (target.first == backStart) {
       assert(target.second <= list[i].second);
       if (target.second == list[i].second) {
           list.erase(list.begin() + i);
       } else {
           list[i].second -= target.second;
       }
    } else {
        // target落在前一个区间内
        if (frontEnd == targetEnd) {
            // 与前一个区间结束位置相同
            list[i - 1].second -= target.second;
        } else {
            // 划分前一个区间
            list[i - 1].second = (int)(target.first - list[i - 1].first);
            list.insert(list.begin() + i, make_pair(targetEnd, (int)(frontEnd - targetEnd)));
        }
    }
}

int main() {
    ifstream file("../../log.txt", ios::in);
    string str;
    vector<Element> mallocList;
    while (file >> str) {
        uint64_t addr;
        int len;
        if (str == "kmalloc") {
            file >> dec >> len >> hex >> addr;
            cout << "kmalloc " << hex << addr << " " <<len << endl;
            insert(mallocList, make_pair(addr, len));
            printList(mallocList);
        } else if (str == "kfree") {
            file >> dec >> len >> hex >> addr;
            cout << "kfree " << hex << addr << " " << len << endl;
            remove(mallocList, make_pair(addr, len));
            printList(mallocList);
        }
    }
    return 0;
}