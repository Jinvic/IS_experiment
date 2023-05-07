#include <bits/stdc++.h>
#include "ZUC.hpp"
using namespace std;

bit128 hex_to_bit128(std::string str)
{
    // 补齐为32位hex值
    if (str.length() > 32)
        str = str.substr(0, 32);
    else
        while (str.length() < 32)
            str = '0' + str;

    bit128 res;
    uint8_t tmp1;
    bit8 tmp2;
    for (int i = 0, idx = 15; i < 32; i += 2)
    {
        // 一次处理8位
        sscanf(str.substr(i, 2).c_str(), "%X", &tmp1);
        tmp2 = tmp1;
        for (int j = 0; j < 8; j++)
            res[idx * 8 + j] = tmp2[j];
        idx -= 1;
    }
    return res;
}

std::string str_to_bit(std::string str)
{
    std::string res = "";
    bit8 tmp;
    for (auto c : str)
    {
        tmp = c;
        for (int i = 7; i >= 0; i--)
            res += '0' + tmp[i];
    }
    return res;
}

std::string bit_to_str(std::string str)
{
    std::string res = "";
    uint8_t tmp;
    for (int i = 0, idx = 0; i < str.length(); i += 8)
    {
        tmp = 0;
        for (int j = 0; j < 8; j++)
            tmp = (tmp << 1) + (str[idx * 8 + j] - '0');
        res += '\0' + tmp;
        idx += 1;
    }
    return res;
}

std::string str_to_hex(std::string str)
{
    std::string res = "";
    uint8_t tmp1;
    char tmp2[2];

    for (int i = 0; i < str.length(); i++)
    {
        tmp1 = str[i];
        sprintf(tmp2, "%02X", tmp1);
        res += tmp2[0];
        res += tmp2[1];
    }
    return res;
}

std::string hex_to_str(std::string str)
{
    std::string res = "";
    char tmp1[2];
    uint8_t tmp2;

    for (int i = 0; i < str.length(); i += 2)
    {
        tmp1[0] = str[i];
        tmp1[1] = str[i + 1];
        sscanf(tmp1, "%2X", &tmp2);
        res += '\0' + tmp2;
    }
    return res;
}

std::string bit_to_hex(std::string str)
{
    return str_to_hex((bit_to_str(str)));
}

std::string hex_to_bit(std::string str)
{
    return hex_to_str((str_to_bit(str)));
}

std::string ZUCEncrypt(bit128 CK, bit128 IV, int LENGTH, std::string IBS)
{
    int L = (LENGTH + 31) / 32;
    ZUC::ZUC_init(CK, IV);
    auto key = ZUC::ZUC_work(L);

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
        OBS += tmp;
    }
    return OBS;
}

int main(void)
{
    // string a = "1111000000111100";
    // string b = bit_to_hex(a);
    // string c = b;
    // string d = hex_to_bit(c);
    // cout << a << '\n'
    //      << b << '\n'
    //      << c << '\n'
    //      << d << '\n';

    string k_str = "123";
    string iv_str = "123";
    bit128 k = hex_to_bit128(k_str);
    bit128 iv = hex_to_bit128(iv_str);

    enum Code_Mode
    {
        encode,
        decode
    } cm = decode;
    if (cm==encode)
    {
        string input_str = "123";
        string input_bit = str_to_bit(input_str);
        string output_bit = ZUCEncrypt(k, iv, input_bit.length(), input_bit);
        string output_str = bit_to_str(output_bit);
        output_str = str_to_hex(output_str);
        cout << output_str << '\n';
    }
    else
    {
        string input_str = "A4F208";
        input_str = hex_to_str(input_str);
        string input_bit = str_to_bit(input_str);
        string output_bit = ZUCEncrypt(k, iv, input_bit.length(), input_bit);
        string output_str = bit_to_str(output_bit);
        cout << output_str << '\n';
    }
}