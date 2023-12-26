// ConsoleApplication1.cpp: определяет точку входа для консольного приложения.
//

//#include "stdafx.h"

#include <iostream>
#include <fstream>	//для файлов
#include <sstream> //для потока строк

#include <string> //строка
#include <vector>
#include <ctime> //для получения времени

#include <map> //словарь
#include <algorithm>

#include <cmath> //fmod

using namespace std;

/*чтобы С++ принимал русский язык из файла — нужно поменять
кодировку текстового файла на ANSI*/

//Вариант 5. Конвертер формата дат

//проверка на существующий формат
bool initial_format(int data, int option, int first_option, string status) {
	//string status;
	time_t clock;
	clock = time(0);

	ofstream log_in; //поток для записи
	log_in.open("log.txt", ios::app); //открыть лог-файл	

	if (data < first_option || data > option) {					//если такого варианта нет, то ошибка
		status = "[error]";
		log_in << ctime(&clock) << "initial_format " + status + " THE OPTION DOESN'T EXIST\n"; //записал в лог ошибку
		return 0;
	}
	else {
		//status = "[succesful]";

		//log_in << ctime(&clock) << "initial_format " + status + "\n";
		return 1;
	}
}

//если переданная строка состоит из цифр
bool is_digit(string data, string status) {
	ofstream log_in; //поток для записи
	log_in.open("log.txt", ios::app); //открыть лог-файл	

	time_t clock;
	clock = time(0);
	int check = 0;

	char digits[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };
	for (int i = 0; i < size(data); i++) {
		if (find(digits, digits + size(digits), data[i]) != end(digits)) {
			check++;
		}
	}
	if (check == size(data) && check != 0) {
		return 1;
	}
	else {
		status = "[error]";
		log_in << ctime(&clock) << "is digit " + status + " THE DATA IS NOT NUMERIC\n"; //записал в лог ошибку
		return 0;
	}
}


//определяет, месяц - число или слово
int check_month(string month, string status) {
	ofstream log_in; //поток для записи
	log_in.open("log.txt", ios::app); //открыть лог-файл	

	time_t clock;
	clock = time(0);

	string months_word[] = { "января", "февраля", "марта", "апреля", "мая", "июня", "июля", "августа", "сентября", "октября", "ноября", "декабря" };

	if (find(months_word, months_word + size(months_word), month) != end(months_word)) { //если месяц - слово
		/*status = "[succesful]";
		log_in << ctime(&clock) << "check_month " + status + "\n";*/
		return 1;
	}//если месяц - число (т.е. состоит только из цифр)
	else if (is_digit(month, status) != 0 && size(month) == 2) {
		if (stod(month) > 0 && stod(month) < 13) {
			/*status = "[succesful]";
			log_in << ctime(&clock) << "check_month " + status + "\n";*/
			return 2;
		}
	}
	else { //если не число и не слово, а состоит из сторонних символов
		status = "[error]";
		log_in << ctime(&clock) << "check_month " + status + " MONTH FORMAT ERROR\n"; //записал в лог ошибку
		return 0;
	}
}



//проверка на високосный год
bool leap_year(string year, string day, string status) {
	ofstream log_in; //поток для записи
	log_in.open("log.txt", ios::app); //открыть лог-файл	

	time_t clock;
	clock = time(0);

	if ((fmod(stod(year), 4) == 0 && fmod(stod(year), 100) != 0) || fmod(stod(year), 400) == 0) {
		if (stod(day) < 30) {
			/*status = "[succesful]";
			log_in << ctime(&clock) << "leap_year " + status + "\n";*/
			return 1;
		} //если дней 29 и меньше 
	}
	else {
		if (stod(day) < 29) {
			/*status = "[succesful]";
			log_in << ctime(&clock) << "leap_year " + status + "\n";*/
			return 1;
		} //если дней 28 и меньше
	}
	status = "[error]";
	log_in << ctime(&clock) << "leap_year " + status + " LEAP YEAR ERROR\n"; //записал в лог ошибку
	return 0;
}

