#include <iostream>
#include <fstream>
#include <locale>
#include <map>
#include <unordered_set>
#include <vector>
#include <string>
#include <sstream>
// #include <thread>
// #include <mutex>

#ifdef BENCH
#include <chrono>
#endif

#include <functional>
#include <cwctype>
#include <Windows.h>

// #define DEBUG_INPUT_FILEPATH "idk_some_test_text.txt"
#define DEBUG_DICT_FILEPATH "!"
#define DEBUG_OUTPUT_FILEPATH "analysis.txt"

// #define FILE_READ_ENCODING_CONVERT

// #define BENCH

#ifdef BENCH
void bench(const std::function<void()>& func, std::ostream& output)
{
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

	func();

	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

	output << "Elapsed: " << (end - start).count() * 1e-9 << " s.";
}
#endif // BENCH

#ifdef FILE_READ_ENCODING_CONVERT
std::codecvt_base::result decodingStringToWstring(const std::string& in, const std::string& localeName, std::wstring& out) {
	std::locale loc(localeName);
	const std::codecvt<wchar_t, char, mbstate_t>& f = std::use_facet<std::codecvt<wchar_t, char, mbstate_t>>(loc);
	mbstate_t mb;
	out = std::wstring(in.size(), '\0');
	const char* from_next;
	wchar_t* to_next;

	std::codecvt_base::result res = f.in(mb, &in[0], &in[in.size()], from_next, &out[0], &out[out.size()], to_next);
	out.resize(to_next - &out[0]);

	return res;
}

std::codecvt_base::result encodingWstringToString(const std::wstring& in, const std::string& localeName, std::string& out)
{
	std::locale loc(localeName);
	const std::codecvt<wchar_t, char, mbstate_t>& f = std::use_facet<std::codecvt<wchar_t, char, mbstate_t>>(loc);
	mbstate_t mb;
	out = std::string(in.size(), '\0');
	char* to_next;
	const wchar_t* from_next;

	std::codecvt_base::result res = f.out(mb, &in[0], &in[in.size()], from_next, &out[0], &out[out.size()], to_next);
	out.resize(to_next - &out[0]);

	return res;
}
#endif // FILE_READ_ENCODING_CONVERT

bool FileToString(std::ifstream& file, std::string& out)
{
	file.seekg(0, std::ios::end);

	size_t file_char_count = file.tellg();

	if (file_char_count > 0)
	{
		file.seekg(0, std::ios::beg);

		out.resize(file_char_count);;

		file.read(&out[0], out.size());

		return true;
	}
	else
		return false;
}

void ProcessText(
	const char* start,
	const char* end,
	const std::function<bool (char)>& is_delimiter = nullptr,
	const std::function<void (char)>& processed_char = nullptr,
	const std::function<void (const std::string&)>& processed_word = nullptr
) {
	const char* after_last_delim = start;
	for (const char* i = start; i < end; i++)
	{
		processed_char(*i);

		if (is_delimiter ? is_delimiter(*i) : !std::isalnum(*i))
		{
			if (i - after_last_delim > 0)
				processed_word({ after_last_delim, static_cast<size_t>(i - after_last_delim) });
			
			after_last_delim = i + 1;
		};
	}
}

void Analyze(
	const char* start,
	const char* end,
	std::map<char, uint64_t>& char_count,
	std::map<std::string, uint64_t>& word_count,
	std::map<uint64_t, uint64_t>& word_count_by_length,
	const std::function<bool (char)>& is_delimiter = nullptr,
	const std::function<bool (const std::string&)>& is_stop_word = nullptr
) {
	ProcessText(start, end, is_delimiter,
		[&char_count, &is_stop_word](char ch)
		{
			if (!is_stop_word || !is_stop_word({ 1, ch }))
				char_count[ch]++;
		},
		[&char_count, &word_count, &word_count_by_length, &is_stop_word](const std::string& word)
		{
			if (!is_stop_word || !is_stop_word(word))
			{
				word_count[word]++;
				word_count_by_length[word.size()]++;
			}
			else
				for (char ch : word)
					char_count[ch]--;
		}
	);
}

