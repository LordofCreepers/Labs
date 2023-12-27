#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <sstream>


using namespace std;

struct Task
{
    string description;
    int priority;
    string deadline;
    string group;
    bool completed;
};


void addTask(vector<Task>& tasks_vector) //функция добавления задачи
{

    Task newTask; //создаем объект струкутры newTask
    cout << "Введите описание задачи: ";
    SetConsoleCP(1251); //временно меняем кодировку чтобы записывать символы из кирилицы
    getline(cin >> ws, newTask.description); //функция ws (white space) используется для игнорирования начальных пробельных символов, таких как пробелы, табуляции и переводы строки, 
    // которые могут быть оставлены после предыдущих операций ввода.
    SetConsoleCP(866);
    cout << "Введите приоритет задачи (1-3): ";
    cin >> newTask.priority;
    cout << "Введите срок выполнения: ";
    cin >> newTask.deadline;
    cout << "Введите группу задач (personal/home/work): ";
    cin >> newTask.group;
    newTask.completed = false; //по умолчанию задача является не выполненой
    tasks_vector.push_back(newTask); //добавляем объект с данными в вектор 
}

void removeTask(vector<Task>& tasks_vector, int taskNumber) //функция удаления задачи 
{
    if (taskNumber >= 1 && taskNumber <= tasks_vector.size())
    {
        tasks_vector.erase(tasks_vector.begin() + taskNumber - 1);
        cout << "Задание " << taskNumber << " было удалено." << endl;
    }
    else
    {
        cout << "Неправильный номер задания!" << endl;
    }
}
//функция отметки о выполнении задачи
void markAsCompleted(vector<Task>& tasks_vector, int taskNumber)
{
    if (taskNumber >= 1 && taskNumber <= tasks_vector.size())
    {
        tasks_vector[taskNumber - 1].completed = true;
        cout << "Задание " << taskNumber << " было помечено как выполненое." << endl;
    }
    else {
        cout << "Неправильный номер задания!" << endl;
    }
}
//функция сохранения задач в файл
void saveTasksToFile(const vector<Task>& tasks_vector, const string& fileName)
{
    ofstream file(fileName);
    if (file.is_open())
    {
        for (const Task& task : tasks_vector) {
            file << task.description << "," << task.priority << "," << task.deadline << "," << task.group << "," << task.completed << endl;
        }
        file.close();
    }
    else {
        cout << "Ошибка открытия файла для записи!" << endl;
    }
}
//функция извлечения информации о задачах из файла
void loadTasksFromFile(vector<Task>& tasks, const string& fileName) {
    ifstream file(fileName);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            istringstream iss(line);// читает строки из файла, используя getline, и затем разбивает каждую строку на подстроки с помощью istringstream и оператора извлечения
            string description; // это часть строки, содержащая описание задачи.
            string priorityStr; // это часть строки, содержащая информацию о приоритете задачи в виде строки.
            string deadline; // это часть строки, содержащая информацию о крайнем сроке выполнения задачи.
            string group; // это часть строки, содержащая информацию о группе, к которой относится задача.
            string completedStr; // это часть строки, содержащая информацию о статусе завершения задачи в виде строки. 
            // Каждый вызов getline извлекает следующую подстроку из iss (которая представляет исходную строку) до разделителя , и сохраняет ее в соответствующей переменной (description, priorityStr, deadline, group, completedStr). 
            // Затем создается объект task (представляющий задачу), и значения извлеченных подстрок присваиваются соответствующим полям этого объекта. 
            if (getline(iss, description, ',') &&
                getline(iss, priorityStr, ',') &&
                getline(iss, deadline, ',') &&
                getline(iss, group, ',') &&
                getline(iss, completedStr, ','))
            {
                Task task;
                task.description = description;
                task.priority = stoi(priorityStr);
                task.deadline = deadline;
                task.group = group;
                task.completed = (completedStr == "1");

                tasks.push_back(task);

            }
        }
        file.close();
    }
}
// функция вывода задач на консоль
void displayTasks(const vector<Task>& tasks_vector, int user_day, int user_month, int user_year)
{

    int choice_displayTasks;
    cout << "Если вы хотите вывести все задачи нажмите 1" << endl;
    cout << "Если вы хотите вывести задачи по приоритетам нажмите 2" << endl;
    cout << "Если вы хотите вывести задачи по группам нажмите 3" << endl;
    cout << "Если вы хотите вывести задачи по дате нажмите 4" << endl;
    cin >> choice_displayTasks;

    if (choice_displayTasks == 1)
    {
        cout << "Задачи:" << endl;
        int taskNumber = 1;
        for (const Task& task : tasks_vector) //из вектора tasks_vector выводятся значения на консоль
        {
            cout << "Задача " << taskNumber << ": " << task.description << ", Приоритет: " << task.priority
                << ", Срок выполнения: " << task.deadline << ", Группа: " << task.group
                << ", Выполнено: " << (task.completed ? "Да" : "Нет") << endl;
            taskNumber++;
        }
    }
    if (choice_displayTasks == 2)
    {
        int taskNumber1 = 1; //выводятся задачи с приоритетом 1
        for (const Task& task : tasks_vector)
        {
            if (task.priority == 1)
                cout << "Задача: " << taskNumber1 << ": " << task.description << ", Приоритет: " << task.priority
                << ", Срок выполнения: " << task.deadline << ", Группа: " << task.group
                << ", Выполнено: " << (task.completed ? "Да" : "Нет") << endl;
            taskNumber1++;

        }
        int taskNumber2 = 1; //выводятся задачи с приоритетом 2
        for (const Task& task : tasks_vector)
        {
            if (task.priority == 2)
                cout << "Задача: " << taskNumber2 << ": " << task.description << ", Приоритет: " << task.priority
                << ", Срок выполнения: " << task.deadline << ", Группа: " << task.group
                << ", Выполнено: " << (task.completed ? "Да" : "Нет") << endl;
            taskNumber2++;

        }
        int taskNumber3 = 1; //выводятся задачи с приоритетом 3
        for (const Task& task : tasks_vector)
        {
            if (task.priority == 3)
                cout << "Задача: " << taskNumber3 << ": " << task.description << ", Приоритет: " << task.priority
                << ", Срок выполнения: " << task.deadline << ", Группа: " << task.group
                << ", Выполнено: " << (task.completed ? "Да" : "Нет") << endl;
            taskNumber3++;

        }
    }
    if (choice_displayTasks == 3) //вывод по группам
    {
        string choice_displayTasks_group;
        cout << "Введите название группы, чтобы вывести все задачи из нее" << endl;
        cin >> choice_displayTasks_group;


        int taskNumber4 = 1;
        cout << "Задачи из группы personal:" << endl;
        for (const Task& task : tasks_vector)
        {

            if (task.group == "personal" && choice_displayTasks_group == "personal")
                cout << "Задача: " << taskNumber4 << ": " << task.description << ", Приоритет: " << task.priority
                << ", Срок выполнения: " << task.deadline << ", Группа: " << task.group
                << ", Выполнено: " << (task.completed ? "Да" : "Нет") << endl;
            taskNumber4++;

        }
        cout << "==========================" << endl;

        int taskNumber5 = 1;
        cout << "Задачи из группы home:" << endl;
        for (const Task& task : tasks_vector)
        {
            if (task.group == "home" && choice_displayTasks_group == "home")
                cout << "Задача: " << taskNumber5 << ": " << task.description << ", Приоритет: " << task.priority
                << ", Срок выполнения: " << task.deadline << ", Группа: " << task.group
                << ", Выполнено: " << (task.completed ? "Да" : "Нет") << endl;
            taskNumber5++;

        }
        cout << "==========================" << endl;

        int tsakNumber6 = 1;
        cout << "Задачи из группы work:" << endl;
        for (const Task& task : tasks_vector)
        {

            if (task.group == "work" && choice_displayTasks_group == "work")
                cout << "Задача: " << tsakNumber6 << ": " << task.description << ", Приоритет: " << task.priority
                << ", Срок выполнения: " << task.deadline << ", Группа: " << task.group
                << ", Выполнено: " << (task.completed ? "Да" : "Нет") << endl;
            tsakNumber6++;

        }
        cout << "==========================" << endl;




    }
    if (choice_displayTasks == 4)
    {
        cout << "Просроченные задачи" << endl;
        int taskNumber7 = 1;
        for (const Task& task : tasks_vector)
        {
            //условие: просрочено ли задание (и не является выполненым одновременно)
            if ((!((((stoi((task.deadline).substr(6, 4))) > user_year)) ||
                (((stoi((task.deadline).substr(6, 4))) == user_year) && (stoi((task.deadline).substr(3, 2)) > user_month)) ||
                (((stoi((task.deadline).substr(6, 4))) == user_year) && (stoi((task.deadline).substr(3, 2)) == user_month) && (stoi((task.deadline).substr(0, 2)) > user_day)))) && task.completed == 0)


                cout << "Задача " << taskNumber7 << ": " << task.description << ", Приоритет: " << task.priority
                << ", Срок выполнения: " << task.deadline << ", Группа: " << task.group
                << ", Выполнено: " << (task.completed ? "Да" : "Нет") << endl;
            taskNumber7++;
        }

    }
}

