我现在向你介绍一个用于实验的简化语言：C--。C--是C语言的一个子集，它的语法和C语言的语法非常相似，但是它的语义和C语言的语义有很大的不同。C--的语法和C语言的语法相似的地方有：
1) 假设1:整型(int)变量不能与浮点型(float)变量相互赋值或者相互运算。
2) 假设2:仅有int型变量才能进行逻辑运算或者作为if和while语句的条件;仅有int型和 float型变量才能参与算术运算。
3) 假设3:任何函数只进行一次定义，无法进行函数声明。
4) 假设4:所有变量(包括函数的形参)的作用域都是全局的，即程序中所有变量均不能
重名。
5) 假设5:结构体间的类型等价机制采用名等价(Name Equivalence)的方式。
6) 假设6:函数无法进行嵌套定义。
7) 假设7:结构体中的域不与变量重名，并且不同结构体中的域互不重名。
比如下面是一段C--代码：
int main()
{
	int i, a[2][10];
	i = 0;
	while (i < 10)
	{
		a[0][i] = i;
		i = i + 1;
	}
}
这段C--代码的语法树如下
Program (1)
  ExtDefList (1)
    ExtDef (1)
      Specifier (1)
        TYPE: int
      FunDec (1)
        ID: main
        LP
        RP
      CompSt (2)
        LC
        DefList (3)
          Def (3)
            Specifier (3)
              TYPE: int
            DecList (3)
              Dec (3)
                VarDec (3)
                  ID: i
              COMMA
              DecList (3)
                Dec (3)
                  VarDec (3)
                    VarDec (3)
                      VarDec (3)
                        ID: a
                      LB
                      RB
                    LB
                    RB
            SEMI
        StmtList (4)
          Stmt (4)
            Exp (4)
              Exp (4)
                ID: i
              ASSIGNOP
              Exp (4)
                INT: 0
            SEMI
          StmtList (5)
            Stmt (5)
              WHILE
              LP
              Exp (5)
                Exp (5)
                  ID: i
                RELOP: <
                Exp (5)
                  INT: 10
              RP
              Stmt (6)
                CompSt (6)
                  LC
                  StmtList (7)
                    Stmt (7)
                      Exp (7)
                        Exp (7)
                          Exp (7)
                            Exp (7)
                              ID: a
                            LB
                            Exp (7)
                              INT: 0
                            RB
                          LB
                          Exp (7)
                            ID: i
                          RB
                        ASSIGNOP
                        Exp (7)
                          ID: i
                      SEMI
                    StmtList (8)
                      Stmt (8)
                        Exp (8)
                          Exp (8)
                            ID: i
                          ASSIGNOP
                          Exp (8)
                            Exp (8)
                              ID: i
                            PLUS
                            Exp (8)
                              INT: 1
                        SEMI
                  RC
        RC
语法树的每一个节点定义如下：
typedef struct Node {
    char *name;
    int lineno;
    char *value_string;
    int is_terminal;
    struct Node *first_child;
    struct Node *next;
} Node;
我现在已经可以根据语法生成语法树了，但是我现在需要根据它完成语义分析和类型检查，并打印分析结果。我现在需要你来完成这个程序。这个工作可能比较复杂，我希望你首先向我展示你的思路：包括：
- 如何构造符号表
- 如何为每个节点添加类型信息等