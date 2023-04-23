#include <bits/stdc++.h>
#include "S-box.cpp"
using namespace std;

const int buffer_size = 2048;
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
#undef ci;
};

// 外层轮函数
void outer_round_func(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t *Wi_8_64, int m)
{
#define w Wi_8_64
    inner_round_func(a, b, c, w[0], m);
    inner_round_func(b, c, a, w[1], m);
    inner_round_func(a, b, c, w[2], m);
    inner_round_func(a, b, c, w[3], m);
    inner_round_func(b, c, a, w[4], m);
    inner_round_func(a, b, c, w[5], m);
    inner_round_func(a, b, c, w[6], m);
    inner_round_func(b, c, a, w[7], m);
#undef w
};

// 密钥调度
void key_schedule(uint64_t *Wi_8_64)
{
#define w Wi_8_64
    w[0] -= (w[7]) ^ 0xA5A5A5A5A5A5A5A5ULL;
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

// MARK: 主函数，负责明文分组
void main_func(char *input_buf, char *output_buf, int len)
{
    uint64_t a = 0x0123456789ABCDEFULL;
    uint64_t b = 0xFEDCBA9876543210ULL;
    uint64_t c = 0xF096A5B4C3B2E187ULL;

    // 明文分组，一次处理64字节即512位
    int blen = len / 64;
    for (int i = 0; i < blen; i++)
        tiger_hash(&a, &b, &c, (uint64_t *)input_buf + i * 8);

    memcpy(output_buf, &a, 8);
    memcpy(output_buf + 8, &b, 8);
    memcpy(output_buf + 16, &c, 8);
};

// 预处理相关
// 对明文预处理，补齐为512位的倍数并返回长度
void multi512_func(char *input, int *len)
{
    int mod = *len % 64;
	// 补齐512位方便分组
	memset(input + *len, 0, sizeof(char) * (mod));
	*len += mod;
};

// 输入输出等外围操作
//  input_flag决定是从命令行还是文件读写
void IO_func(IO_Flag IO_flag)
{
    FILE *input_source, *key_source, *output_dest;
    // 决定读写方式
    switch (IO_flag)
    {
    case CLI:
    default:
        input_source = stdin;
        key_source = stdin;
        output_dest = stdout;
        break;
    case file:
        input_source = fopen("input.txt", "rb");
        key_source = fopen("key.txt", "rb");
        output_dest = fopen("output.txt", "wb");
        break;
    }

    // 输入明文
    char input_buf[buffer_size] = {0};
    char output_buf[24] = {0};
    int len;
    if (IO_flag == CLI)
        fgets(input_buf, buffer_size + 1, input_source);
    else // file
        fread(input_buf, sizeof(char), buffer_size, input_source);
    fflush(input_source);
    len = strlen(input_buf);
    if (IO_flag == CLI && input_buf[len - 1] == '\n')
        len--; // 删除命令行输入时多余的回车

    // 对明文预处理，补齐为64位的倍数并返回长度
    multi512_func(input_buf, &len);

    // MARK:主函数入口
    main_func(input_buf, output_buf, len);

    // 输出
    fwrite(output_buf, sizeof(char), 24, output_dest);
};
