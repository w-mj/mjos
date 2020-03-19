//
// Created by wmj on 3/19/20.
//

#ifndef OS_OSTREAM_H
#define OS_OSTREAM_H

#include <string.hpp>

namespace std {

class ostream {
private:
    int fno;
public:
    explicit ostream(int fno);
    ostream &operator <<(const char *);
    ostream &operator <<(const string &);
};

extern ostream cout;
extern ostream cerr;

constexpr const char *endl = "\n";

}


#endif //OS_OSTREAM_H
