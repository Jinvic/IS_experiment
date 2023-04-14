#include <bits/stdc++.h>
using namespace std;

const int buffer_size = 2048;
enum Code_Mode
{
	encode,
	decode
};

enum Input_Source
{
	standard,
	file
};

// 初始置换表
const uint8_t IP_table[64] = {
	58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7};
// 逆初始置换表
const uint8_t FP_table[64] = {
	40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25};
// 选择置换1
const uint8_t PC_1_LK_table[28] = {
	56, 48, 40, 32, 24, 16, 8, 0, 57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18, 10, 2, 59, 51, 43, 35};
const uint8_t PC_1_RK_table[28] = {
	62, 54, 46, 38, 30, 22, 14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 60, 52, 44, 36, 28, 20, 12, 4, 27, 19, 11, 3};
// 移位表
const uint8_t shift_table[28] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};
// 选择置换2
const uint8_t PC_2_LP_table[48] = {
	13, 16, 10, 23, 0, 4, 2, 27,
	14, 5, 20, 9, 22, 18, 11, 3,
	25, 7, 15, 6, 26, 19, 12, 1};
const uint8_t PC_2_RP_table[48] = {
	12, 23, 2, 8, 18, 26, 1, 11,
	22, 16, 4, 19, 15, 20, 10, 27,
	5, 24, 17, 13, 21, 7, 0, 3};
// 32位扩展为48位
const uint8_t expand_table[48] = {
	31, 0, 1, 2, 3, 4, 3, 4, 5, 6, 7, 8,
	7, 8, 9, 10, 11, 12, 11, 12, 13, 14, 15, 16,
	15, 16, 17, 18, 19, 20, 19, 20, 21, 22, 23, 24,
	23, 24, 25, 26, 27, 28, 27, 28, 29, 30, 31, 0};