bool IsCharADelimiter(char ch)
{
	static const std::unordered_set<char> delims =
	{
		'.',
		',',
		':',
		';',
		'!',
		'?',
		'«',
		'»',
		'[',
		']',
		'=',
		' ',
		'(',
		')',
		'0',
		'1',
		'2',
		'3',
		'4',
		'5',
		'6',
		'7',
		'8',
		'9',
		'\r',
		'\t',
		'\0',
		'\n',
	};

	return delims.find(ch) != delims.cend();
}

void to_multibyte(const std::wstring& source, std::string& target)
{
	target.clear();
	target.resize(source.size());

	WideCharToMultiByte(CP_ACP, 0, source.c_str(), -1, &target[0], source.size(), 0, 0);
}

enum class OutputCriteria
{
	NONE = 0,
	CHARACTER_COUNT = 1,
	WORD_COUNT = 2,
	WORD_COUNT_BY_LENGTH = 4,
	UNIQUE_WORDS = 8
};

enum class OpResult
{
	SUCCESS,
	FAILURE,
	TERMINATE,
	ABORT
};

template<typename T>
struct TreeNode
{
	T Value;
	TreeNode* Parent;
	std::vector<TreeNode*> Children;

	TreeNode* Append(TreeNode* child)
	{
		child->Parent = this;

		Children.push_back(child);

		return child;
	}
};

struct Operation
{
	std::string Title;
	std::function<const TreeNode<Operation>* (const TreeNode<Operation>*, std::istream&, std::ostream&, OpResult&)> Function;
};

const TreeNode<Operation>* MoveToChildNodePrompt(
	const TreeNode<Operation>* node,
	std::istream& in, std::ostream& out,
	OpResult& op_code,
	bool can_move_up
) {
	for (size_t i = 0; i < node->Children.size(); i++)
		out << '\t' << (i + 1) << ") " << node->Children[i]->Value.Title << '\n';

	char digit_or_em;
	in >> digit_or_em;

	if (digit_or_em == '!' && can_move_up)
	{
		op_code = OpResult::ABORT;

		return node->Parent;
	}

	size_t id = digit_or_em - '1';

	if (id > node->Children.size())
	{
		out << "Узел под таким номером не найден...";

		op_code = OpResult::FAILURE;

		return node;
	};

	op_code = OpResult::SUCCESS;

	return node->Children[id];
}

const TreeNode<Operation>* ToggleCriteria(
	const TreeNode<Operation>* node, 
	std::istream& in, std::ostream& out,
	OpResult& op_code,
	OutputCriteria criteria_to_toggle,
	OutputCriteria& criteria_ref
) {
	const uint8_t criteria_mask = static_cast<uint8_t>(criteria_to_toggle);
	const uint8_t criteria_cur = static_cast<uint8_t>(criteria_ref);

	out << "'!' -> назад\n";
	out << "Состояние критерия: " << ((criteria_cur & criteria_mask > 0) ? "Включён" : "Выключен");
	out << '\n';
	out << "Введите '+', чтобы включить, '-' - чтобы выключить: ";

	char on_or_off;
	in >> on_or_off;

	if (on_or_off == '!') 
	{
		op_code = OpResult::ABORT;
		return node->Parent;
	}

	if (on_or_off == '+')
	{
		criteria_ref = static_cast<OutputCriteria>(criteria_cur | criteria_mask);

		out << "Критерий включён...";
		op_code = OpResult::SUCCESS;
		return node->Parent;
	}

	if (on_or_off == '-')
	{
		criteria_ref = static_cast<OutputCriteria>(criteria_cur & ~criteria_mask);

		out << "Критерий выключен...";
		op_code = OpResult::SUCCESS;
		return node->Parent;
	}

	out << "Параметр не был распознан. Разрешены только '+' и '-'";
	op_code = OpResult::FAILURE;
	return node;
}

