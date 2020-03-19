//
// Created by wmj on 3/19/20.
//

#ifndef OS_OSTREAM_H
#define OS_IOSTREAM_H

namespace std {

class ostream {
private:
    int fno;
public:
    ostream(int fno);
    ostream &operator <<(const char *);
};

extern ostream cout;
extern ostream cerr;

constexpr const char *endl = "\n";

}


#endif //OS_OSTREAM_H
