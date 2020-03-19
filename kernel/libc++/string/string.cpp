//
// Created by wmj on 3/19/20.
//

#include <string.hpp>
#include <string.h>

using namespace std;

void string::copy_str(const char *str, size_t len){
    if (str == nullptr)
        return;
    delete[] data;
    if (len == -1)
        len = strlen(str) + 1;
    data = new char[len];
    memcpy(data, str, len);
    this->len = len;
}


string::string(const char *str) {
    copy_str(str);
}

string::string(const char *s, size_t cnt) {
    copy_str(s, cnt);
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

bool string::operator==(const char * s1) const {
    return strncmp(data, s1, size) == 0;
}

size_t string::size() const {
    return size;
}

bool string::operator!=(const char *s1) const {
    return ! this->operator==(s1);
}

