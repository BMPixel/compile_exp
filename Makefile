CC = g++
FLEX = flex
BISON = bison
LDFLAGS = -Wno-write-strings
LIBS = 

SRCDIR = ./src
BINDIR = ./bin
TESTDIR = ./testcases

.PHONY: all clean

all: parser

parser: $(SRCDIR)/syntax.tab.c $(SRCDIR)/lex.yy.c
	$(CC) -g -o $@ $(SRCDIR)/syntax.tab.c $(SRCDIR)/lex.yy.c $(SRCDIR)/analysis.cpp $(LDFLAGS) $(LIBS)

parser-debug: $(SRCDIR)/syntax.tab.c $(SRCDIR)/lex.yy.c
	$(CC) -g -DDEBUG -o $@ $(SRCDIR)/syntax.tab.c $(SRCDIR)/lex.yy.c $(SRCDIR)/analysis.cpp $(LDFLAGS) $(LIBS)

$(SRCDIR)/syntax.tab.c: $(SRCDIR)/syntax.y
	$(BISON) -d -t --defines=$(SRCDIR)/syntax.tab.h $< -o $@

$(SRCDIR)/lex.yy.c: $(SRCDIR)/lexical.l
	$(FLEX) -o $@ $<

testcase1: parser
	parser $(TESTDIR)/testcase1

testcase2: parser
	parser $(TESTDIR)/testcase2

testcase3: parser
	parser $(TESTDIR)/testcase3

testcase4: parser
	parser $(TESTDIR)/testcase4

testcase5: parser
	parser $(TESTDIR)/testcase5

testcase6: parser
	parser $(TESTDIR)/testcase6

debug: parser-debug
	parser-debug $(TESTDIR)/testdebug.c

debug-build-only: parser-debug

clean: 
	rm -f $(SRCDIR)/syntax.tab.c $(SRCDIR)/syntax.tab.h $(SRCDIR)/lex.yy.c parser	