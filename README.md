1. -- 错误类型1:变量在使用时未经定义。 `Exp`  OK
2. -- 错误类型2:函数在调用时未经定义。 `Exp`
3. -- 错误类型3:变量出现重复定义，或变量与前面定义过的结构体名字重复。 `Push` OK
4. -- 错误类型4:函数出现重复定义(即同样的函数名出现了不止一次定义)。 `Push` OK
5. -- 错误类型5:赋值号两边的表达式类型不匹配。 `Exp`
6. 错误类型6:赋值号左边出现一个只有右值的表达式。 `Exp`
7. - 错误类型7:操作数类型不匹配或操作数类型与操作符不匹配(例如整型变量与数组变
量相加减，或数组(或结构体)变量与数组(或结构体)变量相加减)。  `Exp`
8. -- 错误类型8:return语句的返回类型与函数定义的返回类型不匹配。 `RETURN`
9. 错误类型9:函数调用时实参与形参的数目或类型不匹配。 `Exp`
10. 错误类型10:对非数组型变量使用“[...]”(数组访问)操作符。 `Exp`
11. -- 错误类型11:对普通变量使用“(...)”或“()”(函数调用)操作符。 `Exp`
12. 错误类型12:数组访问操作符“[...]”中出现非整数(例如a[1.5])。 `Exp` OK
13. -- 错误类型13:对非结构体型变量使用“.”操作符。 `Exp`
14. -- 错误类型14:访问结构体中未定义过的域。 `Exp`
15. -- 错误类型15:结构体中域名重复定义(指同一结构体中)，或在定义时对域进
行初始化(例如struct A { int a = 0; })。 `Struct Vardec` OK
16. -- 错误类型16:结构体的名字与前面定义过的结构体或变量的名字重复。`push` OK
17. -- 错误类型17:直接使用未定义过的结构体来定义变量。 `vardec` OK