#include <test.h>
#include "./stdlib/strtol.c"
#include "./string/strchr.c"
#include "./string/strcmp.c"
#include "./string/strdup.c"
#include "./string/string.c"
#include "./string/strstr.c"

class TestLibc: public Test {
public:
    TestLibc() {
        registerTest(TestLibc::testStdLib);
        registerTest(TestLibc::testString);
    }

    void testStdLib() {
        char *s = (char*)"1234";
        assertEqual(strtol(s, &s, 10), (long)1234);
    }

    void testString() {
        const char *s = "abcdefg";
        assertEqual(test_strchr(s, 'c'), (char *)s + 2);
        assertEqual(test_strcmp("abc", "abc"), 0);
        assertEqual(test_strcmp(test_strdup(s), s), 0);
        assertEqual(test_strlen("abc"), (size_t)3);
        assertEqual(test_strncmp("abc", "abd", 2), 0);
        assertEqual(test_strstr(s, "cde"), (char *)s + 2);
    }
};

int main() {
    TestLibc test;
    test.testAll();
    return 0;
}