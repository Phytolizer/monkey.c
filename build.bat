@echo OFF
REM This file is automatically generated with: tup generate build.bat
set tup_vardict=%cd%\tup-generate.vardict
cd "src/vec/src"
(clang -Wall -Wextra -Wpedantic -Werror=implicit-function-declaration -fsanitize=address,undefined -std=c99 -g3 -I ../../../inc -c vec.c -o vec.o)
cd "../../sds"
(clang -Wall -Wextra -Wpedantic -Werror=implicit-function-declaration -fsanitize=address,undefined -std=c99 -g3 -I ../../inc -c sds.c -o sds.o)
cd "../monkey_test"
(clang -Wall -Wextra -Wpedantic -Werror=implicit-function-declaration -fsanitize=address,undefined -std=c99 -g3 -I ../../inc -c test_lexer.c -o test_lexer.o)
cd "../libmonkey"
(clang -Wall -Wextra -Wpedantic -Werror=implicit-function-declaration -fsanitize=address,undefined -std=c99 -g3 -I ../../inc -c lexer.c -o lexer.o)
(clang -Wall -Wextra -Wpedantic -Werror=implicit-function-declaration -fsanitize=address,undefined -std=c99 -g3 -I ../../inc -c token.c -o token.o)
cd "../libhash"
(clang -Wall -Wextra -Wpedantic -Werror=implicit-function-declaration -fsanitize=address,undefined -std=c99 -g3 -I ../../inc -c fnv.c -o fnv.o)
(clang -Wall -Wextra -Wpedantic -Werror=implicit-function-declaration -fsanitize=address,undefined -std=c99 -g3 -I ../../inc -c hash.c -o hash.o)
cd "../.."
(llvm-ar rcs libmonkey.a src/libmonkey/lexer.o src/libmonkey/token.o)
(llvm-ar rcs libsds.a src/sds/sds.o)
(llvm-ar rcs libvec.a src/vec/src/vec.o)
(llvm-ar rcs libhash.a src/libhash/fnv.o src/libhash/hash.o)
(clang -fsanitize=address,undefined src/monkey_test/test_lexer.o -o monkey_test.exe libmonkey.a libsds.a libvec.a libhash.a)
