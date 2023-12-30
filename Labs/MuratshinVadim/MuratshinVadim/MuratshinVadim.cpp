//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <utility>
#include <vector>
#include <algorithm>  
#include <windows.h>  
#include <conio.h>
#include <stdio.h>


//ascii верхний регистр латинских букв начинается с 65  и заканчивается на 90  включительно
//ascii нижний  регистр латинских букв начинается с 97  и заканчивается на 122 включительно
//ascii верхний регистр русских   букв начинается с -64 и заканчивается на -33 включительно
//ascii нижний  регистр русских   букв начинается с -32 и заканчивается на -1  включительно

using namespace std;

//функция на определение слова без знаков
pair<string, pair<long unsigned int, int>> ifWord(string& text, long unsigned int iBegin) {
	if (!(text[iBegin] >= 65 && text[iBegin] <= 90) && !(text[iBegin] >= 97 && text[iBegin] <= 122) && !(text[iBegin] >= -64 && text[iBegin] <= -33) && !(text[iBegin] >= -32 && text[iBegin] <= -1)) {
		return make_pair("", make_pair(0, -1));
	}
	else {
		string s = "";
		long unsigned int iEnd = iBegin;
		while (iEnd != text.size() && ((text[iEnd] >= 65 && text[iEnd] <= 90) || (text[iEnd] >= 97 && text[iEnd] <= 122) || (text[iEnd] >= -64 && text[iEnd] <= -33) || (text[iEnd] >= -32 && text[iEnd] <= -1))) {
			s += text[iEnd];
			iEnd++;
		}
		return make_pair(s, make_pair(iEnd, 0));
	}
}

void symbolAnalys(string& txt);

void biggestCountWords(map<string, long unsigned int>& words);

void symbolCount(map<string, long unsigned int>& words, string& txt);

void sizeWordsStat(map<string, long unsigned int>& words);

void wordsList(map<string, long unsigned int>& words);

map<string, long unsigned int> wordsAnalys(string& txt);

void interfaceMain(const string& path, map<string, long unsigned int>& words, string& txt, bool stopWord, string stopStr);

using namespace std;

int main()
{
	setlocale(LC_ALL, "Russian");

	string a;
	cout << "Введите путь до файла(вы можете ввести \"0\", чтобы выйти из программы): ";
	SetConsoleCP(1251);
	getline(cin, a);
	if (a == "0")
		return 0;
	const string path = a;
	ifstream fin(path);
	string proizvedenie;
	string txt = "";
	bool fileAvailable = 0;
	while (getline(fin, proizvedenie)) {
		fileAvailable = 1;
		txt += proizvedenie;
	}
	SetConsoleCP(866);
	if (!fileAvailable) {
		system("cls");
		main();
	}

	map<string, long unsigned int> words = wordsAnalys(txt);
	interfaceMain(path, words, txt, 0, "");
	system("pause");
	return 0;
}

map<string, long unsigned int> wordsAnalys(string& txt) {
	map<string, long unsigned int> words;
	pair<string, pair<long unsigned int, int>> tmpIfWord;
	for (long unsigned int i = 0; i < txt.size(); i++) {
		tmpIfWord = ifWord(txt, i);
		if (tmpIfWord.second.second != -1) {
			words[tmpIfWord.first] += 1;
			i = long unsigned int(tmpIfWord.second.first);
		}
	}
	return words;
}

void biggestCountWords(map<string, long unsigned int>& words) {
	system("cls");
	vector <string> indexOfManyWords;
	for (int y = 0; y < min(10, words.size()); y++) {
		long unsigned int maxxWordNumber = 0;
		string id = "";
		for (const auto& word : words) {
			if (word.second > maxxWordNumber && count(indexOfManyWords.begin(), indexOfManyWords.end(), word.first) == 0) {
				maxxWordNumber = word.second;
				id = word.first;
			}
		}
		indexOfManyWords.push_back(id);
	}

	for (const auto& i : indexOfManyWords)
		cout << i << "\t" << words[i] << endl;
	cout << "Нажмите любую кнопку чтобы продолжить" << endl;
	while (!_kbhit()) {}
}

void symbolCount(map<string, long unsigned int>& words, string& txt) {
	system("cls");
	cout << "Общее количество символов: " << txt.size() << endl;
	cout << "Нажмите любую кнопку чтобы продолжить" << endl;
	while (!_kbhit()) {}
}

void sizeWordsStat(map<string, long unsigned int>& words) {
	system("cls");
	map<string, long unsigned int> statistic;
	string tmps;
	for (const auto& word : words) {
		if (word.first.size() < 5)
			statistic["0Слова с длинной от 1 до 5 символов: "] += word.second;
		else
			statistic[to_string(int(word.first.size() / 5)) + "Слова с длинной от " + to_string(int(word.first.size() / 5) * 5) + " до " + to_string(int(word.first.size() / 5) * 5 + 5) + " символов: "] += word.second;
	}
	for (const auto& stat : statistic) {
		tmps = stat.first;
		tmps.erase(tmps.begin(), tmps.begin() + 1);
		cout << tmps << stat.second << endl;
	}
	cout << "Нажмите любую кнопку чтобы продолжить" << endl;
	while (!_kbhit()) {}
}