//проверяет соответствия дня и месяца, длину месяца и дня
bool check_date(string month, string day, string year, string status) {
	ofstream log_in; //поток для записи
	log_in.open("log.txt", ios::app); //открыть лог-файл	

	time_t clock;
	clock = time(0);

	bool A, B;
	if (check_month(month, status) == 2 && size(day) == 2) { //если месяц - число и день - 2 символа
		int month_num = stod(month);
		if (month_num != 2) {
			A = ((month_num < 8 && month_num % 2 != 0) || (month_num > 7 && month_num % 2 == 0)) && (stod(day) < 32);
			B = ((month_num < 8 && month_num % 2 == 0) || (month_num > 7 && month_num % 2 != 0)) && (stod(day) < 31);
			if (A || B) { return 1; }
		}
		else {
			if (leap_year(year, day, status) == 1) { return 1; }
		}
	}
	else if (check_month(month, status) == 1) { //если месяц - слово
		string months_30[] = { "апреля", "июня", "сентября", "ноября" };
		string months_31[] = { "января", "марта", "мая", "июля", "августа", "октября", "декабря" };
		//если день от 1 до 9 и имеет 1 симол или от 10 и больше и имеет 2 символа
		if (((stod(day) > 0 && stod(day) < 10) && size(day) == 1) || ((stod(day) > 9) && size(day) == 2)) {
			if (month != "февраля") {
				bool A = (find(months_30, months_30 + size(months_30), month) != end(months_30) && stod(day) < 31);
				bool B = (find(months_31, months_31 + size(months_31), month) != end(months_31) && stod(day) < 32);
				if (A || B) { return 1; }
			}
			else {
				if (leap_year(year, day, status) == 1) { return 1; }
			}
		}
	}
	status = "[error]";
	log_in << ctime(&clock) << "check_date " + status + " DATE FORMAT ERROR\n"; //записал в лог ошибку
	return 0;
}


string convertation(int new_select, string year, string month, string day, string status) {
	ofstream log_in; //поток для записи
	log_in.open("log.txt", ios::app); //открыть лог-файл	

	time_t clock;
	clock = time(0);

	map<string, string>word_to_num{
		{ "января", "01" },{ "февраля", "02" },
		{ "марта", "03" },{ "апреля", "04" },
		{ "мая", "05" },{ "июня", "06" },
		{ "июля", "07" },{ "августа", "08" },
		{ "сентября", "09" },{ "октября", "10" },
		{ "ноября", "11" },{ "декабря", "12" }
	};

	map<int, string>num_to_word{
		{ 1, "января" },{ 2, "февраля" },
		{ 3, "марта" },{ 4, "апреля" },
		{ 5, "мая" },{ 6, "июня" },
		{ 7, "июля" },{ 8, "августа" },
		{ 9, "сентября" },{ 10, "октября" },
		{ 11, "ноября" },{ 12, "декабря" }
	};

	switch (new_select) {
	case 1:
		if (check_month(month, status) == 1) {
			month = word_to_num[month];
		}

		if (size(day) == 1) {
			day = "0" + day;
		}
		return day + "-" + month + "-" + year;
	case 2:
		if (check_month(month, status) == 1) {
			month = word_to_num[month];
		}
		if (size(day) == 1) {
			day = "0" + day;
		}
		return day + "/" + month + "/" + year;
	case 3:
		if (check_month(month, status) == 1) {
			month = word_to_num[month];
		}
		if (size(day) == 1) {
			day = "0" + day;
		}
		return year + "/" + month + "/" + day;
	case 4:
		if (check_month(month, status) == 1) {
			month = word_to_num[month];
		}
		if (size(day) == 1) {
			day = "0" + day;
		}
		return day + "." + month + "." + year;
	case 5:
		if (check_month(month, status) == 2) {
			month = num_to_word[stod(month)];
		}
		if (size(day) == 2 && stod(day) < 10) {
			day = day.erase(0, 1);
		}
		return day + " " + month + " " + year;
	}
}


