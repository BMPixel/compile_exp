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

$(SRCDIR)/syntax.tab.c: $(SRCDIR)/syntax.y
	$(BISON) -d -t --defines=$(SRCDIR)/syntax.tab.h $< -o $@

$(SRCDIR)/lex.yy.c: $(SRCDIR)/lexical.l
	$(FLEX) -o $@ $<

test: $(BINDIR)/grammar
	$(BINDIR)/grammar $(TESTDIR)/testcase_4

clean: 
	rm -f $(SRCDIR)/syntax.tab.c $(SRCDIR)/syntax.tab.h $(SRCDIR)/lex.yy.c $(BINDIR)/grammar	