#include <bits/stdc++.h>
#include <windows.h>
#include "S-box.cpp"
using namespace std;

const int buffer_size = 2048;
char to_hex[] = "0123456789ABCDEF";
enum IO_Flag
{
    CLI,
    file
};

// Tiger Hash算法相关
// 内层轮函数
void inner_round_func(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t Wi_64, int m)
{
    uint8_t ci_8_8[8];
    memcpy(ci_8_8, c, 8);
#define ci ci_8_8
    *c ^= Wi_64;
    *a -= S_box[0][ci[0]] ^ S_box[1][ci[2]] ^ S_box[2][ci[4]] ^ S_box[3][ci[6]];
    *b -= S_box[3][ci[1]] ^ S_box[2][ci[3]] ^ S_box[0][ci[5]] ^ S_box[0][ci[7]];
    *b *= m;
#undef ci
};

// 外层轮函数
void outer_round_func(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t *Wi_8_64, int m)
{
#define w Wi_8_64
    inner_round_func(a, b, c, w[0], m);
    inner_round_func(b, c, a, w[1], m);
    inner_round_func(c, a, b, w[2], m);
    inner_round_func(a, b, c, w[3], m);
    inner_round_func(b, c, a, w[4], m);
    inner_round_func(c, a, b, w[5], m);
    inner_round_func(a, b, c, w[6], m);
    inner_round_func(b, c, a, w[7], m);
#undef w
};

// 密钥调度
void key_schedule(uint64_t *Wi_8_64)
{
#define w Wi_8_64
    w[0] -= w[7] ^ 0xA5A5A5A5A5A5A5A5ULL;
    w[1] ^= w[0];
    w[2] += w[1];
    w[3] -= w[2] ^ ((~w[1]) << 19);
    w[4] ^= w[3];
    w[5] += w[4];
    w[6] -= w[5] ^ ((~w[4]) >> 23);
    w[7] ^= w[6];
    w[0] += w[7];
    w[1] -= w[0] ^ ((~w[7]) << 19);
    w[2] ^= w[1];
    w[3] += w[2];
    w[4] -= w[3] ^ ((~w[2]) >> 23);
    w[5] ^= w[4];
    w[6] += w[5];
    w[7] -= w[6] ^ 0x0123456789ABCDEFULL;
#undef w
};

// MARK: tiger_hash算法 处理一个512位分组
void tiger_hash(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t *block_512)
{
    // 复制abc用于轮运算和密钥调度
    uint64_t aa = *a, bb = *b, cc = *c;

    uint64_t Wi_8_64[8];
    memcpy(Wi_8_64, block_512, 64);

    // 外层轮运算
    outer_round_func(&aa, &bb, &cc, Wi_8_64, 5);
    key_schedule(Wi_8_64); // 密钥调度
    outer_round_func(&cc, &aa, &bb, Wi_8_64, 7);
    key_schedule(Wi_8_64); // 密钥调度
    outer_round_func(&aa, &bb, &cc, Wi_8_64, 9);

    *a ^= aa;
    *b -= bb;
    *c += cc;
};

// MARK: 主函数，负责明文分组等
void main_func(char *input_buf, char *output_buf, int len)
{
    uint64_t a = 0x0123456789ABCDEFULL;
    uint64_t b = 0xFEDCBA9876543210ULL;
    uint64_t c = 0xF096A5B4C3B2E187ULL;

    // 明文分组，一次处理64字节即512位
    int blen = len / 64; /* 分组长度 */
    for (int i = 0; i < blen; i++)
        tiger_hash(&a, &b, &c, (uint64_t *)input_buf + i * 8);

    // 输出数据的hex值
    int p = 0;
    uint8_t *ai = (uint8_t *)&a; // 拆成8字节数组
    for (int i = 0; i < 8; i++, p += 2)
    {
        output_buf[p] = to_hex[ai[i] >> 4];     // 前4位hex值
        output_buf[p + 1] = to_hex[ai[i] & 15]; // 后4位hex值
    }
    uint8_t *bi = (uint8_t *)&b; // 拆成8字节数组
    for (int i = 0; i < 8; i++, p += 2)
    {
        output_buf[p] = to_hex[bi[i] >> 4];     // 前4位hex值
        output_buf[p + 1] = to_hex[bi[i] & 15]; // 后4位hex值
    }
    uint8_t *ci = (uint8_t *)&c; // 拆成8字节数组
    for (int i = 0; i < 8; i++, p += 2)
    {
        output_buf[p] = to_hex[ci[i] >> 4];     // 前4位hex值
        output_buf[p + 1] = to_hex[ci[i] & 15]; // 后4位hex值
    }
};

