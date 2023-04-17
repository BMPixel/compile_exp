CC = gcc
FLEX = flex
BISON = bison
LDFLAGS = 
LIBS = 

SRCDIR = ./src
LIBDIR = ./lib
BINDIR = ./bin
TESTDIR = ./testcases

.PHONY: all clean

all: $(BINDIR)/grammar

$(BINDIR)/grammar: $(LIBDIR)/syntax.tab.c $(LIBDIR)/lex.yy.c
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

$(LIBDIR)/syntax.tab.c: $(SRCDIR)/syntax.y
	$(BISON) -d -t --defines=$(LIBDIR)/syntax.tab.h $< -o $@

$(LIBDIR)/lex.yy.c: $(SRCDIR)/lexical.l
	$(FLEX) -o $@ $<

$(LIBDIR)/ast.h: $(SRCDIR)/ast.h
	cp $< $@

test: $(BINDIR)/grammar
	$(BINDIR)/grammar $(TESTDIR)/testcase_1

clean:
	rm -f $(LIBDIR)/syntax.tab.c $(LIBDIR)/syntax.tab.h $(LIBDIR)/lex.yy.c $(BINDIR)/grammar