void wordsList(map<string, long unsigned int>& words) {
	system("cls");
	const string path = "wordList.txt";
	ofstream output(path);
	for (const auto& word : words) {
		if (word.second == 1) {
			cout << word.first << endl;
			output << word.first << endl;
		}
	}
	output.close();
	cout << "Нажмите любую кнопку чтобы продолжить" << endl;
	system("wordList.txt");
	while (!_kbhit()) {}
}

void stopWordFun(const string& path, map<string, long unsigned int>& words, string& txt) {
	system("cls");
	string txt2 = txt;
	long unsigned int n;
	cout << "Введите количество стоп-слов: ";
	cin >> n;
	cout << endl;
	vector <string> stopVec;
	string tmps;
	string stpstr = "";
	for (long unsigned int i = 0; i < n; i++) {
		cout << "Введите " << to_string(i + 1) << " стоп-слово: ";
		SetConsoleCP(1251);
		cin >> tmps;
		stopVec.push_back(tmps);
		stpstr += tmps + " ";
		SetConsoleCP(866);
	}
	map<string, long unsigned int> words2 = wordsAnalys(txt2);
	for (auto i : stopVec) {
		words2.erase(i);
	}
	SetConsoleCP(1251);
	interfaceMain(path, words2, txt2, 1, stpstr);
}

void symbolAnalys(string& txt) {
	system("cls");
	const string path = "symbolAnalys.txt";
	ofstream output(path);
	map<char, int> symbols;
	for (char i : txt) {
		symbols[i]++;
	}
	for (const auto& symbol : symbols) {
		cout << symbol.first << "\t" << symbol.second << endl;
		output << symbol.first << "\t" << symbol.second << endl;
	}
	output.close();
	cout << "Нажмите любую кнопку чтобы продолжить" << endl;
	system("symbolAnalys.txt");
	while (!_kbhit()) {}
}

void interfaceMain(const string& path, map<string, long unsigned int>& words, string& txt, bool stopWord, string stopStr) {
	SetConsoleCP(866);
	system("cls");
	vector <string> a;
	if (stopWord) {
		cout << "Режим стоп-слова" << endl;
		cout << "Список стоп-слов: " << stopStr << endl;
	}
	cout << "Путь к вашему файлу: " << path << "\n" << endl;
	cout << "Список операций:\n";
	cout << "1) Анализ частоты слов. Подсчет и вывод наиболее часто встречающихся слов в тексте.\n";
	cout << "2) Подсчет количества символов. Анализ общего количества символов в тексте, включая пробелы и знаки препинания.\n";
	cout << "3) Статистика по длине слов. Распределение слов по длине и вывод статистики(например, сколько слов с длиной менее 5 символов, сколько от 5 до 10 и так далее).\n";
	cout << "4) Анализ уникальных слов. Подсчет количества уникальных слов в тексте.\n";
	if (stopWord)
		cout << "5) Вернуться в обычный режим\n";
	else
		cout << "5) Удаление слов. Удаление слов из текста перед анализом статистики.\n";
	cout << "6) Список символов. Выводит список символов в файле и их количество.\n";
	int op;
	cout << "Введите номер операции: ";
	cin >> op;
	if (op < 1 || op>6) {
		system("cls");
		cout << "Такой операции нет\n" << endl;
		interfaceMain(path, words, txt, stopWord, stopStr);
	}
	else if (!stopWord) {
		switch (op) {
		case(1): biggestCountWords(words); interfaceMain(path, words, txt, 0, ""); break;
		case(2): symbolCount(words, txt); interfaceMain(path, words, txt, 0, ""); break;
		case(3): sizeWordsStat(words); interfaceMain(path, words, txt, 0, ""); break;
		case(4): wordsList(words); interfaceMain(path, words, txt, 0, ""); break;
		case(5): stopWordFun(path, words, txt); interfaceMain(path, words, txt, 0, ""); break;
		case(6): symbolAnalys(txt); interfaceMain(path, words, txt, 0, ""); break;
		default: break;
		}
	}
	else if (stopWord) {
		switch (op) {
		case(1): biggestCountWords(words); interfaceMain(path, words, txt, 1, stopStr); break;
		case(2): symbolCount(words, txt); interfaceMain(path, words, txt, 1, stopStr); break;
		case(3): sizeWordsStat(words); interfaceMain(path, words, txt, 1, stopStr);  break;
		case(4): wordsList(words); interfaceMain(path, words, txt, 1, stopStr);  break;
		case(5): main(); break;
		case(6): symbolAnalys(txt); interfaceMain(path, words, txt, 1, stopStr);  break;
		default: break;
		}
	}
}
