CC = gcc
LEX = flex
BISON = bison
CFLAGS = -std=c99 -g

CFILES = $(shell find ./ -name "*.c")
OBJS = $(CFILES:.c=.o)
LFILE = $(shell find ./ -name "*.l")
YFILE = $(shell find ./ -name "*.y")
LFC = $(shell find ./ -name "*.l" | sed s/[^/]*\\.l/lex.yy.c/)
YFC = $(shell find ./ -name "*.y" | sed s/[^/]*\\.y/syntax.tab.c/)
LFO = $(LFC:.c=.o)
YFO = $(YFC:.c=.o)

parser: syntax $(filter-out $(LFO), $(OBJS))
	$(CC) -o parser $(filter-out $(LFO), $(OBJS)) -lfl -ly
syntax: lexical syntax-c
	$(CC) -c $(YFC) -o $(YFO)
lexical: $(LFILE)
	$(LEX) -o $(LFC) $(LFILE)
syntax-c: $(YFILE)
	$(BISON) -o $(YFC) -d -v -t $(YFILE)
-include $(patsubst %.o, %.d, $(OBJS))

.PHONY: clean test
test:
	./parser test.cmm
clean:
	rm -f parser
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f $(LFC) $(YFC) $(YFC:.c=.h) $(YFC:.c=.output)
	rm -f *~
