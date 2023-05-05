#include <cstdio>
#include <cstring>
#include <windows.h>
#include "ZUC.cpp"

enum Code_Mode
{
    encode,
    decode
};
enum IO_Flag
{
    CLI,
    file
};
const int buffer_size = 8192;

// 预处理相关
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
void hex_to_dec(char *str, uint8_t *val)
{
    uint8_t l, r;
    int len = (strlen(str) < 32) ? strlen(str) : 32;
    for (int i = 0; i < 32; i += 2)
    {
        l = hex_to_dec_c(str[i]);
        r = hex_to_dec_c(str[i + 1]);
        val[i / 2] = (l << 4) + r;
    }
}

// 输入输出等外围操作
//  input_flag决定是从命令行还是文件读写
void IO_func(Code_Mode code_mode, IO_Flag IO_flag)
{
    FILE *input_source, *output_dest;
    FILE *k_source, *vi_source;
    // 决定读写方式
    switch (IO_flag)
    {
    case CLI:
    default:
        input_source = stdin;
        k_source = stdin;
        vi_source = stdin;
        output_dest = stdout;
        break;
    case file:
        input_source = fopen("input.txt", "rb");
        k_source = fopen("k.txt", "rb");
        vi_source = fopen("vi.txt", "rb");
        output_dest = fopen("output.txt", "wb");
        break;
    }

    // 输入明文，初始种子密钥k和初始向量vi
    char input_buf[buffer_size * 2] = {0};
    char output_buf[buffer_size * 2] = {0}; // 输出hex值，数组开两倍大小
    char k[32] = {0}, vi[32] = {0};
    int len;
    if (IO_flag == CLI)
    {
        fgets(input_buf, buffer_size + 1, input_source);
        fgets(k, 32 + 1, k_source);
        fgets(vi, 32 + 1, vi_source);
    }
    else // file
    {
        fread(input_buf, sizeof(char), buffer_size, input_source);
        fread(k, sizeof(char), 32, k_source);
        fread(vi, sizeof(char), 32, vi_source);
    }
    fflush(input_source);
    len = strlen(input_buf);
    // 删除命令行输入时多余的回车
    if (IO_flag == CLI && input_buf[len - 1] == '\n')
    {
        input_buf[len - 1] = '\0';
        len--;
    }

    if (code_mode == decode)
        decode_visable_rev_func(input_buf, &len);
    else                      // encode
        len += (4 - len % 4); // 补齐为四字节倍数

    // MARK:主函数入口
    ZUC::stream_work(input_buf, output_buf, len,
                     (uint8_t *)k, (uint8_t *)vi);

    if (code_mode == encode)
        encode_visable_func(output_buf, &len);
    else // decode
        while (len && output_buf[len - 1] == '\0')
            len--; // 删去补齐时增加的长度

    // 输出
    fwrite(output_buf, sizeof(char), len, output_dest);

    if (IO_flag == file)
    {
        fclose(input_source);
        fclose(k_source);
        fclose(vi_source);
        fclose(output_dest);
    }
};

// 命令行交互界面
void CLI_interact(void)
{
    Code_Mode code_mode;
    IO_Flag IO_flag;

    char cmd;         // 输入命令
    bool input_legal; // 输入合法性校验

    // 选择 加密encode/解密decode
    input_legal = false;
    while (!input_legal)
    {
        printf("选择 加密[E]/解密[D]:\n");
        scanf("%c", &cmd);
        fflush(stdin);
        switch (cmd)
        {
        case 'e':
        case 'E':
            code_mode = encode;
            input_legal = true;
            break;
        case 'd':
        case 'D':
            code_mode = decode;
            input_legal = true;
            break;
        default:
            printf("输入不合法，请重新输入\n");
            break;
        }
    }

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
        printf("先输入文本（仅一行），可以空格，长度不超过%d字符\n", buffer_size);
        printf("再输入初始种子密钥k的hex值，长度不超过%d字符\n", 32);
        printf("最后输入初始向量vi的hex值，长度不超过%d字符\n", 32);
    }
    else // file
    {
        printf("在input.txt中输入需要处理的文本，长度不超过%d字符\n", buffer_size);
        printf("在k.txt中输入初始种子密钥k的hex值，长度不超过%d字符\n", 32);
        printf("在vi.txt中输入初始向量vi的hex值，长度不超过%d字符\n", 32);
        printf("结果将输出到output.txt中\n");
        printf("完成输入后按任意键继续\n");
        system("pause");
    }
    IO_func(code_mode, IO_flag);
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
