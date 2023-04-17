cp ./src/ast.h ./lib/ast.h
bison -d -t --defines=./lib/syntax.tab.h ./src/syntax.y -o ./lib/syntax.tab.c 
flex -o ./lib/lex.yy.c ./src/lexical.l 
gcc -o ./bin/grammar ./lib/syntax.tab.c ./lib/lex.yy.c 
./bin/grammar ./testcases/testcase_1