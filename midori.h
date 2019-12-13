#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

unsigned __int16* GetBlock(unsigned __int16 state)
{
	unsigned __int16* Block = new unsigned __int16[4];
	for (int i = 0; i < 4; i++)
		Block[i] = (state >> (4 * i)) & 0xf;

	return Block;
}

unsigned __int16 SubCell(unsigned __int16 state, unsigned __int16* S)
{
	unsigned __int16* Block = GetBlock(state);
	state = 0;

	for (int i = 0; i < 4; i++)
		state ^= S[Block[i]] << (4 * i);

	return state;
}

unsigned __int16 ShuffleCell(unsigned __int16 state, unsigned __int16* p)
{
	unsigned __int16* Block = GetBlock(state);
	state = 0;

	for (int i = 0; i < 4; i++)
		state ^= (Block[i] << (4 * p[i]));

	return state;
}

unsigned __int16 MixColumn(unsigned __int16 state)
{
	unsigned __int16* Block = GetBlock(state);
	state = 0;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (i != j)
				state ^= (Block[j] << (4 * i));

	return state;
}

unsigned __int16* GetRK(unsigned __int64 Key, unsigned __int16 * constants)
{
	unsigned __int16* RK = new unsigned __int16[15];

	unsigned __int16 K_1 = Key & 0xffff;
	unsigned __int16 K_0 = (Key >> 16) & 0xffff;

	for (int i = 0; i < 15; i++)
	{
		if (i % 2 == 0)
			RK[i] = K_0 ^ constants[i];
		else
			RK[i] = K_1 ^ constants[i];
	}
	return RK;
}

unsigned __int16 GetWK(unsigned __int64 Key)
{
	unsigned __int16 K_1 = (Key & 0xffff);
	unsigned __int16 K_0 = ((Key >> 16) & 0xffff);

	return ((K_0 ^ K_1) & 0xffff);
}

unsigned __int16 Midori16(unsigned __int16 rounds, unsigned __int64 Key, unsigned __int16 pt)
{
	//Задание констант
	unsigned __int16  constants[15] = { 0x243f, 0x6a88, 0x85a3,	0x08d3,	0x1319,	0x8a2e,	0x0370,	0x7344,	0xa409, 0x3822,	0x299f,	0x31d0,	0x082e,	0xfa98,	0xec4e };
	unsigned __int16 S[16] = { 0xC, 0xA, 0xD, 0x3, 0xE, 0xB, 0xF, 0x7, 0x8, 0x9, 0x1, 0x5, 0x0, 0x2, 0x4, 0x6 };
	unsigned __int16 p[4] = { 3, 0, 1, 2 };

	unsigned __int16* RK = new unsigned __int16[15];
	RK = GetRK(Key, constants);
	unsigned __int16 WK = GetWK(Key);

	//Шифрование
	unsigned __int16 state = pt ^ WK;
	for (int i = 0; i < (rounds - 2); i++)
	{
		state = SubCell(state, S);
		state = ShuffleCell(state, p);
		state = MixColumn(state);
		state = state ^ RK[i];	}	state = SubCell(state, S);
	unsigned __int16 ct = state ^ WK;

	return ct;
}