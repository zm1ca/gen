#include "functions.h"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "rus");

	//Инициализация константных значений генератора
	unsigned __int16 regs[reg_num];
	unsigned __int16 reg_masks[reg_num] = { 0xffff, 0x7fff, 0x1fff, 0x07ff }; //Соответствуют длинам регистров
	unsigned __int16 fb_polynoms[reg_num] =
	{
		0xd008, // x^16 + x^12 + x^3 + x + 1
		0x6000, // x^15 + x + 1
		0x1b00, // x^13+ x^4 + x^3 + x + 1
		0x500, // x^11 + x^2 + 1
	}; //Найти плотные многочлены!

	//Инициализация переменных значений генератора
	//Автоматические значения
	unsigned __int16 key = get_rand();
	char *fOut = "gamma.txt", *fKey = "встроенного генератора C++";
	unsigned __int64 gamma_size_Kb = default_gamma_size_Kb; // длина в Кб

	//Разбор аргументов командной строки  <exe> <size_Kb> <target_file> <key_file>
	if (argc >= 2)
	{
		gamma_size_Kb = atoi(argv[1]);
		
		if (argc >= 3)
			fOut = argv[2];

		if (argc == 4) //Указан файл с 16-битным ключом в формате hex
		{
			fKey = argv[3];
			key = get_key(fKey);
		}

		if (argc > 4)
		{
			cout << "Слишком много аргументов.\gamma_size_KbПрограмма завершает работу.\gamma_size_Kb";
			exit(EXIT_FAILURE);
		}
	}

	//Вывод заданных параметров работы
	cout << gamma_size_Kb << "Кб гаммы генерируются в " << fOut << ", с использованием ключа 0x" << std::hex << key << ", полученного из " << fKey << "." << endl;

	//Выработка гаммы в указанный файл
	getGamma(regs, fb_polynoms, reg_masks, key, fOut, gamma_size_Kb);

	return 0;
}