/**
 * @author Ricardoly
 * @date 2023-05-06
 * @file BIT.hpp
 */

#pragma once

#include "const.hpp"

namespace BIT
{
	template <typename T>
	bit32 H(T x)
	{
		x >>= (x.size() / 2);
		bit32 ans = 0;
		for (int i = 0; i < 16; ++i)
		{
			ans[i] = x[i];
		}
		return ans;
	}

	template <typename T>
	bit32 L(T x)
	{
		bit32 ans = 0;
		for (int i = 0; i < 16; ++i)
		{
			ans[i] = x[i];
		}
		return ans;
	}

	bit32 shift(bit32 x, int len) // x default 32bit
	{
		len %= 32;
		bit32 left = 0, right = 0;
		for (int i = 0; i < len; ++i)
		{
			left[len - 1 - i] = x[31 - i];
		}
		for (int i = 0; i < 32 - len; ++i)
		{
			right[i] = x[i];
		}
		return (right << len) | left;
	}

	bit32 L1(bit32 x) // x default 32bit
	{
		return x ^ shift(x, 2) ^ shift(x, 10) ^ shift(x, 18) ^ shift(x, 24);
	}

	bit32 L2(bit32 x) // x default 32bit
	{
		return x ^ shift(x, 8) ^ shift(x, 14) ^ shift(x, 22) ^ shift(x, 30);
	}

	bit32 S(bit32 X) // X default 32bit
	{
		bit8 x[4]; // X = x0||x1||x2||x3  x[i] 8bit
		bit8 y[4]; // Y = y0||y1||y2||y3  y[i] 8bit
		bit32 Y;   // 32bit

		x[0] = ((X << 0) >> 24).to_ullong();
		x[1] = ((X << 8) >> 24).to_ullong();
		x[2] = ((X << 16) >> 24).to_ullong();
		x[3] = ((X << 24) >> 24).to_ullong();

		for (int i = 0; i < 4; ++i)
		{
			int h = (x[i] >> 4).to_ullong();
			int l = ((x[i] << 4) >> 4).to_ullong();
			y[i] = (i & 1) ? S1[h][l] : S0[h][l];
		}

		Y = (y[0].to_ullong() << 24) + (y[1].to_ullong() << 16) + (y[2].to_ullong() << 8) + y[3].to_ullong();
		return Y;
	}
};