const TreeNode<Operation>* DumpStatisticsToBuffer(
	const TreeNode<Operation>* node,
	std::istream& in, std::ostream& out,
	OpResult& op_code,
	const std::map<char, uint64_t>& char_count,
	const std::map<std::string, uint64_t>& word_count,
	const std::map<uint64_t, uint64_t>& word_count_by_length,
	size_t wbl_floor,
	size_t wbl_ceil,
	OutputCriteria criteria,
	std::ostream& dump_to,
	bool ansi_encode
) {
	std::string out_string;

	if (criteria == OutputCriteria::NONE)
	{
		if (ansi_encode)
			to_multibyte(L"Не выбрано критериев данных для вывода...", out_string);
		else
			out_string = "Не выбрано критериев данных для вывода...";

		out << out_string;
		op_code = OpResult::FAILURE;
		return node->Parent;
	}

	const uint8_t criteria_mask = static_cast<uint8_t>(criteria);

	if ((criteria_mask & static_cast<uint8_t>(OutputCriteria::CHARACTER_COUNT)) > 0)
	{
		if (ansi_encode)
			to_multibyte(L"Кол-во знаков:\n", out_string);
		else
			out_string = "Кол-во знаков:\n";

		dump_to << out_string;

		for (const std::pair<char, uint64_t>& ch : char_count)
		{
			switch (ch.first)
			{
				case ' ':
					if (ansi_encode)
						to_multibyte(L"\tПробел", out_string);
					else
						out_string = "\tПробел";
					
					dump_to << out_string << ": " << ch.second << '\n';
					break;
				case '\t':
					if (ansi_encode)
						to_multibyte(L"\tТабуляция", out_string);
					else
						out_string = "\tТабуляция";
					
					dump_to << out_string << ": " << ch.second << '\n';
					break;
				case '\n':
					break;
				case '\r':
					break;
				case '\0':
					break;
				default:
					dump_to << "\t\'" << ch.first << "': " << ch.second << '\n';
					break;
			}
		}

		dump_to << "\n\n";
	}

	if ((criteria_mask & static_cast<uint8_t>(OutputCriteria::WORD_COUNT)) > 0)
	{
		if (ansi_encode)
			to_multibyte(L"Кол-во слов:\n", out_string);
		else
			out_string = "Кол-во слов:\n";

		dump_to << out_string;

		for (const std::pair<std::string, uint64_t>& wrd : word_count)
			dump_to << "\t'" << wrd.first << "': " << wrd.second << '\n';
				
		dump_to << "\n\n";
	}

	if ((criteria_mask & static_cast<uint8_t>(OutputCriteria::WORD_COUNT_BY_LENGTH)) > 0) 
	{
		if (ansi_encode)
			to_multibyte(L"Кол-во слов длинной от ", out_string);
		else
			out_string = "Кол-во слов длинной от ";

		dump_to << out_string << wbl_floor;

		if (ansi_encode)
			to_multibyte(L" до ", out_string);
		else
			out_string = " до ";

		dump_to << out_string << wbl_ceil;

		if (ansi_encode)
			to_multibyte(L" знаков: ", out_string);
		else
			out_string = " знаков: ";

		dump_to << out_string;

		size_t sum = 0;

		for (size_t i = wbl_floor; i < wbl_ceil + 1; i++)
		{
			std::map<uint64_t, uint64_t>::const_iterator it = word_count_by_length.find(i);

			if (it == word_count_by_length.cend()) continue;

			sum += it->second;
		}
				
		dump_to << sum << "\n\n";
	}

	if ((criteria_mask & static_cast<uint8_t>(OutputCriteria::UNIQUE_WORDS)) > 0)
	{
		if (ansi_encode)
			to_multibyte(L"Уникальные слова:\n", out_string);
		else
			out_string = "Уникальные слова:\n";

		dump_to << out_string;
				
		for (
			std::map<std::string, uint64_t>::const_reverse_iterator unique_word = word_count.crbegin();
			unique_word != word_count.crend(); unique_word++
		) {
			if (unique_word->second > 1) break;

			dump_to << "\t'" << unique_word->first << "'\n";
		}

		dump_to << "\n\n";
	}

	op_code = OpResult::SUCCESS;
	return node->Parent;
}

