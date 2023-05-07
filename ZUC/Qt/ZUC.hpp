/**
 * @author Ricardoly
 * @date 2023-05-06
 * @file ZUC.hpp
 */

#include <vector>
#include <bitset>
#include <string>
#include <iostream>

#include "BIT.hpp"

namespace ZUC
{
bit31 s[17];  // 31bit
bit32 X[4];   // 32bit
bit32 R1, R2; // 32bit

void LFSRWithInitialisationMode(bit31 u) // u 31bit
{
    // (1)
    u64 tmp1 = bit[15] * s[15].to_ullong();
    u64 tmp2 = bit[17] * s[13].to_ullong();
    u64 tmp3 = bit[21] * s[10].to_ullong();
    u64 tmp4 = bit[20] * s[4].to_ullong();
    u64 tmp5 = (bit[8] + 1) * s[0].to_ullong();
    u64 v = (tmp1 + tmp2 + tmp3 + tmp4 + tmp5) % (bit[31] - 1);

    // (2)
    s[16] = (u.to_ullong() + v) % (bit[31] - 1);

    // (3)
    if (s[16] == 0)
        s[16] = (bit[31] - 1);

    // (4)
    for (int i = 0; i < 16; ++i)
    {
        s[i] = s[i + 1];
    }
}

void LFSRWithWorkMode()
{
    // (1)
    u64 tmp1 = bit[15] * s[15].to_ullong();
    u64 tmp2 = bit[17] * s[13].to_ullong();
    u64 tmp3 = bit[21] * s[10].to_ullong();
    u64 tmp4 = bit[20] * s[4].to_ullong();
    u64 tmp5 = (bit[8] + 1) * s[0].to_ullong();
    s[16] = (tmp1 + tmp2 + tmp3 + tmp4 + tmp5) % (bit[31] - 1);

    // (2)
    if (s[16] == 0)
        s[16] = (bit[31] - 1);

    // (3)
    for (int i = 0; i < 16; ++i)
    {
        s[i] = s[i + 1];
    }
}

void BitReconstruction()
{
    // (1)
    X[0] = (BIT::H(s[15]) << 16) | BIT::L(s[14]);

    // (2)
    X[1] = (BIT::L(s[11]) << 16) | BIT::H(s[9]);

    // (3)
    X[2] = (BIT::L(s[7]) << 16) | BIT::H(s[5]);

    // (4)
    X[3] = (BIT::L(s[2]) << 16) | BIT::H(s[0]);
}

bit32 F(bit32 x0, bit32 x1, bit32 x2)
{
    // (1)
    bit32 W = ((x0 ^ R1).to_ullong() + R2.to_ullong()) % bit[32];

    // (2)
    bit32 W1 = (R1.to_ullong() + x1.to_ullong()) % bit[32];

    // (3)
    bit32 W2 = R2 ^ x2;

    // (4)
    bit32 tmp1 = (BIT::L(W1) << 16) | BIT::H(W2);
    tmp1 = BIT::L1(tmp1);
    R1 = BIT::S(tmp1);

    // (5)
    bit32 tmp2 = (BIT::L(W2) << 16) | BIT::H(W1);
    tmp2 = BIT::L2(tmp2);
    R2 = BIT::S(tmp2);

    return W;
}

void MountTheKey(bit128 k, bit128 iv)
{
    int cnt = 0;
    int idx = 0;
    u64 ki = 0, ivi = 0;
    for (int i = 127; i >= 0; --i)
    {
        if (k[i] == 1)
            ki += bit[7 - cnt];
        if (iv[i] == 1)
            ivi += bit[7 - cnt];
        cnt += 1;
        if (cnt == 8)
        {
            cnt = 0;
            s[idx] = (ki << 23) + (d[idx] << 8) + ivi;
            idx += 1;
            ki = ivi = 0;
        }
    }
}

std::vector<bit32> ZUC_init(bit128 k, bit128 iv)
{
    // a
    MountTheKey(k, iv);

    // b
    R1 = R2 = 0;

    // c
    std::vector<bit32> ans;
    for (int i = 0; i <= 31; ++i)
    {
        BitReconstruction();
        bit32 W = F(X[0], X[1], X[2]);
        ans.emplace_back(W);
        LFSRWithInitialisationMode((W >> 1).to_ullong());
    }
    return ans;
}

std::vector<bit32> ZUC_work(int L)
{
    // a
    BitReconstruction();
    F(X[0], X[1], X[2]);
    LFSRWithWorkMode();

    // b
    std::vector<bit32> ans;
    for (int i = 0; i < L; ++i)
    {
        BitReconstruction();
        bit32 Z = F(X[0], X[1], X[2]) ^ X[3];
        ans.emplace_back(Z);
        LFSRWithWorkMode();
    }
    return ans;
}

/**
 * @brief genertae OBS(输出比特流)
 *
 * @param CK 保密性算法密钥 128bit
 * @param IV 初始向量 128bit
 * @param LENGTH 明文消息流的比特长度 32bit
 * @param IBS 输入比特流
 * @return std::string OBS
 */
std::string ZUCEncrypt(bit128 CK, bit128 IV, int LENGTH, std::string IBS)
{
    int L = (LENGTH + 31) / 32;
    ZUC_init(CK, IV);
    auto key = ZUC_work(L);

    std::string KBS = "";
    for (int i = 0; i < L; ++i)
    {
        KBS += key[i].to_string();
    }

    std::string OBS = "";
    int tmp1, tmp2;
    std::string tmp;
    for (int i = 0; i < LENGTH; ++i)
    {
        tmp1 = IBS[i] - '0';
        tmp2 = KBS[i] - '0';
        tmp = (tmp1 ^ tmp2) ? "1" : "0";
        OBS +=  tmp;
    }
    return OBS;
}

void testMount(bit128 CK, bit128 IV)
{
    MountTheKey(CK, IV);
    for (int i = 0; i < 16; ++i)
    {
        std::cout << std::hex << s[i].to_ulong() << '\n';
    }
}
void testInit(bit128 CK, bit128 IV)
{
    ZUC_init(CK, IV);
    for (int i = 0; i < 16; ++i)
    {
        std::cout << std::hex << s[i].to_ulong() << '\n';
    }
}
void testwork(bit128 CK, bit128 IV)
{
    int L = 2;
    ZUC_init(CK, IV);
    auto key = ZUC_work(L);

    for (auto &i : key)
    {
        std::cout << std::hex << i.to_ulong() << '\n';
    }
}
};
