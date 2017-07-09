CC = gcc
LEX = flex
BISON = bison
CFLAGS = -std=gnu99 -g -MMD
PYTHON = python3

SRCDIR = src
OBJDIR = obj
CFILES = $(shell find $(SRCDIR) -name "*.c")
OBJS = $(patsubst $(SRCDIR)%.c, $(OBJDIR)%.o, $(CFILES))
LFILE = $(shell find $(SRCDIR) -name "*.l")
YFILE = $(shell find $(SRCDIR) -name "*.y")
LFC = $(shell find $(SRCDIR) -name "*.l" | sed s/[^/]*\\.l/lex.yy.c/)
YFC = $(shell find $(SRCDIR) -name "*.y" | sed s/[^/]*\\.y/syntax.tab.c/)
LFO = $(patsubst $(SRCDIR)%.c, $(OBJDIR)%.o, $(LFC))
YFO = $(patsubst $(SRCDIR)%.c, $(OBJDIR)%.o, $(YFC))
PYFILE = $(shell find $(SRCDIR) -name "*.py")
PYCONF = $(shell find $(SRCDIR) -name "*.conf")
PYCACHEDIR = $(shell find $(SRCDIR) -name "__pycache__")
TOKEN_H = $(shell find $(SRCDIR) -name "token.h")

parser: $(YFO) $(filter-out $(LFO), $(OBJS))
	$(CC) -o $@ $(filter-out $(LFO), $(OBJS)) -lfl -ly
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	$(CC) -o $@ $< $(CFLAGS) -c
$(YFO): $(LFC) $(YFC)
	mkdir -p $(OBJDIR)
	$(CC) -c $(YFC) -o $@
$(LFC): $(TOKEN_H) $(LFILE)
	$(LEX) -o $@ $(LFILE)
$(YFC): $(YFILE)
	$(BISON) -o $@ -d -v -t $<
$(TOKEN_H): $(PYFILE) $(PYCONF)
	$(PYTHON) $<
-include $(patsubst %.o, %.d, $(OBJS))

.PHONY: clean test testlab2
test:
	./parser test.cmm test.s
testlab2:
	@for t in $(shell find tests/lab2 -name "*.cmm"); \
	do \
		echo parsing "$$t"; \
		./parser $$t; \
		echo; \
	done
clean:
	rm -f parser
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -rf $(OBJDIR)
	rm -f $(LFC) $(YFC) $(YFC:.c=.h) $(YFILE:.y=.output)
	rm -rf $(PYCACHEDIR)
	rm -f test.cmm test.ir test.s
	rm -f *~
