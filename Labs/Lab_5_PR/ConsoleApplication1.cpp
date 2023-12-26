#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <ctime>
#include <iomanip>
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
			int count = 0;
			int countstr = 0;

			while (fin)
			{
				string str;
				getline(fin, str, '\n');
				countstr = std::count(str.begin(), str.end(), ';');
				break;
			}
			fin.close();
			//cout << "Столбец: ";
			for (int i = 0; i < countstr+1; i++)
				cout << i+1 << setw(12);
			cout << setw(-12)<< left << endl;
			fin.open(file);
			while (fin)
			{
				string str;
				getline(fin, str, ';');
				tmp2 += str;
				if (!fin) break;
				std::cout <<left << str << ";" << setw(10);
				tmp2 += ";";
			}
			ofstream fout2("MyFileTmp.csv");
			fout2 << tmp2.substr(0, tmp2.size());
			fout2.close();
			fin.close();

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
				getline(fin, str, ';');
				if (!fin) break;

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
				tmp += ";";
			}
			//cout << tmp << endl;
			fin.close();

			ofstream fout("MyFile.csv");
			fout << tmp.substr(0, tmp.size());
			fout.close();

			for (int i = 0; i < countstr + 1; i++)
				cout << i + 1 << setw(12);
			cout << setw(-12) << left << endl;
			fin.open(file);
			while (fin)
			{
				string str;
				getline(fin, str, ';');
				tmp2 += str;
				if (!fin) break;
				std::cout << left << str << ";" << setw(10);
				tmp2 += ";";
			}

			std::cout << "\nВcего было заменено " << count << " слов.\n";
			time_t seconds = time(NULL);
			tm* timeinfo = localtime(&seconds);
			cout << "Текущее время и дата:" << asctime(timeinfo) << endl;
		}
		cout << "Введите 1 если хотите продолжить или 0 если не хотите" << endl;
		cin >> again;
	}

	system("pause");
	return 0;

}

