//#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <ctime>
#include <iomanip>
#include <vector>
using namespace std;

int main()
{
	setlocale(LC_ALL, "rus");
	bool again = 1;
	while (again == 1)
	{
		cout << "Введите формат файла txt или csv" << endl;
		string n;
		cin >> n;
		string file = "";
		if (n == "txt")
			file = "MyFile.txt";
		else if (n == "csv")
			file = "MyFile.csv";
		else
		{
			cout << "Формат не поддерживется\n";

		}

		ifstream fin;
		fin.open(file);

		if (n == "txt")
		{

			if (!fin.is_open())
			{
				std::cout << "Ошибка. Файл не найден." << endl;
			}
			else
			{
				std::cout << "Файл MyFile.txt найден..." << endl;

				string tmp; // Новый текст
				string tmp2; // Старый текст
				int count = 0;
				tmp = "";
				while (fin)
				{
					string str;
					getline(fin, str, '\n');
					tmp2 += str;
					if (!fin) break;

					std::cout << str << endl;
					tmp2 += "\n";
				}
				ofstream fout2("MyFileTmp.txt");
				fout2 << tmp2.substr(0, tmp2.size());
				fout2.close();


				string word_base = "";
				cout << endl << "Введите слово которое хотите заменить" << endl;
				SetConsoleCP(1251);
				getline(cin, word_base);
				getline(cin, word_base);
				string word_edit = "";
				SetConsoleCP(866);
				cout << "Введите новое слово" << endl;
				SetConsoleCP(1251);
				getline(cin, word_edit);
				SetConsoleCP(866);
				fin.close();
				std::cout << "\nТекст на выходе------------------------------------------:\n" << tmp << "\n";
				fin.open(file);
				while (fin)
				{
					string str;
					getline(fin, str, '\n');
					if (!fin) break;

					//std::cout << str << endl;

					int position = 0;

					while (position != -1)
					{
						int position_start = position;
						position = str.find(word_base, position);

						if (position != -1)
						{
							count++;

							tmp += str.substr(position_start, position - position_start);

							tmp += word_edit;
							position += word_base.size();
						}
						else
						{
							tmp += str.substr(position_start, position - position_start);
						}
					}
					tmp += "\n";
				}
				cout << tmp << endl;
				fin.close();

				ofstream fout("MyFile.txt");
				fout << tmp.substr(0, tmp.size());
				fout.close();

				std::cout << "\nВcего было заменено " << count << " слов.\n";
				time_t seconds = time(NULL);
				tm* timeinfo = localtime(&seconds);
				cout << "Текущее время и дата:" << asctime(timeinfo) << endl;
			}
		}
		if (n == "csv")
		{
			string tmp;
			string tmp2;
			int countstb = 0; // количество столбцов
			vector <int> countstr; // Строка


			while (fin)
			{
				string str;
				getline(fin, str, '\n');
				if (!fin) break;
				countstr.push_back(std::count(str.begin(), str.end(), ';'));
				countstb++;
			}
			fin.close();


			//countstr++;
			//cout << "Столбец: ";
			cout << setw(12);
			for (int i = 0; i < (countstr[0] + 1); i++)
				cout << i + 1 << setw(12);
			cout << setw(-12) << left << endl;
			fin.open(file);
			int stbvec = 0; // Столбец
			int strvec = 0; // Строка вектора
			string str;
			vector<vector<string>> vec(countstr[strvec]); // двумерный вектор
			for (strvec = 0; strvec < countstb; strvec++) // Создаёт матрицу str x stb
			{
				//getline(fin, str, '\n');
				for (stbvec = 0; stbvec <= countstr[strvec]; stbvec++)
				{
					if (stbvec == countstr[strvec])
						getline(fin, str, '\n');
					else
					{
						getline(fin, str, ';');
					}
					vec[strvec].push_back(str);
				}
			}

			for (strvec = 0; strvec < countstb; strvec++) // выводит матрицу
			{
				cout << setw(-12) << left << "Строка:" << strvec + 1 << " ";
				for (stbvec = 0; stbvec <= countstr[strvec]; stbvec++)
				{
					tmp2 += vec[strvec][stbvec];
					tmp2 += ";";
					cout << vec[strvec][stbvec] << setw(12);
				}
				tmp2 += "\n";
				cout << endl;
			}
			ofstream fout2("MyFileTmp.csv");
			fout2 << tmp2.substr(0, tmp2.size());
			fout2.close();
			fin.close();

			cout << endl << "Введите № строки" << endl;
			SetConsoleCP(1251);
			cin >> strvec;
			SetConsoleCP(866);
			cout << endl << "Введите № столбца" << endl;
			SetConsoleCP(1251);
			cin >> stbvec;
			SetConsoleCP(866);
			cout << endl << "Слово в ячейке" << endl;
			cout << vec[strvec - 1][stbvec - 1] << endl;
			cout << "Введите новое слово" << endl;
			string word_edit = "";
			SetConsoleCP(1251);
			getline(cin, word_edit);
			getline(cin, word_edit);
			SetConsoleCP(866);
			vec[strvec - 1][stbvec - 1] = word_edit;

			std::cout << "\nТекст на выходе------------------------------------------:\n" << tmp << "\n";

			cout << left << setw(12);
			for (int i = 0; i < countstr[0] + 1; i++)
				cout << i + 1 << setw(12);
			cout << setw(-12) << left << endl;

			for (strvec = 0; strvec < countstb; strvec++) // выводит матрицу
			{
				cout << setw(-12) << left << "Строка:" << strvec + 1 << " ";
				for (stbvec = 0; stbvec <= countstr[strvec]; stbvec++)
				{
					tmp += vec[strvec][stbvec];
					if (stbvec != countstr[strvec])
					tmp += ";";
					cout << vec[strvec][stbvec] << setw(12);
				}
				tmp += "\n";
				cout << endl;
			}

			ofstream fout("MyFile.csv");
			fout << tmp.substr(0, tmp.size());
			fout.close();

			std::cout << "\nБыла заменена ячейка" << " .\n";
			time_t seconds = time(NULL);
			tm* timeinfo = localtime(&seconds);
			cout << "Текущее время и дата:" << asctime(timeinfo) << endl;
		}
		cout << "Введите 1 если хотите продолжить или 0 если не хотите" << endl;
		cin >> again;
	}

	std::system("pause");
	return 0;

}