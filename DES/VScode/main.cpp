#include <bits/stdc++.h>
using namespace std;

const int buffer_size = 2048;
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

// 初始置换表
const uint8_t IP_table[64] = {
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7,
	56, 48, 40, 32, 24, 16, 8, 0,
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6};
// 逆初始置换表
const uint8_t FP_table[64] = {
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41, 9, 49, 17, 57, 25,
	32, 0, 40, 8, 48, 16, 56, 24};
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
	// DEBUG:
	/* uint8_t l[4], r[4];
	btoi(l, 4, L_32_bin, 32);
	btoi(r, 4, R_32_bin, 32);
	printf("L0R0如下:\n");
	for (int i = 0; i < 4; i++)
		printf("%d ", l[i]);
	for (int i = 0; i < 4; i++)
		printf("%d ", r[i]);
	printf("\n"); */

	uint8_t L0_32_bin[32], R1_32_bin[32];
	// 保存上一轮的L
	memcpy(L0_32_bin, L_32_bin, 32);
	// 上一轮的R作为下一轮的L
	memcpy(L_32_bin, R_32_bin, 32);
	// 计算这一轮的R
	DES_Feistel_F(R_32_bin, Ki_48_bin);
	for (int i = 0; i < 32; i++)
		R_32_bin[i] = L0_32_bin[i] ^ R_32_bin[i];

	// DEBUG:
	/* btoi(l, 4, L_32_bin, 32);
	btoi(r, 4, R_32_bin, 32);
	printf("L1R1如下:\n");
	for (int i = 0; i < 4; i++)
		printf("%d ", l[i]);
	for (int i = 0; i < 4; i++)
		printf("%d ", r[i]);
	printf("\n"); */
}