string parse_date(string date, int old_select, int new_select, string status) {
	ofstream log_in; //поток для записи
	log_in.open("log.txt", ios::app); //открыть лог-файл	

	time_t clock;
	clock = time(0);

	istringstream parse(date); //для разделения даты на день, месяц, год

	string day, month, year;

	bool flag = 0;

	switch (old_select) {
	case 1:
		if (count(date.begin(), date.end(), '-') == 2) {
			getline(parse, day, '-');
			getline(parse, month, '-');
			getline(parse, year);
			if (check_month(month, status) == 2) {
				flag = 1;
			}
		}
		break;
	case 2:
		if (count(date.begin(), date.end(), '/') == 2) {
			getline(parse, day, '/');
			getline(parse, month, '/');
			getline(parse, year);
			if (check_month(month, status) == 2) {
				flag = 1;
			}
		}
		break;
	case 3:
		if (count(date.begin(), date.end(), '/') == 2) {
			getline(parse, year, '/');
			getline(parse, month, '/');
			getline(parse, day);
			if (check_month(month, status) == 2) {
				flag = 1;
			}
		}
		break;
	case 4:
		if (count(date.begin(), date.end(), '.') == 2) {
			getline(parse, day, '.');
			getline(parse, month, '.');
			getline(parse, year);
			if (check_month(month, status) == 2) {
				flag = 1;
			}
		}
		break;
	case 5:
		if (count(date.begin(), date.end(), ' ') == 2) {
			getline(parse, day, ' ');
			getline(parse, month, ' ');
			getline(parse, year);
			if (check_month(month, status) == 1) {
				flag = 1;
			}
		}
		break;
	}

	if (flag == 1) { //если спарсено успешно
		if (is_digit(day, status) == 1 && is_digit(year, status) == 1) {
			if (check_date(month, day, year, status) == 1) {
				return convertation(new_select, year, month, day, status);
			}
		}
	}
	status = "[error]";
	log_in << ctime(&clock) << "check_date " + status + " UNABLE TO SPLIT A STRING\n"; //записал в лог ошибку
	return "-";

}

int main()
{
	setlocale(LC_ALL, "Russian"); //поддержка русского языка внутри программы

	string status;

	ifstream instruction("instruction.txt");
	string str;
	while (!instruction.eof()) {
		getline(instruction, str);
		cout << str << "\n";
	}

	cout << "Список доступных к выбору форматов дат:" << endl;

	//считывание доступных форматов с файла Date_format.txt
	ifstream format_out;									//поток ввода
	format_out.open("Date_format.txt");						//открывает файл Date_format.txt

	string variant;

	int option = 0; //счетчик для нумерации строк
	int first_option = option + 1; //первый статус счетчика — 1

	while (!format_out.eof()) {								//пока не конец файла
		option++;

		getline(format_out, variant); //получает строку
		cout << option << ' ' << variant << endl;
	}

	format_out.close();
	cout << "\n";

	//занести выбор в отедельную функцию
	int old_select, new_select;


	cout << "Укажите исходный формат даты.\nВведите номер варианта выбранного формата из списка выше: " << endl;
	cin >> old_select; //выбор исходного формата из списка
	cout << "\n";

	//проверка на существование формата. Если не существует — выбор идет заново
	while (initial_format(old_select, option, first_option, status) != 1) { //если формат не существует
		cout << "Такой вариант формата не существует. Введите снова.\n";
		cin >> old_select;
	}

	//выбор целевого формата
	cout << "Выберите формат, в который хотите перевести дату.\nВведите номер варианта выбранного формата из списка выше: " << endl;
	cin >> new_select;
	cout << "\n";


	while (initial_format(new_select, option, first_option, status) != 1) { //если формат не существует
		cout << "Такой вариант формата не существует. Введите снова.\n";
		cin >> new_select;
	}

	//открытие файла с датами на чтение
	ifstream dates_in;									//поток ввода
	dates_in.open("Dates.txt");
	vector <string> available_dates; //массив для хранения дат из файла

									 //считывание дат с файла
	string date;
	while (getline(dates_in, date)) {								//пока не конец файла
		available_dates.push_back(date);  //кладет в массив
	}
	dates_in.close();


	//открытие того же файла на запись дат
	ofstream dates_out;
	dates_out.open("Dates_result.txt");

	//Проверка и конвертация. И, если проверка успешна, запись конвертированной даты в файл
	for (int i = 0; i < size(available_dates); i++) {
		dates_out << available_dates[i] << " -> " << parse_date(available_dates[i], old_select, new_select, status) << "\n";
	}
	dates_out.close();


	ofstream log_in; //поток для записи
	log_in.open("log.txt", ios::app); //открыть лог-файл
									  //отделяет запуски программы
	log_in << "=============================================================\n";

	log_in.close();

	cout << "Даты конвертированы!\n";

	system("pause");
	return 0;
}