// S-box
const uint8_t S_box[8][4][16] = {
	// S_box_1
	{{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
	 {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
	 {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
	 {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
	// S_box_2
	{{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
	 {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
	 {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
	 {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
	// S_box_3
	{{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
	 {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
	 {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
	 {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
	// S_box_4
	{{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
	 {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
	 {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
	 {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
	// S_box_5
	{{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
	 {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
	 {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
	 {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
	// S_box_6
	{{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
	 {10, 15, 4, 2, 7, 12, 0, 5, 6, 1, 13, 14, 0, 11, 3, 8},
	 {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
	 {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
	// S_box_7
	{{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
	 {13, 0, 11, 7, 4, 0, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
	 {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
	 {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
	// S_box_8
	{{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
	 {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
	 {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
	 {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};
// P-box
const uint8_t P_box[32]{
	15, 6, 19, 20, 28, 11, 27, 16, 0, 14, 22, 25, 4, 17, 30, 9,
	1, 7, 23, 13, 31, 26, 2, 8, 18, 12, 29, 5, 21, 10, 3, 24};

// inte可为char,short,int,long long。对应字节长度分别为1,2,4,8
void itob(uint8_t *inte, int ilen, uint8_t *bin, int blen)
{
	// memset(bin, 0, sizeof(byte) * len);

	// inte从低位到高位取字节按位赋值给bin数组
	int i = blen - 1, j = ilen - 1, cnt = 0;
	uint8_t x = inte[j];
	for (; i >= 0; i--)
	{
		bin[i] = x & 1;
		x >>= 1;

		cnt++;
		if (cnt == 8) // 完成一个字节
		{
			cnt = 0;
			x = inte[--j];
		}
	}
}
// inte可为char,short,int,long long。对应字节长度分别为1,2,4,8
void btoi(uint8_t *inte, int ilen, uint8_t *bin, int blen)
{
	// memset(bin, 0, sizeof(byte) * len);
	// bin从高位到低位按位取出赋值给inte
	int i = 0, j = 0, cnt = 0;
	uint8_t x = 0;
	for (; i < blen; i++)
	{
		x = (x << 1) | bin[i];

		cnt++;
		if (cnt == 8) // 完成一个字节
		{
			cnt = 0;
			inte[j++] = x;
		}
	}
}

/* MARK: DES加密相关 */
// 初始置换
void IP_func(uint8_t *block_64_bin)
{
	uint8_t cpy[64];
	memcpy(cpy, block_64_bin, 64);
	for (int i = 0; i < 64; i++)
		block_64_bin[i] = cpy[IP_table[i]];
}
// 逆初始置换
void FP_func(uint8_t *block_64_bin)
{
	uint8_t cpy[64];
	memcpy(cpy, block_64_bin, 64);
	for (int i = 0; i < 64; i++)
		block_64_bin[i] = cpy[FP_table[i]];
}

// 将32位二进制数组扩展为48位
void R_expand(uint8_t *input_32_bin, uint8_t *output_48_bin)
{
	for (int i = 0; i < 48; i++)
		output_48_bin[i] = input_32_bin[expand_table[i]];
}

// 将6位二进制数组压缩为4位
void S_box_compress(uint8_t *input_6_bin, uint8_t *output_4_bin, int Sb_num)
{
	int row = (input_6_bin[0] << 1) + input_6_bin[5]; // 首尾两位
	int col = (input_6_bin[1] << 3) + (input_6_bin[2] << 2) +
			  (input_6_bin[3] << 1) + input_6_bin[4]; // 中间四位
	uint8_t res = S_box[Sb_num][row][col];
	itob(&res, 1, output_4_bin, 4);
}

// P-box
void P_box_func(uint8_t *input_32_bin)
{
	uint8_t cpy[32];
	memcpy(cpy, input_32_bin, 32);
	for (int i = 0; i < 32; i++)
		input_32_bin[i] = cpy[P_box[i]];
}

void DES_Feistel_F(uint8_t *R_32_bin, uint8_t *Ki_48_bin)
{
	// 存位
	uint8_t R_48_bin[48];
	// 32为扩展为48位
	R_expand(R_32_bin, R_48_bin);
	// R和Ki异或
	for (int i = 0; i < 48; i++)
		R_48_bin[i] = R_48_bin[i] ^ Ki_48_bin[i];
	// R由48位压缩至32位
	for (int i = 0, j = 0; i < 48; i += 6, j += 4)
		S_box_compress(R_48_bin + i, R_32_bin + j, i % 6);
	// P-box
	P_box_func(R_32_bin);
}

void DES_round(uint8_t *L_32_bin, uint8_t *R_32_bin, uint8_t *Ki_48_bin)
{
	uint8_t L0_32_bin[32], R1_32_bin[32];
	// 保存上一轮的L
	memcpy(L0_32_bin, L_32_bin, 32);
	// 上一轮的R作为下一轮的L
	memcpy(L_32_bin, R_32_bin, 32);
	// 计算这一轮的R
	DES_Feistel_F(R_32_bin, Ki_48_bin);
	for (int i = 0; i < 32; i++)
		R_32_bin[i] = L0_32_bin[i] ^ R_32_bin[i];
}

void DES_func(uint8_t *block_64_bin, uint8_t (*ki_16_48_bin)[48])
{
	uint8_t *L_32_bin, *R_32_bin;
	uint8_t cpy[32]; // 用于最后一轮运算完成后交换LR

	// 初始置换
	IP_func(block_64_bin);
	L_32_bin = block_64_bin;
	R_32_bin = block_64_bin + 32;

	// MARK:循环16轮
	for (int i = 0; i < 16; i++)
		DES_round(L_32_bin, R_32_bin, ki_16_48_bin[i]);

	// MARK:最后一轮运算后对左右两部分进行互换，实际密文为(R16,L16)
	memcpy(cpy, L_32_bin, 32);
	memcpy(L_32_bin, R_32_bin, 32);
	memcpy(R_32_bin, cpy, 32);

	// 逆初始置换
	FP_func(block_64_bin);
}

/* MARK: 密钥调度相关 */
// 选择置换1
void PC_1(uint8_t *key_64_bin, uint8_t *LK_28_bin, uint8_t *RK_28_bin)
{
	for (int i = 0; i < 28; i++)
		LK_28_bin[i] = key_64_bin[PC_1_LK_table[i]];
	for (int i = 0; i < 28; i++)
		RK_28_bin[i] = key_64_bin[PC_1_RK_table[i]];
}
// 选择置换2
void PC_2(uint8_t *ki_48_bin, uint8_t *LK_28_bin, uint8_t *RK_28_bin)
{

	for (int i = 0; i < 24; i++)
		ki_48_bin[i] = LK_28_bin[PC_2_LP_table[i]];
	for (int i = 0; i < 24; i++)
		ki_48_bin[24 + i] = RK_28_bin[PC_2_RP_table[i]];
}

void key_schedule(uint8_t *key_64_bin, uint8_t (*ki_16_48_bin)[48])
{
	// 数组开双倍大小，用指针代替移位操作
	uint8_t LK_56_bin[56], RK_56_bin[56];
	int plk = 0, prk = 0;
	// 选择置换1
	PC_1(key_64_bin, LK_56_bin, RK_56_bin);
	// PC_1(key_64_bin, LK_56_bin + 28, RK_56_bin + 28);
	memcpy((LK_56_bin + 28), LK_56_bin, 28);
	memcpy((RK_56_bin + 28), RK_56_bin, 28);

	for (int i = 0; i < 16; i++)
	{
		// 移位操作
		plk = (plk + shift_table[i] + 28) % 28;
		prk = (prk + shift_table[i] + 28) % 28;

		// 选择置换2
		PC_2(ki_16_48_bin[i], LK_56_bin + plk, RK_56_bin + prk);
	}
}

/* MARK: 预处理相关 */
// 对明文预处理，补齐为64位的倍数并返回长度
void encode_multi64_func(char *buffer, int *len)
{
	int mod = *len % 8;
	// 补齐64位方便分组
	memset(buffer + *len, 0, sizeof(char) * (mod));
	*len += mod;
	buffer[*len] = '\0';
}

// 加密时，将最后的密文处理为可显示字符
void encode_visable_func(char *cipher_buf, int *len, uint64_t *blocks, int blen)
{
	// 每个字节每次取4位,即每一字符拆成两个字符
	unsigned char row[8];
	unsigned char doub[2];
	for (int i = 0; i < blen; i++)
	{
		memcpy(row, &blocks[i], 8);
		for (int j = 0; j < 8; j++)
		{
			doub[0] = 64 + (row[j] >> 4); // 前4位
			doub[1] = 80 + (row[j] & 15); // 后4位
			memcpy(cipher_buf + (i * 8 * 2) + (j * 2), &doub, 2);
		}
	}
	*len = *len * 2;
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
};

// input_flag决定是从命令行还是文件读入
void main_func(Input_Source input_flag, Code_Mode code_mode)
{
	FILE *plaintext_source, *key_source, *cipher_dest;
	// 决定输入源
	switch (input_flag)
	{
	case standard:
	default:
		plaintext_source = stdin;
		key_source = stdin;
		cipher_dest = stdout;
		break;
	case file:
		plaintext_source = fopen("input.txt", "rb");
		key_source = fopen("key.txt", "rb");
		cipher_dest = fopen("output.txt", "wb");
		break;
	}

	// 输入明文
	char plaintext_buf[buffer_size] = {0};
	char cipher_buf[buffer_size] = {0};
	int len, blen; // 字符数组长度，分组数组长度
	// fgets(plaintext_buf, buffer_size, plaintext_sourse);
	fread(plaintext_buf, sizeof(char), buffer_size, plaintext_source);
	fflush(plaintext_source);
	len = strlen(plaintext_buf);
	if (code_mode == encode)
		// 对明文预处理，补齐为64位的倍数并返回长度
		encode_multi64_func(plaintext_buf, &len);
	else
		// 加密时最最终密文进行了可视化处理，所以解密时需要还原为原始密文
		decode_visable_rev_func(plaintext_buf, &len);
	blen = len / 8;

	// 输入密钥
	char key[8] = {0};
	// fgets(key, 8 + 1, key_sourse);
	fread(key, sizeof(char), 8, key_source);
	fflush(key_source);
	uint8_t key_64_bin[64];
	uint8_t ki_16_48_bin[16][48];
	itob((uint8_t *)key, 8, key_64_bin, 64);

	// MARK:密钥调度
	key_schedule(key_64_bin, ki_16_48_bin);

	// MARK: 明文分组
	uint64_t *blocks = (uint64_t *)calloc(blen, sizeof(uint64_t)); // 存放所有分组
	uint64_t block_64;											   // 存放当前操作的分组
	uint8_t block_64_bin[64];

	// 一次处理8字节即64位
	for (int i = 0, cnt = 0; i < len; i += 8)
	{
		memcpy(&block_64, plaintext_buf + i, 8);
		itob((uint8_t *)&block_64, 8, block_64_bin, 64);

		DES_func(block_64_bin, ki_16_48_bin);

		btoi((uint8_t *)&block_64, 8, block_64_bin, 64);
		blocks[cnt++] = block_64;
	}

	if (code_mode == encode)
	{
		// 预处理为可显示字符，每次取4位,即每一字节拆成两个字符
		encode_visable_func(cipher_buf, &len, blocks, blen);
	}
	else // decode
	{
		for (int i = 0; i < blen; i++)
			memcpy(cipher_buf + i * 8, &blocks[i], 8);
	}

	// 输出
	fwrite(cipher_buf, sizeof(char), len, cipher_dest);
	// fprintf(cipher_dest, "%s", cipher_buf);

	if (input_flag == file)
	{
		fclose(plaintext_source);
		fclose(key_source);
		fclose(cipher_dest);
	}
}

signed main(void)
{
	main_func(file, encode);
	// main_func(file, decode);

	return 0;
}

//TODO:密文解密与明文不符