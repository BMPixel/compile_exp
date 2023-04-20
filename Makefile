CC = g++
FLEX = flex
BISON = bison
LDFLAGS = -Wno-write-strings
LIBS = 

SRCDIR = ./src
BINDIR = ./bin
TESTDIR = ./testcases

.PHONY: all clean

all: $(BINDIR)/grammar

$(BINDIR)/grammar: $(SRCDIR)/syntax.tab.c $(SRCDIR)/lex.yy.c
	$(CC) -g -o $@ $(SRCDIR)/syntax.tab.c $(SRCDIR)/lex.yy.c $(SRCDIR)/analysis.cpp $(LDFLAGS) $(LIBS)

$(BINDIR)/grammar-debug: $(SRCDIR)/syntax.tab.c $(SRCDIR)/lex.yy.c
	$(CC) -g -DDEBUG -o $@ $(SRCDIR)/syntax.tab.c $(SRCDIR)/lex.yy.c $(SRCDIR)/analysis.cpp $(LDFLAGS) $(LIBS)

$(SRCDIR)/syntax.tab.c: $(SRCDIR)/syntax.y
	$(BISON) -d -t --defines=$(SRCDIR)/syntax.tab.h $< -o $@

$(SRCDIR)/lex.yy.c: $(SRCDIR)/lexical.l
	$(FLEX) -o $@ $<

testcase1: $(BINDIR)/grammar
	$(BINDIR)/grammar $(TESTDIR)/testcase1

testcase2: $(BINDIR)/grammar
	$(BINDIR)/grammar $(TESTDIR)/testcase2

testcase3: $(BINDIR)/grammar
	$(BINDIR)/grammar $(TESTDIR)/testcase3

testcase4: $(BINDIR)/grammar
	$(BINDIR)/grammar $(TESTDIR)/testcase4

testcase5: $(BINDIR)/grammar
	$(BINDIR)/grammar $(TESTDIR)/testcase5

testcase6: $(BINDIR)/grammar
	$(BINDIR)/grammar $(TESTDIR)/testcase6

debug: $(BINDIR)/grammar-debug
	$(BINDIR)/grammar-debug $(TESTDIR)/testdebug.c

clean: 
	rm -f $(SRCDIR)/syntax.tab.c $(SRCDIR)/syntax.tab.h $(SRCDIR)/lex.yy.c $(BINDIR)/grammar	