// 预处理相关
// 对明文预处理，补齐为512位的倍数并返回长度
void multi512_func(char *input, int *len)
{
    // 位长度
     uint64_t blen = *len * 8, blen_cpy = blen;

    // 填充1
    const uint8_t pad1 = 1 << 7; /* 因为按字节输入输出一定是8倍数，这里直接填充一个字节而不是一位 */
    memcpy(input + *len, &pad1, 1);
    blen += 8;
    *len += 1;

    // 填充0直到比512位倍数少64位
    int pad0 = (448 - (blen % 512)) % 512; /* 填充0的位数 */
    for (int i = 0; i < pad0 / 8; i++)
        *(input + *len + i) = 0;
    *len += pad0 / 8;

    // 原始消息的位数的64位的二进制整数添加到消息末尾
    memcpy(input + *len, &blen_cpy, 8);
    *len += 8;
};

// 输入输出等外围操作
//  input_flag决定是从命令行还是文件读写
void IO_func(IO_Flag IO_flag)
{
    FILE *input_source, *output_dest;
    // 决定读写方式
    switch (IO_flag)
    {
    case CLI:
    default:
        input_source = stdin;
        output_dest = stdout;
        break;
    case file:
        input_source = fopen("input.txt", "rb");
        output_dest = fopen("output.txt", "wb");
        break;
    }

    // 输入明文
    char input_buf[buffer_size] = {0};
    char output_buf[48] = {0}; // 输出hex值，数组开两倍大小
    int len;
    if (IO_flag == CLI)
        fgets(input_buf, buffer_size + 1, input_source);
    else // file
        fread(input_buf, sizeof(char), buffer_size, input_source);
    fflush(input_source);
    len = strlen(input_buf);
    // 删除命令行输入时多余的回车
    if (IO_flag == CLI && input_buf[len - 1] == '\n')
    {
        input_buf[len - 1] = '\0';
        len--;
    }

    // 对明文预处理，补齐为512位的倍数并返回长度
    multi512_func(input_buf, &len);

    // MARK:主函数入口
    main_func(input_buf, output_buf, len);

    // 输出
    fwrite(output_buf, sizeof(char), 48, output_dest);

    if (IO_flag == file)
    {
        fclose(input_source);
        fclose(output_dest);
    }
};

// 命令行交互界面
void CLI_interact(void)
{
    IO_Flag IO_flag;

    char cmd;         // 输入命令
    bool input_legal; // 输入合法性校验

    // 选择 命令行CLI/文件file 输入输出
    input_legal = false;
    while (!input_legal)
    {
        printf("选择 命令行[C]/文件[F] 输入输出:\n");
        scanf("%c", &cmd);
        fflush(stdin);
        switch (cmd)
        {
        case 'c':
        case 'C':
            IO_flag = CLI;
            input_legal = true;
            break;
        case 'f':
        case 'F':
            IO_flag = file;
            input_legal = true;
            break;
        default:
            printf("输入不合法，请重新输入\n");
            break;
        }
    }

    if (IO_flag == CLI)
    {
        printf("输入文本（仅一行），可以空格，长度不超过%d字符\n", buffer_size);
    }
    else // file
    {
        printf("在input.txt中输入需要处理的文本，长度不超过%d字符\n", buffer_size);
        printf("结果将输出到output.txt中\n");
        printf("完成输入后按任意键继续\n");
        system("pause");
    }
    IO_func(IO_flag);
    printf("\n已完成\n");
}

signed main(void)
{
    // 设置控制台程序输出的代码页编码为utf-8格式以解决中文乱码问题
    SetConsoleOutputCP(65001);

    CLI_interact();
    system("pause");
    return 0;
}