int main()
{
    setlocale(LC_ALL, "RU");
    vector<Task> tasks_vector; // вектор, содержащий элементы структуры Task
    string fileName = "tasks.txt";
    loadTasksFromFile(tasks_vector, fileName);

    string user_date; // дата, введенная пользователем

    cout << "Введите текущую дату (DD.MM.YYYY)" << endl;
    cin >> user_date;
    // вычисление и преобразование введённой даты
    int user_day, user_month, user_year;
    user_day = stoi(user_date.substr(0, 2));
    user_month = stoi(user_date.substr(3, 2));
    user_year = stoi(user_date.substr(6, 4));
    if ((user_month < 1 || user_month>12) || (user_day < 1 || user_day>31))
        cout << "Ошибка при введении данных!" << endl;
    else
    {
        int choice;
        do {
            cout << "|||||||||||MENU|||||||||||" << endl;
            cout << "1. Добавить новое задание" << endl;
            cout << "2. Удалить задание" << endl;
            cout << "3. Пометить задание как выполненое" << endl;
            cout << "4. Вывести задания" << endl;
            cout << "5. Сохранить все задачи в файл и выйти" << endl;
            cout << "Выберите вариант: " << endl;
            cout << "||||||||||||||||||||||||||" << endl;
            cin >> choice;

            switch (choice)
            {
            case 1:
                addTask(tasks_vector);
                break;
            case 2:
                int taskNumberToRemove;
                cout << "Введите номер задачи для удаления: ";
                cin >> taskNumberToRemove;
                removeTask(tasks_vector, taskNumberToRemove);
                break;
            case 3:
                int taskNumberToComplete;
                cout << "Введите номер задачи для отметки 'Выполнено': ";
                cin >> taskNumberToComplete;
                markAsCompleted(tasks_vector, taskNumberToComplete);
                break;
            case 4:
                displayTasks(tasks_vector, user_day, user_month, user_year);
                break;
            case 5:
                saveTasksToFile(tasks_vector, fileName);
                cout << "Задачи были сохранены в  " << fileName << endl;
                system("pause");
                break;
            default:
                cout << "Нерпавильный ввод" << endl;
            }
        } while (choice != 5); // программа работает с пользователем, пока он не введет 5 (сохранение и выход)
    }


    return 0;
}
