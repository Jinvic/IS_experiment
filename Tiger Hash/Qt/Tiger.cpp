#include "S-box.cpp"
#include <cstdio>
#include <cstring>
namespace TH // Tiger Hash
{
// Tiger Hash算法相关
enum Tiger_Mode
{
    Tiger,
    Tiger2
};
Tiger_Mode tiger_mode = Tiger2;

// 内层轮函数
void inner_round_func(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t Wi_64, int m)
{
    uint8_t ci_8_8[8];
    memcpy(ci_8_8, c, 8);

#define ci ci_8_8
    *c ^= Wi_64;
    *a -= S_box[0][ci[0]] ^ S_box[1][ci[2]] ^ S_box[2][ci[4]] ^ S_box[3][ci[6]];
    *b -= S_box[3][ci[1]] ^ S_box[2][ci[3]] ^ S_box[1][ci[5]] ^ S_box[0][ci[7]];
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
    sprintf(output_buf, "%08X%08X", (uint32_t)(a >> 32), (uint32_t)a);
    sprintf(output_buf + 16, "%08X%08X", (uint32_t)(b >> 32), (uint32_t)c);
    sprintf(output_buf + 32, "%08X%08X", (uint32_t)(b >> 32), (uint32_t)c);
};

// 预处理相关
// 对明文预处理，补齐为512位的倍数并返回长度
void multi512_func(char *input, int *len)
{
    // 位长度
    uint64_t blen = *len * 8, blen_cpy = blen;

    // 填充1 因为按字节输入输出一定是8倍数，这里直接填充一个字节而不是一位
    if (tiger_mode == Tiger)
        input[*len] = 0x01;
    else // Tiger2
        input[*len] = 0x80;
    blen += 8;
    *len += 1;

    // 填充0直到比512位倍数少64位
    int pad0 = (448 - (blen % 512)) % 512; /* 填充0的位数 */
    for (int i = 0; i < pad0 / 8; i++)
        input[*len + i] = 0;
    *len += pad0 / 8;

    // 原始消息的位数的64位的二进制整数添加到消息末尾
    memcpy(input + *len, &blen_cpy, 8);
    *len += 8;
};
}
