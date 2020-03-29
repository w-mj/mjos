#include <test.h>
#include "include/algorithm.hpp"

class TestLibCXX: public Test {
public:
    TestLibCXX() {
        registerTest(TestLibCXX::testMin);
    }

    void testMin() {
        assertEqual(os::min(1, 2, 3, 4, 5), 2);
    }

};

int main(void) {
    TestLibCXX test;
    test.testAll();
    using namespace std;
    cout << "TEST" << endl;
}