//
// Created by wmj on 3/19/20.
//

#ifndef OS_STRING_H
#define OS_STRING_H

#include <stddef.h>
#include <vector.hpp>
namespace std {
class string {
private:
    char *data = nullptr;
    void copy_str(const char *str, size_t len=-1);
    size_t len = 0;
public:
    string() = default;
    string(const char *);
    string(const char *s, size_t cnt);
    string(const string&);
    bool   operator==(const char *) const;
    bool   operator==(const string&) const;
    bool   operator!=(const char *) const;
    bool   operator!=(const string&) const;
    string substr(size_t s, size_t len) const;
    char&  operator[](int i);
    char operator[](int i) const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] const char *c_str() const;

    ~string();
};
    std::vector<string> split(const string& s, char sp);
}


#endif //OS_STRING_H
