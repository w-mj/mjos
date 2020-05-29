#include <test.h>
#include "include/algorithm.hpp"

class TestLibCXX: public Test {
public:
    TestLibCXX() {
        registerTest(TestLibCXX::testMin);
        registerTest(TestLibCXX::testLowest);
    }

    void testMin() {
        assertEqual(os::min(1, 2, 3, 4, 5), 1);
    }

    void testLowest() {
        assertEqual(os::lowest_0(3), (uint8_t)2);
        assertEqual((int)os::lowest_1(0), (int)(sizeof(int) * 8));
    }

};

int main(void) {
    TestLibCXX test;
    test.testAll();
    using namespace std;
    cout << "TEST" << endl;
}