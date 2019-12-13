#pragma once
#include "midori.h"

#define reg_num 4
#define default_gamma_size_Kb 1024
#define gamma_size (gamma_size_Kb*8192) //Из Кб в биты

//Выработка 'случайного' числа усилением встроенного генератора C++
unsigned __int16 get_rand()
{
	srand(time(0));
	unsigned int tmp = rand(), res = 0;

	for (int i = 0; i < 16; ++i)
	{
		res |= (tmp & 1) << i;
		tmp = rand();
	}

	return res;
}

unsigned __int16 get_key(char* fKey)
{
	ifstream in(fKey);

	char* strKey = new char[6];
	strKey[0] = '0';
	strKey[1] = 'x';

	for (int i = 2; i < 6; i++)
		in.get(strKey[i]);

	in.close();

	return strtoul(strKey, NULL, 0);;
}

//Функция вычисления четности вектора
unsigned __int16 get_parity(unsigned __int16 x)
{
	x ^= (x >> 8);
	x ^= (x >> 4);
	x ^= (x >> 2);
	x ^= (x >> 1);
	return (x & 1);
}

//Функция сдвига регистра
void reg_shift(unsigned __int16 &regs, unsigned __int16 fb_polynoms, unsigned __int16 reg_masks)
{
	regs = ((regs << 1) | (get_parity(regs & fb_polynoms))) & reg_masks;
	return;
}

//Функция инициализации регистров
void gen_init(unsigned __int16 *regs, unsigned __int16 *reg_masks, unsigned __int16 key)
{
	unsigned int init_key = 0xfed4e850;

	regs[0] = (Midori16(3, init_key, key)) & reg_masks[0];
	for (unsigned int i = 2; i < reg_num; ++i)
		regs[i] = (Midori16(3, init_key, (key * regs[i-1]) & 0xffff )) & reg_masks[i];

	return;
}

//Функция получения бита гаммы
unsigned int gen_shift(unsigned __int16 *regs, unsigned __int16 *fb_polynoms, unsigned __int16 *reg_masks)
{
	unsigned __int16 i, gamma_bit, arg, vote;

	//Нелинейная составляющая
	for (i = 0; i < reg_num; ++i)
		arg |= ((regs[i] & 1) << i);
	gamma_bit = (0xeca0 >> arg) & 1; //0xeca0 - бент-функция от 4 переменных; 7 единиц 9 нулей

	//Линейная составляющая
	for (i = 0; i < reg_num; ++i)
		gamma_bit ^= (regs[i] >> ((3 * i) & 0x3ff)) & 1;

	//Сдвиг
	for (i = 0; i < reg_num; ++i)
	{
		//1 этап
		arg = ((regs[(i + 1) % reg_num] >> 1) & 1) |
			(((regs[(i + 2) % reg_num] >> 2) & 1) << 1) |
			(((regs[(i + 3) % reg_num] >> 3) & 1) << 2);

		vote = (0xe8 >> arg) & 1;

		for (unsigned int k = 0; k < vote + 1; ++k)
			reg_shift(regs[i], fb_polynoms[i], reg_masks[i]);

		//2 этап
		arg = ((regs[(i + 4) % reg_num] >> 4) & 1) |
			(((regs[(i + 3) % reg_num] >> 3) & 1) << 1) |
			(((regs[(i + 2) % reg_num] >> 2) & 1) << 2);

		vote = (0xe8 >> arg) & 1;

		regs[i] ^= vote ^ ((regs[(i + 1) % reg_num] >> 7) & 1);
	}

	return gamma_bit;
}

void getGamma(unsigned __int16 *regs, unsigned __int16 *fb_polynoms, unsigned __int16 *reg_masks, unsigned __int16 key, char* fOut, unsigned __int64 gamma_size_Kb)
{
	//Инициализация генератора
	gen_init(regs, reg_masks, key);

	//Открытие в файла в режиме предварительной очистки
	ofstream out;
	out.open(fOut, ios_base::trunc);

	//Выработка гаммы и запись в файл
	for (unsigned __int64 i = 0; i < gamma_size; ++i)
		out << (gen_shift(regs, fb_polynoms, reg_masks) & 1);

	out.close();
	cout << "Сделано." << endl;
	return;
}


/*
bool tmp;
unsigned __int64  count = 0;
for (unsigned __int64 i = 0; i < gamma_size; ++i)
{
	tmp = (gen_shift(regs, fb_polynoms, reg_masks) & 1);
	out << tmp;
	if (tmp)
		count++;
}
cout << (double)(count / (double)gamma_size) * 100 << endl;
*/