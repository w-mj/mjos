//
// Created by wmj on 3/19/20.
//

#ifndef OS_STRING_H
#define OS_STRING_H

namespace std {
class string {
private:
    char *data = nullptr;
    void copy_str(const char *str, size_t len=-1);
    size_t len;
public:
    string() = default;
    string(const char *);
    string(const char *s, size_t cnt);
    string(const string&);
    string &operator=(const string &) = default;
    bool   operator==(const char *) const;
    bool   operator!=(const char *) const;
    [[nodiscard]] size_t size() const;

    [[nodiscard]] const char *c_str() const;

    ~string();
};
}


#endif //OS_STRING_H
