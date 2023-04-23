#include <bits/stdc++.h>
#include "S-box.cpp"
using namespace std;

//TODO:
void multi512_func(){};

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

void tiger_hash(uint64_t *block_512)
{
    uint64_t a = 0x0123456789ABCDEFULL;
    uint64_t b = 0xFEDCBA9876543210ULL;
    uint64_t c = 0xF096A5B4C3B2E187ULL;
    uint64_t aa = a, bb = b, cc = c;

    uint64_t Wi_8_64[8];
    memcpy(Wi_8_64, block_512, 64);

    outer_round_func(&aa, &bb, &cc, Wi_8_64, 5);
    key_schedule(Wi_8_64);
    outer_round_func(&cc, &aa, &bb, Wi_8_64, 7);
    key_schedule(Wi_8_64);
    outer_round_func(&aa, &bb, &cc, Wi_8_64, 9);

    a ^= aa;
    b -= bb;
    c += cc;
};