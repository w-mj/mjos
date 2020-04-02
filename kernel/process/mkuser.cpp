// 用于生成eflpiggy.S


#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

vector<string> user_list = {"shell"};
int main(void) {
    string base_path = "../../user/";
    ofstream elfpiggy;
    elfpiggy.open("elfpiggy.S", ios_base::out| ios::trunc);
    elfpiggy << "#include <linkage.h>" << endl;
    ofstream userprocesses;
    userprocesses.open("userprocesses.c", ios_base::out| ios::trunc);
    for (const auto &x: user_list) {
        string path = base_path + x;
        elfpiggy << ".balign 4096" << endl;
        elfpiggy << "GLOBAL(" << x << "_start)" << endl;
        elfpiggy << "    .incbin \"" << base_path << x << "/" << x << ".o\"" << endl;
        elfpiggy << "GLOBAL(" << x << "_end)" << endl;
        userprocesses << "extern void *" << x << "_start;" << endl;
    }
    userprocesses << "void *user_processes[] = {" << endl;
    for (const auto &x: user_list) {
        userprocesses << "    &" << x << "_start," << endl;
    }
    userprocesses << "    (void *)0\n};" << endl;
    userprocesses.close();
    elfpiggy.close();
}