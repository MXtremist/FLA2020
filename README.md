# NJU FLA2020 实验部分

## 编译说明

1. 进入目录`turing-project/`

2. 输入命令`make`，生成可执行文件`turing`

3. 运行，可以提前额外输入命令`export PATH=.:$PATH`将当前路径加入环境变量，这样在运行时就可以不输入`./turing`而直接使用`turing`

4. 输入的文件名不需要加上路径，文件要求处于`programs/`文件夹内

5. 一个命令的示例为：

   ````shell
   ./turing -v case1.tm 101 
   ````

6. 结束运行，输入命令`make clean`删除可执行文件`turing`

