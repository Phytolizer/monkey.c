#include "MonkeyTest/Lexer.h"

#include <TestFramework/Test.h>

int main(void)
{
    LEAK_SET_STDERR();
    TestState testState = {0};
    TEST_RUN_SUITE(Lexer, &testState);

    if (LEAK_CHECK())
    {
        return 1;
    }
    TEST_SUMMARY(&testState);
    return testState.numFailures;
}
