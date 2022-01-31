#include "MonkeyTest/Lexer.h"

#include <TestFramework/Test.h>

int main(void)
{
    // Dump leaks to stderr.
    LEAK_SET_STDERR();

    // Initialize the test state.
    // This tracks test passes and an assertion count.
    TestState testState = {0};

    // Run the tests.
    TEST_RUN_SUITE(Lexer, &testState);

    // Verify the absence of leaks.
    if (LEAK_CHECK())
    {
        return 1;
    }

    TEST_SUMMARY(&testState);
    // Return 0 on success.
    return testState.numFailures;
}
