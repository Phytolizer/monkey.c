#include "MonkeyTest/Lexer.h"

#include <TestFramework/Test.h>

int main(void)
{
    TestState testState = {0};
    TEST_RUN_SUITE(Lexer, &testState);
    return testState.numFailures;
}