// 加密一个分组
void DES_func(uint8_t *block_64_bin, uint8_t (*ki_16_48_bin)[48], Code_Mode code_mode)
{
	uint8_t *L_32_bin, *R_32_bin;
	uint8_t cpy[32]; // 用于最后一轮运算完成后交换LR

	// DEBUG:
	/* uint64_t block_64;
	btoi((uint8_t *)&block_64, 8, block_64_bin, 64);
	printf("IP前 %lld\n", block_64); */

	// 初始置换
	IP_func(block_64_bin);
	L_32_bin = block_64_bin;
	R_32_bin = block_64_bin + 32;

	// DEBUG:
	/* btoi((uint8_t *)&block_64, 8, block_64_bin, 64);
	printf("IP后 %lld\n", block_64); */

	// MARK:循环16轮
	for (int i = 0; i < 16; i++)
		if (code_mode == encode)
			DES_round(L_32_bin, R_32_bin, ki_16_48_bin[i]);
		else
			DES_round(L_32_bin, R_32_bin, ki_16_48_bin[15 - i]);

	// MARK:加密时，最后一轮运算后对左右两部分进行互换，实际密文为(R16,L16)
	// 解密时，最开始密文为(R16,L16)，16轮后变为(R1,L1)，交换为(L1,R1)
	memcpy(cpy, L_32_bin, 32);
	memcpy(L_32_bin, R_32_bin, 32);
	memcpy(R_32_bin, cpy, 32);

	// DEBUG:
	/* btoi((uint8_t *)&block_64, 8, block_64_bin, 64);
	printf("FP前 %lld\n", block_64); */

	// 逆初始置换
	FP_func(block_64_bin);

	// DEBUG:
	/* btoi((uint8_t *)&block_64, 8, block_64_bin, 64);
	printf("FP后 %lld\n", block_64); */
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
void encode_visable_func(char *cipher_buf, int *len)
{
	uint8_t *cpy = (uint8_t *)calloc(*len, sizeof(char));
	memcpy(cpy, cipher_buf, *len);

	// 每个字节每次取4位,即每一字符拆成两个字符
	for (int i = 0, p = 0; i < *len; i++, p += 2)
	{

		cipher_buf[p] = 64 + (cpy[i] >> 4);		// 前4位
		cipher_buf[p + 1] = 80 + (cpy[i] & 15); // 后4位
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

// MARK: 主函数，负责明文分组和密钥调度
void main_func(char *plaintext_buf, char *cipher_buf, int len, char *key, Code_Mode code_mode)
{
	uint8_t key_64_bin[64];
	uint8_t ki_16_48_bin[16][48];
	itob((uint8_t *)key, 8, key_64_bin, 64);

	// MARK:密钥调度
	key_schedule(key_64_bin, ki_16_48_bin);

	// MARK: 明文分组
	int blen = len / 8;
	uint64_t *blocks = (uint64_t *)calloc(blen, sizeof(uint64_t)); // 存放所有分组
	uint64_t block_64;											   // 存放当前操作的分组
	uint8_t block_64_bin[64];

	// 一次处理8字节即64位
	for (int i = 0, cnt = 0; i < len; i += 8)
	{
		memcpy(&block_64, plaintext_buf + i, 8);
		itob((uint8_t *)&block_64, 8, block_64_bin, 64);
		// DEBUG:
		/* printf("%lld\n", block_64); */

		DES_func(block_64_bin, ki_16_48_bin, code_mode);

		btoi((uint8_t *)&block_64, 8, block_64_bin, 64);
		blocks[cnt++] = block_64;
		// DEBUG:
		/* printf("%lld\n", block_64); */
	}

	for (int i = 0; i < blen; i++)
		memcpy(cipher_buf + i * 8, &blocks[i], 8);
}

// 输入输出等外围操作
//  input_flag决定是从命令行还是文件读写
//  code_mode决定是解密还是加密
void IO_func(IO_Flag IO_flag, Code_Mode code_mode)
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
	char output_buf[buffer_size] = {0};
	int len;
	if (IO_flag == CLI)
		fgets(input_buf, buffer_size + 1, input_source);
	else // file
		fread(input_buf, sizeof(char), buffer_size, input_source);
	fflush(input_source);
	len = strlen(input_buf);
	if (IO_flag == CLI && input_buf[len - 1] == '\n')
		len--; // 删除命令行输入时多余的回车
	if (code_mode == encode)
		// 对明文预处理，补齐为64位的倍数并返回长度
		encode_multi64_func(input_buf, &len);
	else
		// 加密时最最终密文进行了可视化处理，所以解密时需要还原为原始密文
		decode_visable_rev_func(input_buf, &len);

	// 输入密钥
	char key[10] = {0};
	if (IO_flag == CLI)
		fgets(key, 8 + 1, key_source);
	else // file
		fread(key, sizeof(char), 8, key_source);
	fflush(key_source);
	if (IO_flag == CLI && key[strlen(key) - 1] == '\n')
		key[strlen(key) - 1] = '\0'; // 删除命令行输入时多余的回车

	// MARK:主函数入口
	main_func(input_buf, output_buf, len, key, code_mode);

	if (code_mode == encode)
		// 预处理为可显示字符，每次取4位,即每一字节拆成两个字符
		encode_visable_func(output_buf, &len);

	// 输出
	fwrite(output_buf, sizeof(char), len, output_dest);
	// fprintf(output_dest, "%s", output_buf);

	if (IO_flag == file)
	{
		fclose(input_source);
		fclose(key_source);
		fclose(output_dest);
	}
}

// 命令行交互界面
void CLI_interact(void)
{
	IO_Flag IO_flag;
	Code_Mode code_mode;

	char cmd;		  // 输入命令
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
		printf("再输入密钥,长度不超过8字符\n");
	}
	else // file
	{
		printf("在input.txt中输入需要处理的文本，长度不超过%d字符\n", buffer_size);
		printf("再在key.txt中输入密钥,长度不超过8字符\n");
		printf("结果将输出到output.txt中\n");
		printf("完成输入后按任意键继续\n");
		system("pause");
	}
	IO_func(IO_flag, code_mode);
	printf("\n已完成\n");
}

// DEBUG: 开发时测试用，无需阅读
namespace test
{
	// 检查输入输出每个字符的值是否一致
	void main_func_test(Code_Mode code_mode)
	{
		char a[20] = {};
		if (code_mode == encode)
		{ // a="12345678"
			printf("字符串\"12345678\"对应值如下\n");
			for (int i = 0; i < 8; i++)
			{
				a[i] = '1' + i;
				printf("%d ", a[i]);
			}
			printf("\n");
		}
		else
		{
			printf("输入密文对应值\n");
			for (int i = 0; i < 8; i++)
				scanf("%d", &a[i]);
		}

		char b[20] = "";
		char c[20] = "87654321";

		main_func(a, b, 8, c, code_mode);

		printf("DES后对应值如下\n");
		for (int i = 0; i < 8; i++)
			printf("%d ", b[i]);
		printf("\n");
	}

	// 检查IP和FP函数是否互逆
	void IP_FP_func_test(void)
	{
		uint8_t a[8], b[8], a_cpy[8];
		uint8_t a_bin[64], b_bin[64];
		printf("字符串\"12345678\"对应值如下\n");
		for (int i = 0; i < 8; i++)
		{
			a[i] = '1' + i;
			printf("%d ", a[i]);
		}
		printf("\n");
		memcpy(a_cpy, a, 8);

		printf("IP置换结果如下\n");
		itob(a, 8, a_bin, 64);
		IP_func(a_bin);
		btoi(a, 8, a_bin, 64);
		for (int i = 0; i < 8; i++)
			printf("%d ", a[i]);
		printf("\n");

		printf("输入IP置换结果\n");
		for (int i = 0; i < 8; i++)
			scanf("%d", &b[i]);
		printf("FP置换结果如下\n");
		itob(b, 8, b_bin, 64);
		FP_func(b_bin);
		btoi(b, 8, b_bin, 64);
		for (int i = 0; i < 8; i++)
			printf("%d ", b[i]);
		printf("\n");

		bool flag = true;
		for (int i = 0; i < 8; i++)
			if (a_cpy[i] != b[i])
			{
				flag = false;
				break;
			}
		if (flag)
			printf("正确，IP和FP互为逆运算\n");
		else
			printf("错误，IP和FP不互为逆运算\n");
	}

	// 检查DES的每一轮是否可逆
	void DES_round_test(void)
	{
		// 密钥调度
		uint8_t key[6] = {'1', '2', '3', '4', '5', '6'};
		uint8_t key_48_bin[48];
		itob(key, 6, key_48_bin, 48);

		uint8_t a[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};
		uint8_t a_cpy[8], b[8];
		int b_tmp[8];
		uint8_t a_bin[64], b_bin[64];

		printf("原数据为:\n");
		for (int i = 0; i < 8; i++)
			printf("%d ", a[i]);
		printf("\n");
		memcpy(a_cpy, a, 8);

		itob(a, 8, a_bin, 64);
		DES_round(a_bin, &a_bin[32], key_48_bin);
		btoi(a, 8, a_bin, 64);
		printf("一轮DES后值为\n");
		for (int i = 0; i < 8; i++)
			printf("%d ", a[i]);
		printf("\n");

		printf("LR互换后再输入\n");
		for (int i = 0; i < 8; i++)
		{
			scanf("%d", &b_tmp[i]);
			b[i] = b_tmp[i];
		}
		itob(b, 8, b_bin, 64);
		DES_round(b_bin, &b_bin[32], key_48_bin);
		btoi(b, 8, b_bin, 64);
		printf("一轮DES后值为\n");
		for (int i = 0; i < 8; i++)
			printf("%d ", b[i]);
		printf("\n");

		bool flag = true;
		for (int i = 0; i < 8; i++)
			if (a_cpy[i] != b[(i + 4) % 8])
			{
				flag = false;
				break;
			}
		if (flag)
			printf("正确，DES_round可逆\n");
		else
			printf("错误，DES_round不可逆\n");
	}

	// 验证DES_Feistel_F函数的正确性
	void DES_Feistel_F_test(void)
	{
		uint8_t L0[4] = {'1', '2', '3', '4'};
		uint8_t R0[4] = {'5', '6', '7', '8'};
		uint8_t R0_cpy[4];
		memcpy(R0_cpy, R0, 4);
		uint8_t R_bin[32];
		itob(R0, 4, R_bin, 32);
		printf("R值为\n");
		for (int i = 0; i < 4; i++)
			printf("%d ", R0[i]);
		printf("\n");

		uint8_t key[6] = {'1', '2', '3', '4', '5', '6'};
		uint8_t key_bin[48];
		itob(key, 6, key_bin, 48);

		uint8_t res[4];
		DES_Feistel_F(R_bin, key_bin);
		btoi(res, 4, R_bin, 32);

		// printf("Ki值为\n1 2 3 4 5 6\n");
		printf("F(R,Ki)结果为：\n");
		for (int i = 0; i < 4; i++)
			printf("%d ", res[i]);
		printf("\n");
	}

	// 测试16轮DES的输入输出是否可逆
	void DES_round_16_test(void)
	{
		uint8_t key[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};
		uint8_t key_bin[64];
		uint8_t ki_16_48_bin[16][48];
		itob(key, 8, key_bin, 64);
		key_schedule(key_bin, ki_16_48_bin);

		uint8_t L[4] = {'1', '2', '3', '4'};
		uint8_t R[4] = {'5', '6', '7', '8'};
		uint8_t L_32_bin[32], R_32_bin[32];
		itob(L, 4, L_32_bin, 32);
		itob(R, 4, R_32_bin, 32);
		// unsigned int LL, RR;
		// memcpy(&LL, L, 4);
		// memcpy(&RR, R, 4);

		// 加密
		printf("L0R0 %d %d\n", L, R);
		for (int i = 0; i < 16; i++)
		{
			DES_round(L_32_bin, R_32_bin, ki_16_48_bin[i]);
			btoi(L, 4, L_32_bin, 32);
			btoi(R, 4, R_32_bin, 32);
			printf("L%dR%d %d %d\n", i + 1, i + 1, L, R);
		}

		uint8_t cpy[32]; // 用于最后一轮运算完成后交换LR
		memcpy(cpy, L_32_bin, 32);
		memcpy(L_32_bin, R_32_bin, 32);
		memcpy(R_32_bin, cpy, 32);

		// 解密
		btoi(R, 4, L_32_bin, 32);
		btoi(L, 4, R_32_bin, 32);
		printf("R16L16 %d %d\n", R, L);
		for (int i = 0; i < 16; i++)
		{
			DES_round(L_32_bin, R_32_bin, ki_16_48_bin[15 - i]);
			btoi(R, 4, L_32_bin, 32);
			btoi(L, 4, R_32_bin, 32);
			printf("R%dL%d %d %d\n", 15 - i, 15 - i, R, L);
		}
	}
}

signed main(void)
{
	// DEBUG:
	//  test::main_func_test(encode);
	//  test::main_func_test(decode);
	//  test::IP_FP_func_test();
	//  test::DES_round_test();
	//  test::DES_Feistel_F_test();
	//  test::DES_round_16_test();
	// IO_func(file, encode);
	// IO_func(file, decode);

	CLI_interact();
	return 0;
}
