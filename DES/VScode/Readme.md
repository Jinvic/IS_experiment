# VSCode配置

`.vscode`为vscode配置文件夹  
编译调试：`F5`  
编译运行：`Ctrl+F5`  
仅编译：`Ctrl+Shift+B`

---
# 参考

因为书上讲的不清楚，还参考了这篇博客[DES算法详细图解](https://blog.csdn.net/qq_40672635/article/details/105384881)（纯原理，无代码），以及[DES英文维基](https://en.wikipedia.org/wiki/Data_Encryption_Standard)，[DES中文维基](https://zh.wikipedia.org/wiki/%E8%B3%87%E6%96%99%E5%8A%A0%E5%AF%86%E6%A8%99%E6%BA%96)  
各个表单数据来源：[DES补充材料](https://zh.wikipedia.org/wiki/DES%E8%A1%A5%E5%85%85%E6%9D%90%E6%96%99)。原表从1开始计数，代码中对所有数组减一，从0开始计数。

---
# 关于数据类型和变量
### 数据类型
大量使用了`uint8_t`,`uint32_t`,`uint64_t`等数据类型，其中u表示unsigned,数字表示位数，_t表示由typedef定义。
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

###变量命名
变量的命名主要来自教材，一部分参考了维基，如初始置换为IP，逆初始置换为FP,选择置换为PC等。
其他一些命名规律：
`_[数字]`：表示数据位数，如轮运算中R有32位则定义为R_32；
`_bin`：表示二进制数组，如a=31,则a_bin={0,0,0,0,1,1,1,1}；
`_table`：表示对应的表，如初始置换表为IP_table；
`_buf`：即buffer缓冲区，用于输入输出数据；

---
# 关于测试用代码 
* 命名空间test内的函数为测试用，**无需阅读**。使用方法为`test::[函数名]`。如：`test::itob_btoi_test();`。
* ~~在正式代码中，存在一些注释为`//DEBUG:`的代码，这些代码是测试时用于阶段性输出关键值以检验正确性的，**无需阅读**。~~ **相关代码基本删除**

***以上代码都可能在项目正式完成后删去。***



---
# 关于密文的可视化处理
在加密时，选择的是直接读取每个字符作为一个字节进行加密。加密完成后，每个字节却不一定对应可显示的字符。
为了解决这个问题，选择将每个8位字节拆成两个4位，再映射到可显示的95个ASCII字符之一。即原密文的一个字节将由最终密文的两个字节表示。具体映射如下：

>第一个字符：前四位取值+64，值域为64-79，对应字符@,A-O。
>第二个字符：后四位取值+80，值域为80-95，对应字符P-Z，[，\，]，^。

因此，在解密时需要先将每两个字符压缩为一个，再按DES算法解密。
也因此，**由本程序加密的内容不能由其他同为DES算法的程序解密**，因为对密文进行了额外处理。

---
#待办事项
* ~~文件输入输出（已完成）~~
* ~~ECB模式（已完成）~~
* ~~密文可视化（已完成）~~
* 命令行界面交互设计
* CBC模式
* CTR模式
* Qt图形化界面
* 3-DES适配












