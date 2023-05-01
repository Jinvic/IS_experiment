# VSCode配置

`.vscode`为vscode配置文件夹  
编译调试：`F5`  
编译运行：`Ctrl+F5`  
仅编译：`Ctrl+Shift+B`

---
# 参考

相关的资料比想象中还少，好在教材讲得还算清楚。但是S-box的链接失效了，《Tiger：A Fast New Hash Function》是这么说的：

> “The S boxes tl to t4 would take ten pages to publish here, so they will be published electronically along with the full source code, and made available from the authors' world wide web home pages.”

大意为内容过长放在了作者（Eli Biham）网络主页，然而这个[链接](https://www.cs.technion.ac.il/'biham/)已经失效了。
之后又找了找，基本都是指向这个地址。最后找到了另一位作者Ross Anderson的[主页](https://www.cl.cam.ac.uk/~rja14/)，从[这里](https://www.cl.cam.ac.uk/~rja14/Papers/tiger-src.tar.gz)的源码里找到了S-box的数据。虽然[这里](https://www.cl.cam.ac.uk/~rja14/Papers/tigersb.pdf)也给出了S-box的生成方法，但也给出了源码，复制下来跑一遍再拿数据感觉没必要绕这么一圈。

全部的参考内容如下：

* 信息安全原理与实践 第2版 ([美] Mark Stamp 著 张戈 译)
* Information Security Principles and Practice, Second Edition (Mark Stamp(auth.))

这两本是教材，前者是后者的中译本

* [Tiger: A Fast New Hash Function](https://www.cl.cam.ac.uk/~rja14/Papers/tiger.pdf)
* [Generation of the S boxes of Tiger](https://www.cl.cam.ac.uk/~rja14/Papers/tigersb.pdf)

这两篇来自Tiger Hash算法的作者Ross Anderson和Eli Biham，链接指向Ross Anderson的主页。

* [Cryptanalysis of the Tiger Hash Function](https://iacr.org/archive/asiacrypt2007/48330539/48330539.pdf)

查资料时找到的，顺手存一下。

以上除了中译本（文件过大）都可以在[这里](https://github.com/Jinvic/IS_experiment/tree/main/referance)下载。

---
# 关于数据类型和命名规则
### 数据类型
使用了`uint8_t`,`uint32_t`,`uint64_t`等数据类型，其中u表示unsigned,数字表示位数，_t表示由typedef定义。  
这些数据类型在stdint.h中定义，源码如下：
```C
/* 7.18.1.1  Exact-width integer types */
typedef signed char int8_t;
typedef unsigned char   uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef int  int32_t;
typedef unsigned   uint32_t;
__MINGW_EXTENSION typedef long long  int64_t;
__MINGW_EXTENSION typedef unsigned long long   uint64_t;
```
其实就是我们熟悉的char、int之类，使用这些typedef变量是为了方便区别长度。

### 变量命名
变量的命名主要来自教材，一部分参考了[Tiger: A Fast New Hash Function](https://www.cl.cam.ac.uk/~rja14/Papers/tiger.pdf)。  
其他一些命名规律：  
`_[数字]`：表示数据位数，如block_512表示长度为512位的一个分组；  
`_[数字]_[数字]`：表示多维数组，其中前面的数字表示数组长度，后面的数字表示元素位数。如Wi_8_64表示数组长度为8，元素长度为64位的数组；  
`_buf`：即buffer缓冲区，用于输入输出数据；  

---
# 关于测试用代码
~~在正式代码中，存在一些注释为`//DEBUG:`的代码，这些代码是测试时用于阶段性输出关键值以检验正确性的，**无需阅读**~~  
新建了一个文件`Tiger_DEBUG.cpp`来保存含测试代码的内容，**无需阅读**。    
相应的，正式代码中的所有测试用代码都已移除，可以正常阅读。

---
# 关于可处理文本长度
开发环境下默认为2048个字符。如有修改需求，可直接修改`Tiger.cpp`的`buffer_size`常量。  
``` C++
const int buffer_size = 2048;
```

---
# 关于命令行和文件输入输出
在命令行输入输出下，使用`scanf()`进行输入，一次只能输入一行。而且输入数据后会带一个`\n`，需要额外处理。  
而在文件输入输出下，使用`fread()`进行输入，一次可输入多行，并且没有`\n`的问题（只要用户不主动多敲一个回车）。    
综上，**建议使用文件模式进行输入输出**。

<!-- ---
# 关于图形化界面
使用Qt开发，详见[这里](https://github.com/Jinvic/IS_experiment/tree/main/DES/Qt)。 -->

---
# 待办事项
* ~~输入输出（命令行，文件）（已完成）~~
* ~~分组，补齐512位倍数（已完成）~~
* ~~命令行界面交互设计（已完成）~~
* ~~Tiger和Tiger2两种模式兼容（已完成）~~
* ~~Qt图形化界面~~
* 软件测试
