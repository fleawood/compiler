from functools import reduce
import sys
import os


def gen_enum(tokens):
    codelist = ['\t_' + token.strip('\n') for token in tokens]
    head = 'enum {\n'
    body = reduce(lambda x, y: x + ',\n' + y, codelist)
    tail = '\n};\n'
    return head + body + tail


def gen_chararray(tokens):
    codelist = ['"' + token.strip('\n') + '"' for token in tokens]
    head = ('static char *%s[] = {\n' % arrayname)
    body = reduce(lambda x, y: x + ', ' + y, codelist)
    tail = '\n};\n'
    return head + body + tail


def gen_head():
    code = "#ifndef _token_h_\n"
    code += "#define _token_h_\n\n"
    return code


def gen_tail():
    code = "\n#endif"
    return code


def gen_macro():
    code = ("#define token_name(token) %s[token]\n" % arrayname)
    return code


arrayname = '_token_name'
tokens = []
d = sys.path[0]
with open(os.path.join(d, 'token.conf'), 'r') as f:
    tokens = f.readlines()
head = gen_head()
enum = gen_enum(tokens)
chararray = gen_chararray(tokens)
macro = gen_macro()
tail = gen_tail()
with open(os.path.join(d, 'token.h'), 'w') as f:
    f.write(head + enum + chararray + macro + tail)


