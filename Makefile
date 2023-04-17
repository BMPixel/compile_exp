CC = g++
FLEX = flex
BISON = bison
LDFLAGS = -Wno-write-strings
LIBS = 

SRCDIR = ./src
LIBDIR = ./lib
BINDIR = ./bin
TESTDIR = ./testcases

.PHONY: all clean

all: $(BINDIR)/grammar

$(BINDIR)/grammar: $(LIBDIR)/syntax.tab.c $(LIBDIR)/lex.yy.c $(LIBDIR)/ast.h $(LIBDIR)/analysis.h $(LIBDIR)/analysis.cpp
	$(CC) -o $@ $(LIBDIR)/syntax.tab.c $(LIBDIR)/lex.yy.c $(LIBDIR)/analysis.cpp $(LDFLAGS) $(LIBS)

$(LIBDIR)/syntax.tab.c: $(SRCDIR)/syntax.y
	$(BISON) -d -t --defines=$(LIBDIR)/syntax.tab.h $< -o $@

$(LIBDIR)/lex.yy.c: $(SRCDIR)/lexical.l
	$(FLEX) -o $@ $<

$(LIBDIR)/ast.h: $(SRCDIR)/ast.h
	cp $< $@

$(LIBDIR)/analysis.h: $(SRCDIR)/analysis.h
	cp $< $@

$(LIBDIR)/analysis.cpp: $(SRCDIR)/analysis.cpp
	cp $< $@

test: $(BINDIR)/grammar
	$(BINDIR)/grammar $(TESTDIR)/testcase_4

clean:
	rm -f $(LIBDIR)/syntax.tab.c $(LIBDIR)/syntax.tab.h $(LIBDIR)/lex.yy.c $(BINDIR)/grammar
