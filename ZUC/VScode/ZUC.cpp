#include <bitset>
// #include <stdint.h>
#include "sboxes.cpp"
namespace ZUC
{
    uint32_t s[16];          // 线性反馈寄存器的16个31比特寄存器单元变量
    uint32_t X0, X1, X2, X3; // 比特重组输出的4个32比特字
    uint32_t R1, R2;         // 非线性函数F的2个32比特记忆单元变量
    uint32_t W;              // 非线性函数F输出的32比特字
    uint32_t W1;             // R1与X1进行模2^32加法运算输出的32比特字
    uint32_t W2;             // R2与X2按比特位逐位异或运算输出的32比特字
    uint32_t *Z;             // 算法每次输出32比特密钥字
    int L;                   // 输出密钥字长度

    // MARK:线性反馈移位寄存器LFSR
    void LFSR_initialisation(uint32_t u)
    {
        const uint32_t MOD = ((1ULL << 31) - 1);
        uint32_t v = ((1ULL << 15) * s[15] +
                      (1ULL << 17) * s[13] +
                      (1ULL << 21) * s[10] +
                      (1ULL << 20) * s[4] +
                      (1 + (1ULL << 8)) * s[0]) %
                     MOD;

        uint32_t s16 = (v + u) % MOD;
        if (s16 == 0)
            s16 = MOD;
        for (int i = 0; i < 15; i++)
            s[i] = s[i + 1];
        s[15] = s16;
    }

    void LFSR_work(void)
    {
        const uint32_t MOD = ((1ULL << 31) - 1);
        uint32_t s16 = ((1ULL << 15) * s[15] +
                        (1ULL << 17) * s[13] +
                        (1ULL << 21) * s[10] +
                        (1ULL << 20) * s[4] +
                        (1 + (1ULL << 8)) * s[0]) %
                       MOD;

        if (s16 == 0)
            s16 = MOD;
        for (int i = 0; i < 15; i++)
            s[i] = s[i + 1];
        s[15] = s16;
    }

    // MARK:比特重组BR
    void bit_reconstruction(void)
    {
        X0 = ((s[15] >> 15) << 16) +
             (s[14] & ((1 << 16) - 1)); // H||L
        X1 = ((s[11] & ((1 << 16) - 1)) << 16) +
             (s[9] >> 15); // L||H
        X2 = ((s[7] & ((1 << 16) - 1)) << 16) +
             (s[5] >> 15); // L||H
        X3 = ((s[2] & ((1 << 16) - 1)) << 16) +
             (s[0] >> 15); // L||H
    }

    // MARK:非线性函数F
    uint32_t L1(uint32_t X)
    {
        return X ^ _rotl(X, 2) ^ _rotl(X, 10) ^ _rotl(X, 18) ^ _rotl(X, 24);
    }
    uint32_t L2(uint32_t X)
    {
        return X ^ _rotl(X, 8) ^ _rotl(X, 14) ^ _rotl(X, 22) ^ _rotl(X, 30);
    }

    uint32_t F(void)
    {
        W = ((X0 ^ R1) + R2) % (1ULL << 32);
        W1 = (R1 + X1) % (1ULL << 32);
        W2 = R2 ^ X2;

        uint32_t W1H = W1 >> 16, W1L = W1 & ((1 << 16) - 1);
        uint32_t W2H = W2 >> 16, W2L = W2 & ((1 << 16) - 1);
        R1 = S(L1((W1L << 16) + W2H));
        R2 = S(L2((W2L << 16) + W1H));

        return W;
    }

    // MARK:密钥装入
    // 将128比特的key和iv载入并处理
    void key_load(uint8_t k[], uint8_t iv[])
    {
        // 长15比特的二进制串常量
        uint32_t d[16] = {
            0x44D7, 0x26BC, 0x626B, 0x135E, 0x5789, 0x35E2, 0x7135, 0x09AF,
            0x4D78, 0x2F13, 0x6BC4, 0x1AF1, 0x5E26, 0x3C4D, 0x789A, 0x47AC};

        // si=ki||di||ivi
        for (int i = 0; i < 16; i++)
            s[i] = ((uint32_t)k[i] << 23) + (d[i] << 8) + iv[i];
    }

    void ZUC_initialisation(uint8_t k[], uint8_t iv[])
    {
        key_load(k, iv);
        R1 = R2 = 0;

        for (int i = 0; i < 32; i++)
        {
            bit_reconstruction();
            W = F();
            LFSR_initialisation(W >> 1);
        }
    }

    void ZUC_work()
    {
        bit_reconstruction();
        F();
        LFSR_work();

        for (int i = 0; i < L; i++)
        {
            bit_reconstruction();
            Z[L] = F() ^ X3;
            LFSR_work();
        }
    }

    void stream_work(char *input, char *output, int len, uint8_t k[], uint8_t iv[])
    {
        // 每4字节即32位为一组
        L = len / 4 + ((len % 4) ? 1 : 0);
        Z = new uint32_t[L];
        ZUC_initialisation(k, iv);
        ZUC_work();

        for (int i = 0; i < L; i++)
            ((uint32_t *)output)[i] = ((uint32_t *)input)[i] ^ Z[i];

        // delete[] Z;
    }
}
