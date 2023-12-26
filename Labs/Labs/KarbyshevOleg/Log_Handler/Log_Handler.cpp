#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <clocale>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <time.h>
using namespace std;

void message()
{
    cout << "Выберите действие:" << endl;
    cout << "1 - Информация о IP адресах, запросах, кодах состояния." << endl;
    cout << "2 - Вывести часто встречаемый IP адрес." << endl;
    cout << "3 - Вывести часто посещаемый URL адрес." << endl;
    cout << "4 - Вывести временной промежуток(дата самой ранней записи - дата последней записи" << endl;
    cout << "q - Выйти" << endl;
    cout << "Введите номер действия: "; 
}

vector<string> extractLogsInformation()
{
    ifstream input("logs.txt");
    string line = "";
    vector<string> information;

    while (getline(input, line))
    {
        size_t start = line.find_first_of(' ');
        size_t end = line.find_last_of(' ');

        information.push_back(line.substr(start + 1, end - start));
    }

    return information;
}

void frequentlyEncounteredIpAddresses()
{
    ifstream input("logs.txt");
    string line = "";
    map<string, int> ipAddress;

    while (getline(input, line))
    {
        size_t start = line.find_first_of(' ');
        size_t end = line.find_first_of(']', start + 1);

        istringstream iss(line.substr(start + 1, end - start));
        string word;
        while (iss >> word)
        {
            ipAddress[word]++;
        }
    }

    int max = 0;
    vector<string> maxIPs;
    for (const auto& key : ipAddress)
    {
        if (max < key.second)
        {
            max = key.second;
            maxIPs.clear();
            maxIPs.push_back(key.first);
        }
        else if (max == key.second)
        {
            maxIPs.push_back(key.first);
        }
    }
    cout << "Самый(ые) часто встречаемый(ые) IP_Address клиента(ов): " << "\n";
    for (const auto& ip : maxIPs)
    {
        cout << ip << " : " << max << " посещений" << endl;
    }
}

void frequentlyVisitedURL()
{
    ifstream input("logs.txt");
    string line = "";
    map<string, int> url;

    while (getline(input, line))
    {
        size_t start = line.find_last_of(' ');
        size_t end = line.find_last_of(']');

        istringstream iss(line.substr(start + 1, end - start));
        string word = "";
        while (iss >> word)
        {
            url[word]++;
        }
    }

    int maxUrl = 0;
    vector<string> maxURLs;
    for (const auto& key : url)
    {
        if (maxUrl < key.second)
        {
            maxUrl = key.second;
            maxURLs.clear();
            maxURLs.push_back(key.first);
        }
        else if (maxUrl == key.second)
        {
            maxURLs.push_back(key.first);
        }
    }
    cout << "Самый(ые) часто встречаемый(ые) URL: " << "\n";
    for (const auto& url : maxURLs)
    {
        cout << url << " : " << maxUrl << endl;
    }
}

vector<string> parserTimestamp()
{
    ifstream input("logs.txt");
    string line = "";

    vector<string> dates;
    while (getline(input, line))
    {
        size_t end = line.find(']');
        dates.push_back(line.substr(1, end - 1));
    }
    return dates;
}

void getEarlyandLateDate(vector<string> dates)
{
    vector<tm> dateObjects;
   
    for (const auto& dateString : dates) {
        stringstream ss(dateString);
        ss >> get_time(&dateObjects.emplace_back(), "%Y-%m-%d_%H:%M:%S");
    }
    
    tm earliestDate = dateObjects[0];
    tm latestDate = dateObjects[0];

    for (const auto& date : dateObjects) 
    {
        if ((date.tm_year < earliestDate.tm_year) ||
            (date.tm_year == earliestDate.tm_year && date.tm_mon < earliestDate.tm_mon) ||
            (date.tm_year == earliestDate.tm_year && date.tm_mon == earliestDate.tm_mon && date.tm_mday < earliestDate.tm_mday) ||
            (date.tm_year == earliestDate.tm_year && date.tm_mon == earliestDate.tm_mon && date.tm_mday == earliestDate.tm_mday && date.tm_hour < earliestDate.tm_hour) ||
            (date.tm_year == earliestDate.tm_year && date.tm_mon == earliestDate.tm_mon && date.tm_mday == earliestDate.tm_mday && date.tm_hour == earliestDate.tm_hour && date.tm_min < earliestDate.tm_min) ||
            (date.tm_year == earliestDate.tm_year && date.tm_mon == earliestDate.tm_mon && date.tm_mday == earliestDate.tm_mday && date.tm_hour == earliestDate.tm_hour && date.tm_min == earliestDate.tm_min && date.tm_sec < earliestDate.tm_sec)) 
        {
            earliestDate = date;
        }

        if ((date.tm_year > latestDate.tm_year) ||
            (date.tm_year == latestDate.tm_year && date.tm_mon > latestDate.tm_mon) ||
            (date.tm_year == latestDate.tm_year && date.tm_mon == latestDate.tm_mon && date.tm_mday > latestDate.tm_mday) ||
            (date.tm_year == latestDate.tm_year && date.tm_mon == latestDate.tm_mon && date.tm_mday == latestDate.tm_mday && date.tm_hour > latestDate.tm_hour) ||
            (date.tm_year == latestDate.tm_year && date.tm_mon == latestDate.tm_mon && date.tm_mday == latestDate.tm_mday && date.tm_hour == latestDate.tm_hour && date.tm_min > latestDate.tm_min) ||
            (date.tm_year == latestDate.tm_year && date.tm_mon == latestDate.tm_mon && date.tm_mday == latestDate.tm_mday && date.tm_hour == latestDate.tm_hour && date.tm_min == latestDate.tm_min && date.tm_sec > latestDate.tm_sec))
        {
            latestDate = date;
        }
    }

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &earliestDate);
    cout << "Самая ранняя дата: " << buffer << std::endl;

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &latestDate);
    cout << "Самая поздняя дата: " << buffer << std::endl;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    char choice = 0;
    cout << "Добро пожаловать в обработчик логов!" << endl;
    
    while (choice != 'q')
    {
        message();
        cin >> choice;
        switch (choice)
        {
        default:

        case '1':
            cout << "Информация о IP адресах,запросах,кодах состояния: " << endl;
            for (string str : extractLogsInformation())
            {
                cout << str << "\n";
            }
            break;
        case '2':
            frequentlyEncounteredIpAddresses();
            break;
        case '3':
            frequentlyVisitedURL();
            break;
        case '4':
            getEarlyandLateDate(parserTimestamp());
            break;
        case 'q':
            break;
        }
    }
}