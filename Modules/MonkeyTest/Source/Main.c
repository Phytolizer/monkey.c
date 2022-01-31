#include "MonkeyTest/Lexer.h"

#include <TestFramework/Test.h>
#include <WindowsHelpers/LeakCheck.h>

int main(void)
{
    TestState testState = {0};
    TEST_RUN_SUITE(Lexer, &testState);

    LEAK_CHECK();
    return testState.numFailures;
}
