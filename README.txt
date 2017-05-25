1. 编译
直接make即可
2. 运行
make之后产生可执行文件parser,输入./parser [文件名]，就能对所给文件进行语法分析。另外还提供了一个快捷方式：输入make test，将执行./parser test.cmm。
3. 目录结构
.
├── Makefile
├── parser
├── README.txt
├── src
├── obj
├── test.cmm
└── tests
    ├── lab1
    └── lab2
src目录下为实现代码，tests目录下为从实验说明中复制来的若干测试源文件,obj目录为生成的二进制文件。
