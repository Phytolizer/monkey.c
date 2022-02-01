#include "Build.h"

#include <string.h>

#ifdef _WIN32
#define OBJ_SUFFIX ".obj"
#define EXE_SUFFIX ".exe"
#define LIB_PREFIX ""
#define LIB_SUFFIX ".lib"
#define CFLAGS "/W4", "/WX", "/analyze", "/std:c11", "/diagnostics:caret", "/nologo", "/D_CRT_SECURE_NO_WARNINGS"
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

#define BUILD_C_FILE(file, out, ...) BuildCFile(file, out, __VA_ARGS__, NULL)
void LinkCExecutable(const char* const* inputs, const char* out)
{
    ARRAY_CMD("1*1*11", "cl.exe", cflagsArray, CONCAT("/Fe", out), inputs, "/link", "/SUBSYSTEM:CONSOLE");
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
#define CFLAGS "-Wall", "-Wextra", "-Wpedantic", "-std=c11", "-Werror", "-O2"
void BuildCFile(const char* file, const char* out)
{
    CMD("cc", CFLAGS, "-c", file, "-o", out);
}
void LinkCExecutable(const char* const* inputs, const char* out)
{
    ARRAY_CMD("111*", "cc", "-o", out, inputs);
}
void LinkCStaticLib(const char* const* inputs, const char* out)
{
    ARRAY_CMD("111*", "ar", "rcs", out, inputs);
}
#endif

const char* embedSources[] = {"Embed.c"};
const char* stringSources[] = {"String.c"};
const char* monkeySources[] = {"Ast.c", "Lexer.c", "Parser.c", "Token.c"};

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
    LinkCExecutable(embedObjects, embedExe);

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

    LinkCStaticLib(stringObjects, PATH(outDir, "SimpleString", LIB_PREFIX "String" LIB_SUFFIX));

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

    LinkCStaticLib(monkeyObjects, PATH(outDir, "Monkey", LIB_PREFIX "Monkey" LIB_SUFFIX));

    MKDIRS(outDir, "Embedded", "MonkeyTest", "Input", "Lexer");
    const char* nextTokenTestBase = PATH(outDir, "Embedded", "MonkeyTest", "Input", "Lexer", "NextToken");
    CMD(embedExe, PATH("Modules", "MonkeyTest", "Input", "Lexer", "NextToken.txt"), ABS_PATH(nextTokenTestBase));
    const char* nextTokenTestSource = CONCAT(nextTokenTestBase, ".c");
}
