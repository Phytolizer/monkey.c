#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif
#define PATH_SEP_LEN (sizeof PATH_SEP - 1)

#define FOREACH_VARGS(param, arg, args, body)                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        va_start(args, param);                                                                                         \
        for (const char* arg = va_arg(args, const char*); arg != NULL; arg = va_arg(args, const char*))                \
        {                                                                                                              \
            body;                                                                                                      \
        }                                                                                                              \
    } while (0)

#define FOREACH_ARRAY(type, item, items, body)                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        for (size_t i = 0; i < sizeof items / sizeof(type); i += 1)                                                    \
        {                                                                                                              \
            type item = items[i];                                                                                      \
            body;                                                                                                      \
        }                                                                                                              \
    } while (0)

#ifdef _WIN32
#define FOREACH_FILE_IN_DIR(file, dir, body)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        const char* file = NULL;                                                                                       \
        WIN32_FIND_DATA data;                                                                                          \
        HANDLE h = FindFirstFile(PATH(dir, "*"), &data);                                                               \
        if (h != INVALID_HANDLE_VALUE)                                                                                 \
        {                                                                                                              \
            do                                                                                                         \
            {                                                                                                          \
                if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))                                               \
                {                                                                                                      \
                    file = data.cFileName;                                                                             \
                    body;                                                                                              \
                }                                                                                                      \
            } while (FindNextFile(h, &data));                                                                          \
            FindClose(h);                                                                                              \
        }                                                                                                              \
    } while (0)
#else
#define FOREACH_FILE_IN_DIR(file, dir, body)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        struct dirent* entry;                                                                                          \
        const char* file = NULL;                                                                                       \
        DIR* dirp = opendir(dir);                                                                                      \
        if (dirp == NULL)                                                                                              \
        {                                                                                                              \
            fprintf(stderr, "Failed to open directory %s: %s\n", dir, strerror(errno));                                \
            exit(EXIT_FAILURE);                                                                                        \
        }                                                                                                              \
        while ((entry = readdir(dirp)) != NULL)                                                                        \
        {                                                                                                              \
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)                                   \
                continue;                                                                                              \
            file = entry->d_name;                                                                                      \
            body;                                                                                                      \
        }                                                                                                              \
        closedir(dirp);                                                                                                \
    } while (0)
#endif

