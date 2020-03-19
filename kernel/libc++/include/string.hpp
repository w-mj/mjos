//
// Created by wmj on 3/19/20.
//

#ifndef OS_STRING_H
#define OS_STRING_H

namespace std {
class string {
private:
    char *data = nullptr;
    void copy_str(const char *str);
public:
    string() = default;
    explicit string(const char *);
    string(const string&);
    string &operator=(const string &) = default;

    [[nodiscard]] const char *c_str() const;

    ~string();
};
}


#endif //OS_STRING_H
