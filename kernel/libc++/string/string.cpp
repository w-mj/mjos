//
// Created by wmj on 3/19/20.
//

#include <string.hpp>
#include <string.h>

using namespace os;

void string::copy_str(const char *str, size_t len){
    if (str == nullptr)
        return;
    delete[] data;
    if (len == -1)
        len = strlen(str) + 1;
    data = new char[len];
    memcpy(data, str, len);
    data[len] = 0;
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
    return strncmp(data, s1, len) == 0;
}

size_t string::size() const {
    return len;
}

bool string::operator!=(const char *s1) const {
    return !((*this) == s1);
}

bool string::operator==(const string &ano) const {
    return (*this) == ano.data;
}

bool string::operator!=(const string &ano) const {
    return !((*this) == ano);
}

string string::substr(size_t s, size_t len) const{
    return string(data + s, len);
}

char& string::operator[](int i) {
    return data[i];
}

char string::operator[](int i) const {
    return data[i];
}

string &string::operator=(const string &ano) {
//    if (&ano == this)
//        return *this;
    copy_str(ano.data, ano.size());
    return *this;
}


#include <vector.hpp>
os::vector<os::string> os::split(const os::string& s, char sp) {
    size_t i = 0, l = 0;
    os::vector<os::string> res;
    while (i < s.size()) {
        if (s[i] == sp) {
            if (i != l)
                res.push_back(s.substr(l, i - l));
            l = i + 1;
        }
        i++;
    }
    if (i != l)
        res.push_back(s.substr(l, i - l));
    return res;
}