const char* VConcatSep(const char* sep, va_list args)
{
    const size_t sepLen = strlen(sep);
    size_t length = 0;
    size_t sepsCount = 0;

    va_list argsCopy;
    va_copy(argsCopy, args);

    for (const char* arg = va_arg(argsCopy, const char*); arg != NULL; arg = va_arg(argsCopy, const char*))
    {
        length += strlen(arg);
        sepsCount += 1;
    }
    sepsCount -= 1;

    assert(length > 0);
    char* result = calloc(length + sepsCount + sepLen + 1, 1);
    if (result == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for concatenated string: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    length = 0;
    for (const char* arg = va_arg(args, const char*); arg != NULL; arg = va_arg(args, const char*))
    {
        size_t argLen = strlen(arg);
        memcpy(result + length, arg, argLen);
        length += argLen;
        if (sepsCount > 0)
        {
            memcpy(result + length, sep, sepLen);
            length += sepLen;
            sepsCount -= 1;
        }
    }

    return result;
}

const char* ArrayConcatSep(const char* sep, const char* const* array)
{
    const size_t sepLen = strlen(sep);
    size_t length = 0;
    size_t sepsCount = 0;

    for (size_t i = 0; array[i] != NULL; i += 1)
    {
        length += strlen(array[i]);
        sepsCount += 1;
    }
    sepsCount -= 1;

    assert(length > 0);
    char* result = calloc(length + sepsCount + sepLen + 1, 1);
    if (result == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for concatenated string: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    length = 0;
    for (size_t i = 0; array[i] != NULL; i += 1)
    {
        size_t argLen = strlen(array[i]);
        memcpy(result + length, array[i], argLen);
        length += argLen;
        if (sepsCount > 0)
        {
            memcpy(result + length, sep, sepLen);
            length += sepLen;
            sepsCount -= 1;
        }
    }

    return result;
}

const char* ConcatSepImpl(const char* sep, ...)
{
    va_list args;
    va_start(args, sep);
    const char* result = VConcatSep(sep, args);
    va_end(args);
    return result;
}

#define CONCAT_SEP(...) ConcatSepImpl(__VA_ARGS__, NULL)

#define PATH(...) CONCAT_SEP(PATH_SEP, __VA_ARGS__, NULL)

const char* AbsPathImpl(int ignore, ...)
{
    va_list args;
    va_start(args, ignore);
    const char* result = VConcatSep(PATH_SEP, args);
    va_end(args);

#ifdef _WIN32
    TCHAR* fullPathNameBuffer = calloc(4096, 1);
    if (fullPathNameBuffer == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for absolute path: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    DWORD length = GetFullPathName(result, 4096, fullPathNameBuffer, NULL);
    if (length == 0)
    {
        fprintf(stderr, "Failed to get absolute path: %lu\n", GetLastError());
        exit(EXIT_FAILURE);
    }
    if (length > 4096)
    {
        fullPathNameBuffer = calloc(length + 1, 1);
        if (fullPathNameBuffer == NULL)
        {
            fprintf(stderr, "Failed to allocate memory for absolute path: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        length = GetFullPathName(result, length, fullPathNameBuffer, NULL);
        if (length == 0)
        {
            fprintf(stderr, "Failed to get absolute path: %lu\n", GetLastError());
            exit(EXIT_FAILURE);
        }
    }
    return fullPathNameBuffer;
#else
#endif
}

#define ABS_PATH(...) AbsPathImpl(69, __VA_ARGS__, NULL)

bool MkdirCross(const char* path, const int mode)
{
#ifdef _WIN32
    (void)mode;
    return CreateDirectory(path, NULL) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
#else
    return mkdir(path, mode) >= 0 || errno == EEXIST;
#endif
}

void MkdirsImpl(int ignore, ...)
{
    size_t length = 0;
    size_t sepsCount = 0;

    va_list args;
    FOREACH_VARGS(ignore, arg, args, {
        length += strlen(arg);
        sepsCount += 1;
    });
    sepsCount -= 1;

    assert(length > 0);
    char* result = calloc(length + sepsCount * PATH_SEP_LEN + 1, 1);
    if (result == NULL)
    {
        return;
    }
    length = 0;

    FOREACH_VARGS(ignore, arg, args, {
        size_t n = strlen(arg);
        memcpy(result + length, arg, n);
        length += n;

        if (sepsCount > 0)
        {
            memcpy(result + length, PATH_SEP, PATH_SEP_LEN);
            length += PATH_SEP_LEN;
            sepsCount -= 1;
        }

        if (!MkdirCross(result, 0755))
        {
            fprintf(stderr, "[ERROR] could not create directory %s\n", result);
            exit(1);
        }
    });
}

#define MKDIRS(...) MkdirsImpl(69, __VA_ARGS__, NULL)

const char* ConcatImpl(int ignore, ...)
{
    size_t length = 0;

    va_list args;
    FOREACH_VARGS(ignore, arg, args, { length += strlen(arg); });

    assert(length > 0);
    char* result = calloc(length + 1, 1);
    if (result == NULL)
    {
        return NULL;
    }

    length = 0;
    FOREACH_VARGS(ignore, arg, args, {
        size_t n = strlen(arg);
        memcpy(result + length, arg, n);
        length += n;
    });

    return result;
}

#define CONCAT(...) ConcatImpl(69, __VA_ARGS__, NULL)

#ifdef _WIN32
void Execute(char* commandLine)
{
    STARTUPINFO startupInfo = {
        .cb = sizeof(STARTUPINFO),
        .lpReserved = NULL,
        .lpDesktop = NULL,
        .lpTitle = NULL,
        .dwFlags = 0,
        .cbReserved2 = 0,
        .lpReserved2 = NULL,
    };
    PROCESS_INFORMATION processInfo = {0};
    BOOL createProcessResult =
        CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo);
    if (createProcessResult == 0)
    {
        fprintf(stderr, "[ERROR] could not execute command %s\n", commandLine);
        fprintf(stderr, "[ERROR] code %lu\n", GetLastError());
        exit(1);
    }
    WaitForSingleObject(processInfo.hProcess, INFINITE);
    DWORD exitCode = 0;
    if (GetExitCodeProcess(processInfo.hProcess, &exitCode) == 0)
    {
        fprintf(stderr, "[ERROR] could not get exit code for command %s\n", commandLine);
        fprintf(stderr, "[ERROR] code %lu\n", GetLastError());
        exit(1);
    }
    if (exitCode != 0)
    {
        fprintf(stderr, "[WARN] command %s failed with exit code %lu\n", commandLine, exitCode);
    }
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
}
#endif

void CmdImpl(int ignore, ...)
{
#ifdef _WIN32
    size_t length = 0;
    size_t seps = 0;
    va_list args;
    FOREACH_VARGS(ignore, arg, args, {
        length += strlen(arg);
        seps += 1;
    });
    char* commandLine = calloc(length + seps + 3, 1);
    if (commandLine == NULL)
    {
        fprintf(stderr, "[ERROR] could not allocate memory for command line\n");
        exit(1);
    }
    size_t cursor = 0;
    bool first = true;
    FOREACH_VARGS(ignore, arg, args, {
        if (first)
        {
            commandLine[cursor] = '"';
            cursor += 1;
        }
        size_t n = strlen(arg);
        memcpy(commandLine + cursor, arg, n);
        cursor += n;
        if (first)
        {
            commandLine[cursor] = '"';
            cursor += 1;
        }
        commandLine[cursor] = ' ';
        cursor += 1;
        first = false;
    });

    Execute(commandLine);
#else
    size_t argc = 0;
    va_list args;
    FOREACH_VARGS(ignore, arg, args, { argc += 1; });

    char** argv = calloc(argc + 1, sizeof(char*));

    argc = 0;
    FOREACH_VARGS(ignore, arg, args, {
        size_t n = strlen(arg);
        argv[argc] = calloc(n + 1, 1);
        memcpy(argv[argc], arg, n);
        argc += 1;
    });

    pid_t cpid = fork();
    if (cpid == 0)
    {
        execvp(argv[0], argv);
        fprintf(stderr, "[ERROR] could not execute command %s\n", argv[0]);
        perror("execvp");
        exit(1);
    }
    else if (cpid < 0)
    {
        fprintf(stderr, "[ERROR] could not fork a child process\n");
        perror("fork");
        exit(1);
    }

    wait(NULL);
#endif
}

#define CMD(...)                                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        printf("[CMD] %s\n", CONCAT_SEP(" ", __VA_ARGS__));                                                            \
        CmdImpl(69, __VA_ARGS__, NULL);                                                                                \
    } while (0)

void ArrayCmd(const char* const* args)
{
#ifdef _WIN32
    size_t length = 0;
    size_t seps = 0;
    for (size_t i = 0; args[i] != NULL; i += 1)
    {
        length += strlen(args[i]);
        seps += 1;
    }
    char* commandLine = calloc(length + seps + 3, 1);
    if (commandLine == NULL)
    {
        fprintf(stderr, "[ERROR] could not allocate memory for command line\n");
        exit(1);
    }
    size_t cursor = 0;
    bool first = true;
    for (size_t i = 0; args[i] != NULL; i += 1)
    {
        if (first)
        {
            commandLine[cursor] = '"';
            cursor += 1;
        }
        size_t n = strlen(args[i]);
        memcpy(commandLine + cursor, args[i], n);
        cursor += n;
        if (first)
        {
            commandLine[cursor] = '"';
            cursor += 1;
        }
        commandLine[cursor] = ' ';
        cursor += 1;
        first = false;
    }
    printf("[CMD] %s\n", commandLine);
    Execute(commandLine);
#else
    pid_t cpid = fork();
    if (cpid == 0)
    {
        execvp(args[0], (char* const*)args);
        fprintf(stderr, "[ERROR] could not execute command %s\n", args[0]);
        perror("execvp");
        exit(1);
    }
    else if (cpid < 0)
    {
        fprintf(stderr, "[ERROR] could not fork a child process\n");
        perror("fork");
        exit(1);
    }

    wait(NULL);
#endif
}

const char* FindLastOf(const char* str, const char* chars)
{
    const char* last = NULL;
    for (size_t i = 0; str[i] != '\0'; i += 1)
    {
        for (size_t j = 0; chars[j] != '\0'; j += 1)
        {
            if (str[i] == chars[j])
            {
                last = str + i;
            }
        }
    }
    return last;
}

bool HasExtension(const char* path, const char* ext)
{
    const char* extension = strrchr(path, '.');
    if (extension == NULL)
    {
        return false;
    }
    return strcmp(extension, ext) == 0;
}

const char* RemoveExtension(const char* path)
{
    const char* extension = strrchr(path, '.');
    if (extension == NULL || extension == path)
    {
        return path;
    }
    const char* slash = FindLastOf(path, "/\\");
    if (slash != NULL && slash > extension)
    {
        return path;
    }
    char* result = calloc(extension - path + 1, 1);
    if (result == NULL)
    {
        fprintf(stderr, "[ERROR] could not allocate memory for modified path\n");
        exit(1);
    }

    memcpy(result, path, extension - path);
    return result;
}

size_t NullTerminatedArraySize(const char* const* array)
{
    size_t size = 0;
    for (size_t i = 0; array[i] != NULL; i += 1)
    {
        size += 1;
    }
    return size;
}

const char* const* ConstructArgsArray(const char* format, ...)
{
    size_t argc = 0;

    va_list args;
    va_start(args, format);
    for (size_t i = 0; format[i] != '\0'; i += 1)
    {
        switch (format[i])
        {
            case '1':
                argc += 1;
                (void)va_arg(args, const char*);
                break;
            case '*':
                argc += NullTerminatedArraySize(va_arg(args, const char* const*));
                break;
            default:
                assert(false && "illegal format string character");
                break;
        }
    }
    va_end(args);

    const char** result = calloc(argc + 1, sizeof(const char*));
    va_start(args, format);

    size_t cursor = 0;
    for (size_t i = 0; format[i] != '\0'; i += 1)
    {
        switch (format[i])
        {
            case '1':
                result[cursor] = va_arg(args, const char*);
                cursor += 1;
                break;
            case '*': {
                const char* const* array = va_arg(args, const char* const*);
                for (size_t j = 0; array[j] != NULL; j += 1)
                {
                    result[cursor] = array[j];
                    cursor += 1;
                }
                break;
            }
        }
    }

    return result;
}

#define ARRAY_CMD(format, ...) ArrayCmd(ConstructArgsArray(format, __VA_ARGS__))
