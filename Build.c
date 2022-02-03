#include "Build.h"

#include <string.h>

#ifdef _WIN32
#define OBJ_SUFFIX ".obj"
#define EXE_SUFFIX ".exe"
#define LIB_PREFIX ""
#define LIB_SUFFIX ".lib"
#define CFLAGS                                                                                                         \
    "/W4", "/WX", "/std:c11", "/diagnostics:caret", "/nologo", "/D_CRT_SECURE_NO_WARNINGS", "/external:W0",            \
        "/external:anglebrackets"
const char* cflagsArray[] = {CFLAGS, NULL};
void BuildCFile(const char* file, const char* out, ...)
{
    size_t numIncludeDirs = 0;
    va_list args;
    FOREACH_VARGS(out, arg, args, { numIncludeDirs += 1; });
    const char** includeDirs = calloc(numIncludeDirs + 1, sizeof(char*));
    size_t i = 0;
    FOREACH_VARGS(out, arg, args, {
        includeDirs[i] = CONCAT("/I", arg);
        i += 1;
    });
    ARRAY_CMD("1*111*", "cl.exe", cflagsArray, "/c", file, CONCAT("/Fo", out), includeDirs);
}

void LinkCExecutable(const char* const* inputs, const char* out, const char* const* libs)
{
    ARRAY_CMD("1*1*11*", "cl.exe", cflagsArray, CONCAT("/Fe", out), inputs, "/link", "/SUBSYSTEM:CONSOLE", libs);
}
void LinkCStaticLib(const char* const* inputs, const char* out)
{
    ARRAY_CMD("11*1", "lib.exe", CONCAT("/OUT:", out), inputs, "/nologo");
}
#else
#define OBJ_SUFFIX ".o"
#define EXE_SUFFIX ""
#define LIB_PREFIX "lib"
#define LIB_SUFFIX ".a"
#define CFLAGS "-Wall", "-Wextra", "-Wpedantic", "-std=gnu11", "-Werror", "-ggdb"
const char* cflagsArray[] = {CFLAGS, NULL};
void BuildCFile(const char* file, const char* out, ...)
{
    size_t numIncludeDirs = 0;
    va_list args;
    FOREACH_VARGS(out, arg, args, { numIncludeDirs += 1; });
    const char** includeDirs = calloc(numIncludeDirs + 1, sizeof(char*));
    size_t i = 0;
    FOREACH_VARGS(out, arg, args, {
        includeDirs[i] = CONCAT("-I", arg);
        i += 1;
    });
    ARRAY_CMD("1*1111*", "cc", cflagsArray, "-c", file, "-o", out, includeDirs);
}
void LinkCExecutable(const char* const* inputs, const char* out, const char* const* libs)
{
    ARRAY_CMD("111**", "cc", "-o", out, inputs, libs);
}
void LinkCStaticLib(const char* const* inputs, const char* out)
{
    ARRAY_CMD("111*", "ar", "rcs", out, inputs);
}
#endif

#define BUILD_C_FILE(...) BuildCFile(__VA_ARGS__, NULL)

const char* embedSources[] = {"Embed.c"};
const char* stringSources[] = {"String.c"};
const char* monkeySources[] = {"Ast.c", "Lexer.c", "Parser.c", "Token.c"};
const char* monkeyReplSources[] = {"Main.c", "WhoAmI.c"};
const char* monkeyTestSources[] = {"Main.c", "Lexer.c", "Parser.c"};