int main()
{
	setlocale(LC_ALL, "Russian");

	std::string the_entire_file;

	std::unordered_set<std::string> stop_words_dictionary;

	TreeNode<Operation> pre_analysis;
	pre_analysis.Value = 
	{
		"...",
		[](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_code
		) -> const TreeNode<Operation>*
		{
			out << "'!' -> завершение программы\n";
			out << "Выберите операцию: \n";

			return MoveToChildNodePrompt(node, in, out, op_code, true);
		}
	};

	TreeNode<Operation> stop_words_dict_population;
	stop_words_dict_population.Value =
	{
		"Манипуляция словарём стоп-слов",
		[](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_details
		) -> const TreeNode<Operation>*
		{
			out << "'!' -> назад\n";
			out << "Выберите операцию над словарём стоп-слов:\n";

			return MoveToChildNodePrompt(node, in, out, op_details, true);
		}
	};

	TreeNode<Operation> dict_add_words;
	dict_add_words.Value =
	{
		"Добавление стоп-слов",
		[&stop_words_dictionary](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_code
		) -> const TreeNode<Operation>*
		{
			out << "'!' -> назад\n";
			out << "Введите с клавиатуры стоп-слово: ";

			SetConsoleCP(1251);
			std::string stop_word;
			in >> stop_word;
			SetConsoleCP(886);

			if (stop_word == "!")
			{
				char choice = 0;
				while (choice < '1' || choice > '2')
				{
					out << "1 - добавить '!' в список стоп-слов, 2 - завершить ввод стоп-слов: ";

					in >> choice;

					if (choice < '1' || choice > '2')
						out << "Выбранный пункт не совпадает ни с одним из предоставленных...\n";
				}

				if (choice == '2')
					return node->Parent;
			}

			if (stop_words_dictionary.find(stop_word) == stop_words_dictionary.cend())
			{
				stop_words_dictionary.insert(stop_word);

				out << "Стоп-слово '" << stop_word << "' было успешно добавлено в словарь!";
				op_code = OpResult::SUCCESS;
			}
			else
			{
				out << "Стоп-слово '" << stop_word << "' уже присутствует в словаре...";
				op_code = OpResult::FAILURE;
			}

			return node;
		}
	};

	TreeNode<Operation> dict_remove_words;
	dict_remove_words.Value =
	{
		"Удаление стоп-слов",
		[&stop_words_dictionary](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult op_code
		) -> const TreeNode<Operation>*
		{
			out << "'!' -> назад\n";
			out << "Введите с клавиатуры стоп-слово: ";

			SetConsoleCP(1251);
			std::string stop_word;
			in >> stop_word;
			SetConsoleCP(886);

			if (stop_word == "!")
			{
				char choice = 0;
				while (choice < '1' || choice > '2')
				{
					out << "1 - удалить '!' из списка стоп-слов, 2 - завершить ввод стоп-слов: ";

					in >> choice;

					if (choice < '1' || choice > '2')
						out << "Выбранный пункт не совпадает ни с одним из предоставленных...\n";
				}

				if (choice == '2')
					return node->Parent;
			}

			if (stop_words_dictionary.find(stop_word) != stop_words_dictionary.cend())
			{
				stop_words_dictionary.erase(stop_word);

				out << "Стоп-слово '" << stop_word << "' было успешно удалено из стоп-словаря!";
				op_code = OpResult::SUCCESS;
			}
			else
			{
				out << "Стоп-слово '" << stop_word << "' не было найдено в словаре...";
				op_code = OpResult::FAILURE;
			}

			return node;
		}
	};

	TreeNode<Operation> dict_flush;
	dict_flush.Value = 
	{
		"Очистить словарь",
		[&stop_words_dictionary](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult op_code
		) -> const TreeNode<Operation>*
		{
			out << "Данное действие полностью очистит словарь и необратимо\n";
			out << "'!' -> назад\n";
			out << "Введите что-то, кроме '!', чтобы подтвердить: ";

			char ch = 0;
			in >> ch;

			if (ch == '!')
			{
				op_code = OpResult::ABORT;

				return node->Parent;
			};
			
			stop_words_dictionary.clear();

			out << "Словарь очищен!";
			op_code = OpResult::SUCCESS;
			return node->Parent;
		}
	};

	TreeNode<Operation> load_dict_from_file;
	load_dict_from_file.Value =
	{
		"Чтение из файла",
		[&stop_words_dictionary](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_code
		) -> const TreeNode<Operation>*
		{
			out << "Данное действие перезапишет уже имеющиеся данные в словаре стоп-слов\n";
			out << "'!' -> назад\n";
			out << "Введите путь до файла-словаря стоп-слов: ";

			std::string filepath;
			in >> filepath;

			if (filepath[0] == '!')
			{
				op_code = OpResult::ABORT;
				return node->Parent;
			}

			std::ifstream dict_file(filepath);

			if (dict_file)
			{
				stop_words_dictionary.clear();

				out << "Начинается чтение...";

				std::string stop_word;

				while (dict_file >> stop_word)
					stop_words_dictionary.insert(stop_word);

				out << "Чтение завершено!";
				op_code = OpResult::SUCCESS;
				return node->Parent;
			}

			out << "Файл не существует, нечитаем или недоступен...";
			op_code = OpResult::FAILURE;
			return node;
		}
	};

	TreeNode<Operation> dump_dict;
	dump_dict.Value =
	{
		"Вывод содержимого стоп-словаря",
		[&stop_words_dictionary](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_code
		) -> const TreeNode<Operation>*
		{
			for (const std::string& word : stop_words_dictionary)
				out << '\'' << word << "' ";

			return node->Parent;
		}
	};

	TreeNode<Operation> write_dict_to_file;
	write_dict_to_file.Value =
	{
		"Запись содержимого стоп-словаря в файл",
		[&stop_words_dictionary](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_code
		) -> const TreeNode<Operation>*
		{
			out << "'!' -> назад\n";
			out << "Введите путь до файла: ";

			std::string filepath;
			in >> filepath;

			if (filepath[0] == '!')
			{
				op_code = OpResult::ABORT;
				return node->Parent;
			}

			std::ofstream dict_file(filepath);

			if (dict_file)
			{
				out << "Запись данных в файл...";

				for (const std::string& stop_word : stop_words_dictionary)
					dict_file << stop_word << ' ';

				out << "Данные успешно записаны!";
				op_code = OpResult::SUCCESS;
				return node->Parent;
			}

			out << "Путь был введён некорректно или нет доступа по данному пути...";
			op_code = OpResult::FAILURE;
			return node;
		}
	};

	TreeNode<Operation> post_analysis;
	post_analysis.Value = 
	{
		"...",
		[&the_entire_file](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_code
		) -> const TreeNode<Operation>*
		{
			out << "'!' -> завершить программу\n";
			out << "Выберите операцию над полученной статистикой\n";

			return MoveToChildNodePrompt(node, in, out, op_code, true);
		}
	};

	std::map<char, uint64_t> char_count;
	std::map<std::string, uint64_t> word_count;
	std::map<uint64_t, uint64_t> word_count_by_length;

	OutputCriteria criteria = OutputCriteria::NONE;

	size_t wbl_floor = 0;
	size_t wbl_ceil = 0;

	TreeNode<Operation> output_criteria;
	output_criteria.Value = 
	{
		"Выбор данных для вывода",
		[](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_code
		) -> const TreeNode<Operation>*
		{
			out << "'!' -> назад\n";
			out << "Выберите, какие данные должны участвовать в выводе:\n";

			return MoveToChildNodePrompt(node, in, out, op_code, true);
		}
	};

	TreeNode<Operation> oc_char_count;
	oc_char_count.Value = 
	{
		"Кол-во знаков",
		[&criteria](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_code
		) -> const TreeNode<Operation>*
		{
			return ToggleCriteria(node, in, out, op_code, OutputCriteria::CHARACTER_COUNT, criteria);
		}
	};

	TreeNode<Operation> oc_word_count;
	oc_word_count.Value =
	{
		"Кол-во слов",
		[&criteria](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_code
		) -> const TreeNode<Operation>*
		{
			return ToggleCriteria(node, in, out, op_code, OutputCriteria::WORD_COUNT, criteria);
		}
	};

	TreeNode<Operation> oc_word_count_by_length;
	oc_word_count_by_length.Value = 
	{
		"Кол-во слов определённой длинны",
		[&criteria, &wbl_floor, &wbl_ceil](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_code
		) -> const TreeNode<Operation>*
		{
			const TreeNode<Operation>* result_node = 
				ToggleCriteria(node, in, out, op_code, OutputCriteria::WORD_COUNT_BY_LENGTH, criteria);
			
			if (op_code == OpResult::SUCCESS)
			{
				out << "\nВведите два числа, разделённые пробелом - минимальный и максимальный размеры слов:\n";

				in >> wbl_floor >> wbl_ceil;

				return result_node;				
			}

			return result_node;
		}
	};

	TreeNode<Operation> oc_unique_words;
	oc_unique_words.Value =
	{
		"Уникальные слова",
		[&criteria](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_code
		) -> const TreeNode<Operation>*
		{
			return ToggleCriteria(node, in, out, op_code, OutputCriteria::UNIQUE_WORDS, criteria);
		}
	};

	TreeNode<Operation> dump_to_console;
	dump_to_console.Value =
	{
		"Вывод статистики в консоль",
		[
			&criteria, 
			&char_count, 
			&word_count, 
			&word_count_by_length,
			&wbl_floor,
			&wbl_ceil
		](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_code
		) -> const TreeNode<Operation>* 
		{
			return DumpStatisticsToBuffer(
				node, in, out, op_code,
				char_count, word_count, word_count_by_length,
				wbl_floor, wbl_ceil,
				criteria, out, false
			);
		}
	};

	TreeNode<Operation> dump_to_file;
	dump_to_file.Value =
	{
		"Запись статистики в файл",
		[
			&criteria,
			&char_count,
			&word_count,
			&word_count_by_length,
			&wbl_floor,
			&wbl_ceil
		](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_code
		) -> const TreeNode<Operation>*
		{
			out << "'!' -> назад\n";
			out << "Введите путь до файла: ";

			std::string filepath;
			in >> filepath;

			if (filepath[0] == '!')
			{
				op_code = OpResult::ABORT;
				return node->Parent;
			}

			std::ofstream file(filepath);

			out << "Идёт запись файла...";

			if (file)
			{
				const TreeNode<Operation>* result_node = DumpStatisticsToBuffer(
					node, in, out, op_code,
					char_count, word_count, word_count_by_length,
					wbl_floor, wbl_ceil,
					criteria, file, true
				);

				if (op_code == OpResult::SUCCESS)
					out << "Файл успешно сохранён!";
				
				return result_node;
			}
			
			out << "Путь введён некорректно или у программы нет доступа для записи по этому пути...";
			op_code = OpResult::FAILURE;
			return node;
		}
	};

	TreeNode<Operation> post_analysis_reset;
	post_analysis_reset.Value =
	{
		"Сбросить данные анализа",
		[
			&the_entire_file,
			&char_count,
			&word_count,
			&word_count_by_length,
			&wbl_floor,
			&wbl_ceil,
			&criteria,
			&pre_analysis
		](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_code
		) -> const TreeNode<Operation>*
		{
			out << "'!' -> назад\n";
			out << "Вы уверены? После данной операции вы потеряете все данные, полученные после анализа текста.\n";
			out << " Введите что-нибудь, кроме '!', чтобы принять: ";

			char excl;
			in >> excl;

			if (excl == '!')
			{
				op_code = OpResult::ABORT;
				return node->Parent;
			}

			out << "Идёт очистка данных...";

			the_entire_file.clear();
			char_count.clear();
			word_count.clear();
			word_count_by_length.clear();
			wbl_floor = 0;
			wbl_ceil = 0;
			criteria = OutputCriteria::NONE;

			out << "\nДанные успешно очищены!";
			op_code = OpResult::SUCCESS;
			return &pre_analysis;
		}
	};

	TreeNode<Operation> start_analysis;
	start_analysis.Value =
	{
		"Запуск процедуры анализа",
		[
			&post_analysis,
			&stop_words_dictionary, 
			&the_entire_file,
			&char_count,
			&word_count,
			&word_count_by_length
		](
			const TreeNode<Operation>* node,
			std::istream& in, std::ostream& out,
			OpResult& op_code
		) -> const TreeNode<Operation>*
		{
			out << "'!' -> назад\n";
			out << "Введите путь до файла для анализа: ";

			std::string filepath;
			in >> filepath;

			if (filepath[0] == '!')
			{
				op_code = OpResult::ABORT;
				return node->Parent;
			}

			std::ifstream in_file(filepath, std::ios::in | std::ios::ate);

			if (in_file)
			{
				std::ifstream::pos_type file_char_count = in_file.tellg();

				if (file_char_count > 0)
				{
					out << "Чтение из файла...";

					in_file.seekg(0, std::ios::beg);

					the_entire_file.resize(file_char_count);

					in_file.read(&the_entire_file[0], file_char_count);

					out << "Чтение из файла завершено! Начинается процедура анализа текста...";

					Analyze(
						the_entire_file.data(), the_entire_file.data() + the_entire_file.size(),
						char_count, word_count, word_count_by_length,
						IsCharADelimiter,
						[&stop_words_dictionary](const std::string& word) -> bool
						{
							return stop_words_dictionary.find(word) != stop_words_dictionary.cend();
						}
					);

					out << "Анализ завершён!";
					op_code = OpResult::SUCCESS;
					return &post_analysis;
				}

				out << "Файл пуст...";
				op_code = OpResult::FAILURE;
				return node;
			}

			out << "Файл не существует, нечитаем или недоступен...";
			op_code = OpResult::FAILURE;
			return node;
		}
	};

	pre_analysis.Parent = nullptr;
	(&pre_analysis)
		->Append(&stop_words_dict_population)
			->Append(&dict_add_words)->Parent
			->Append(&dict_remove_words)->Parent
			->Append(&load_dict_from_file)->Parent
			->Append(&write_dict_to_file)->Parent
			->Append(&dict_flush)->Parent->Parent
		->Append(&start_analysis);
	
	post_analysis.Parent = nullptr;
	(&post_analysis)
		->Append(&output_criteria)
			->Append(&oc_char_count)->Parent
			->Append(&oc_word_count)->Parent
			->Append(&oc_word_count_by_length)->Parent
			->Append(&oc_unique_words)->Parent->Parent
		->Append(&dump_to_console)->Parent
		->Append(&dump_to_file)->Parent
		->Append(&post_analysis_reset);

	const TreeNode<Operation>* prev_op_node = &pre_analysis;
	const TreeNode<Operation>* active_op_node = &pre_analysis;

	while (true)
	{
		while (active_op_node)
		{
			std::cout << "> ";

			std::vector<std::string> path_v;

			for (const TreeNode<Operation>* path_node = active_op_node; path_node; path_node = path_node->Parent)
			{
				std::string path_node_w_backslash = path_node->Value.Title;
				path_node_w_backslash += '\\';
				path_v.push_back(path_node_w_backslash);
			}
			
			for (std::vector<std::string>::const_reverse_iterator it = path_v.crbegin(); it != path_v.crend(); it++)
				std::cout << *it;
			
			std::cout << "\n\n";

			OpResult op_code;

			const TreeNode<Operation>* result = active_op_node->Value.Function(active_op_node, std::cin, std::cout, op_code);

			std::cout << "\n\n\n";

			prev_op_node = active_op_node;
			active_op_node = result;
		}

		std::cout << "Вы уверены? Весь несохранённый прогресс будет утерян. Введите '!', чтобы подтвердить: ";

		char excl;
		std::cin >> excl;

		if (excl == '!')
			break;
		else
			active_op_node = prev_op_node;
	}

	/*
	std::string the_entire_file;

	bool success = false;

	while (!success)
	{
	std::cout << "Путь до файла для анализа: ";

	#ifdef DEBUG_INPUT_FILEPATH
	std::string filepath = DEBUG_INPUT_FILEPATH;
	#else
	std::string filepath;
	std::cin >> filepath;
	#endif

	std::ifstream file(filepath, std::ios::in | std::ios::ate);

	if (file.is_open())
	{
	std::ifstream::pos_type file_char_count = file.tellg();

	if (file_char_count > 0)
	{
	file.seekg(0, std::ios::beg);

	success = true;

	the_entire_file = std::string(file_char_count, '\0');

	file.read(&the_entire_file[0], the_entire_file.size());
	}
	else
	std::cout << "Файл пуст...\n";
	}
	else
	std::cout << "Файл не существует, нечитаем или недоступен для чтения...\n";
	}

	success = false;

	std::unordered_set<std::string> stop_words;

	while (!success)
	{
	std::cout << "Путь до файла-словаря стоп-слов '!', ����� ���������� ���� ���: ";

	#ifdef DEBUG_DICT_FILEPATH
	std::string filepath = DEBUG_DICT_FILEPATH;
	#else
	std::string filepath;
	std::cin >> filepath;
	#endif

	if (filepath[0] == '!')
	success = true;
	else
	{
	std::ifstream file(filepath);

	if (file.is_open())
	{
	success = true;

	while (file.rdbuf()->in_avail() > -1)
	{
	std::string word;
	file >> word;

	stop_words.insert(word);
	}
	}
	else
	std::cout << "���� �� ���������� ��� ��������...\n";
	}
	}

	success = false;

	std::ofstream out_file;

	while (!success)
	{
	std::cout << "���� �� �����, ���� ����� �������� ���������� �������: ";

	#ifdef DEBUG_OUTPUT_FILEPATH
	std::string filepath = DEBUG_OUTPUT_FILEPATH;
	#else
	std::string filepath;
	std::cin >> filepath;
	#endif

	out_file.open(filepath);

	if (out_file.is_open())
	success = true;
	else
	std::cout << "���� �� ����� �� ����������...\n";
	}

	std::cout << "������������ ������...";

	std::map<char, uint64_t> char_count;
	std::map<std::string, uint64_t> word_count;
	std::map<uint64_t, uint64_t> word_count_by_length;

	Analyze(
	&the_entire_file[0], &the_entire_file[0] + the_entire_file.size(),
	char_count, word_count, word_count_by_length,
	[](char ch) -> bool
	{
	return !std::isalnum(static_cast<unsigned char>(ch), std::locale("Russian")) && ch != L'\'' && ch != L'"';
	},
	[&stop_words](const std::string& word) -> bool
	{
	return stop_words.find(word) != stop_words.cend();
	}
	);

	std::string str_out;
	to_multibyte(L"���-�� ������:\n", str_out);

	out_file << str_out;

	for (const std::pair<char, uint64_t>& chars : char_count)
	out_file << '\'' << chars.first << "': " << chars.second << '\n';

	to_multibyte(L"\n\n���-�� ����:\n", str_out);

	out_file << str_out;

	for (const std::pair<std::string, uint64_t>& words : word_count)
	out_file << '\'' << words.first << "': " << words.second << '\n';

	to_multibyte(L"\n\n���������� �����:\n", str_out);

	out_file << str_out;

	for (std::map<std::string, uint64_t>::const_reverse_iterator it = word_count.crbegin(); it != word_count.crend(); it++)
	{
	if (it->second > 1) break;

	out_file << '\'' << it->first << "' ";
	} */
}
