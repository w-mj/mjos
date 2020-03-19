//
// Created by wmj on 3/19/20.
//

#include <string.hpp>
#include <string.h>

using namespace std;

void string::copy_str(const char *str){
    if (str == nullptr)
        return;
    delete[] data;
    size_t len = strlen(str) + 1;
    data = new char[len];
    memcpy(data, str, len);
}


string::string(const char *str) {
    copy_str(str);
}

string::string(const string& ano) {
    copy_str(ano.data);
}

const char *string::c_str() const {
    return data;
}

string::~string() {
    delete[] data;
}