int main(void)
{
    const char* outDir = PATH("build");
    MKDIRS(outDir);

    MKDIRS(PATH(outDir, "Embed"));
    const char* embedDir = PATH("Modules", "Embed", "Source");
    const char** embedObjects = calloc((sizeof embedSources / sizeof embedSources[0]) + 1, sizeof(char*));
    if (embedObjects == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for Embed objects\n");
        return 1;
    }

    FOREACH_ARRAY(const char*, embedSource, embedSources, {
        const char* embedObject = PATH(outDir, "Embed", CONCAT(RemoveExtension(embedSource), OBJ_SUFFIX));
        BUILD_C_FILE(PATH(embedDir, embedSource), embedObject);
        embedObjects[i] = embedObject;
    });

    const char* embedExe = PATH(outDir, "Embed", "Embed" EXE_SUFFIX);
    LinkCExecutable(embedObjects, embedExe, EMPTY_ARRAY());

    const char** stringObjects = calloc((sizeof stringSources / sizeof stringSources[0]) + 1, sizeof(char*));
    if (stringObjects == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for String objects\n");
        return 1;
    }

    const char* stringDir = PATH("Modules", "SimpleString", "Source");
    MKDIRS(outDir, "SimpleString");
    FOREACH_ARRAY(const char*, stringSource, stringSources, {
        const char* stringObject = PATH(outDir, "SimpleString", CONCAT(RemoveExtension(stringSource), OBJ_SUFFIX));
        BUILD_C_FILE(PATH(stringDir, stringSource),
                     stringObject,
                     PATH("Modules", "SimpleString", "Include"),
                     PATH("Modules", "WindowsHelpers", "Include"));
        stringObjects[i] = stringObject;
    });

    const char* simpleStringLib = PATH(outDir, "SimpleString", LIB_PREFIX "SimpleString" LIB_SUFFIX);
    LinkCStaticLib(stringObjects, simpleStringLib);

    const char** monkeyObjects = calloc((sizeof monkeySources / sizeof monkeySources[0]) + 1, sizeof(char*));
    if (monkeyObjects == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for Monkey objects\n");
        return 1;
    }

    const char* monkeyDir = PATH("Modules", "Monkey", "Source");
    MKDIRS(outDir, "Monkey");
    FOREACH_ARRAY(const char*, monkeySource, monkeySources, {
        const char* monkeyObject = PATH(outDir, "Monkey", CONCAT(RemoveExtension(monkeySource), OBJ_SUFFIX));
        BUILD_C_FILE(PATH(monkeyDir, monkeySource),
                     monkeyObject,
                     PATH("Modules", "Monkey", "Include"),
                     PATH("Modules", "SimpleString", "Include"),
                     PATH("Modules", "WindowsHelpers", "Include"));
        monkeyObjects[i] = monkeyObject;
    });

    const char* monkeyLib = PATH(outDir, "Monkey", LIB_PREFIX "Monkey" LIB_SUFFIX);
    LinkCStaticLib(monkeyObjects, monkeyLib);

    const char** monkeyReplObjects =
        calloc((sizeof monkeyReplSources / sizeof monkeyReplSources[0]) + 1, sizeof(char*));
    if (monkeyReplObjects == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for MonkeyRepl objects\n");
        return 1;
    }

    const char* monkeyReplDir = PATH("Modules", "MonkeyRepl", "Source");
    MKDIRS(outDir, "MonkeyRepl");
    FOREACH_ARRAY(const char*, monkeyReplSource, monkeyReplSources, {
        const char* monkeyReplObject =
            PATH(outDir, "MonkeyRepl", CONCAT(RemoveExtension(monkeyReplSource), OBJ_SUFFIX));
        BUILD_C_FILE(PATH(monkeyReplDir, monkeyReplSource),
                     monkeyReplObject,
                     PATH("Modules", "Monkey", "Include"),
                     PATH("Modules", "MonkeyRepl", "Include"),
                     PATH("Modules", "SimpleString", "Include"),
                     PATH("Modules", "WindowsHelpers", "Include"));
        monkeyReplObjects[i] = monkeyReplObject;
    });

#ifdef _WIN32
#define MONKEY_REPL_LIBS monkeyLib, simpleStringLib, "Secur32.lib", "Advapi32.lib"
#else
#define MONKEY_REPL_LIBS monkeyLib, simpleStringLib
#endif

    LinkCExecutable(
        monkeyReplObjects, PATH(outDir, "MonkeyRepl", "MonkeyRepl" EXE_SUFFIX), NEW_ARRAY(MONKEY_REPL_LIBS));

    MKDIRS(outDir, "Embedded", "MonkeyTest", "Input", "Lexer");
    const char* nextTokenTestBase = PATH(outDir, "Embedded", "MonkeyTest", "Input", "Lexer");
    CMD(embedExe,
        PATH("Modules", "MonkeyTest", "Input", "Lexer", "NextToken.txt"),
        PATH(ABS_PATH(nextTokenTestBase), "NextToken"));
    const char* nextTokenTestSource = PATH(nextTokenTestBase, "NextToken.c");

    const char** monkeyTestObjects =
        calloc((sizeof monkeyTestSources / sizeof monkeyTestSources[0]) + 2, sizeof(char*));
    monkeyTestObjects[0] = PATH(nextTokenTestBase, "NextToken" OBJ_SUFFIX);
    BUILD_C_FILE(nextTokenTestSource, monkeyTestObjects[0], nextTokenTestBase);
    const char* monkeyTestDir = PATH("Modules", "MonkeyTest", "Source");
    MKDIRS(outDir, "MonkeyTest");
    FOREACH_ARRAY(const char*, monkeyTestSource, monkeyTestSources, {
        const char* monkeyTestObject =
            PATH(outDir, "MonkeyTest", CONCAT(RemoveExtension(monkeyTestSource), OBJ_SUFFIX));
        BUILD_C_FILE(PATH(monkeyTestDir, monkeyTestSource),
                     monkeyTestObject,
                     PATH("Modules", "MonkeyTest", "Include"),
                     PATH("Modules", "Monkey", "Include"),
                     PATH("Modules", "SimpleString", "Include"),
                     PATH("Modules", "TestFramework", "Include"),
                     PATH("Modules", "WindowsHelpers", "Include"),
                     PATH(outDir, "Embedded"));
        monkeyTestObjects[i + 1] = monkeyTestObject;
    });

    LinkCExecutable(
        monkeyTestObjects, PATH(outDir, "MonkeyTest", "MonkeyTest" EXE_SUFFIX), NEW_ARRAY(monkeyLib, simpleStringLib));
}
