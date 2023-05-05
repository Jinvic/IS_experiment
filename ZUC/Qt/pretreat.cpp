#include<stdint.h>
#include<cstring>
#include<string>

// 预处理相关
namespace PT
{
//  加密时，将最后的密文处理为可显示字符
void encode_visable_func(char *cipher_buf, int *len)
{
    uint8_t *cpy = new uint8_t[*len];
    memcpy(cpy, cipher_buf, *len);

    // 每个字节每次取4位,即每一字符拆成两个字符
    for (int i = 0, p = 0; i < *len; i++, p += 2)
    {

        cipher_buf[p] = 64 + (cpy[i] >> 4);     // 前4位
        cipher_buf[p + 1] = 80 + (cpy[i] & 15); // 后4位
    }
    *len = *len * 2;
    delete[] cpy;
};

// 解密时，将密文还原为原始密文
void decode_visable_rev_func(char *cipher_buf, int *len)
{
    // 每两个字符取后4位拼成一个字节
    uint8_t l, r;
    *len = *len / 2;
    for (int i = 0, p = 0; i < *len; i++, p += 2)
    {
        l = (cipher_buf[p] - 64);
        r = (cipher_buf[p + 1] - 80);
        cipher_buf[i] = (l << 4) + r;
    }
    memset(cipher_buf + *len, 0, sizeof(char) * (*len));
};

// 一个字符 十六进制转换为十进制
uint8_t hex_to_dec_c(char c)
{
    uint8_t res;
    if (c >= 'a' && c <= 'z')
        res = c - 'a' + 10;
    else if ((c >= 'A' && c <= 'Z'))
        res = c - 'A' + 10;
    else if ((c >= '0' && c <= '9'))
        res = c - '0';
    else
        res = 0;
    return res;
}

// k和vi 十六进制转换为十进制
void hex_to_dec(std::string str, uint8_t *val)
{
    uint8_t l, r;
    if(str.length() & 1 && str.length() < 32)str += "0";
    int len = (str.length() < 32) ? str.length() : 32;
    for (int i = 0; i < len; i += 2)
    {
        l = hex_to_dec_c(str[i]);
        r = hex_to_dec_c(str[i + 1]);
        val[i / 2] = (l << 4) + r;
    }
}
}
