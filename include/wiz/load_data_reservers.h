
#ifndef LOAD_DATA_RESERVERS_H
#define LOAD_DATA_RESERVERS_H

#include <vector>
#include <fstream>
#include <string>

#include "cpp_string.h"
#include "load_data_utility.h"
#include "queues.h"

namespace wiz {
	namespace load_data {
		class LoadDataOption
		{
		public:
			const char LineComment = '#';	// # 
			const char Left = '{';
			const char Right = '}';	// { }
			const char Assignment = '=';	// = 
		};

		class LoadDataOption2
		{
		public:
			const char Left = '{';	// objectt start
			const char Right = '}';	// object last
			const char Left2 = '['; // array start
			const char Right2 = ']'; // array last
			const char Assignment = ':'; //
			const char Comma = ',';
		};

		inline bool isWhitespace(const char ch)
		{
			switch (ch)
			{
			case ' ':
			case '\t':
			case '\r':
			case '\n':
			case '\v':
			case '\f':
				return true;
				break;
			}
			return false;
		}


		inline int Equal(const long long x, const long long y)
		{
			if (x == y) {
				return 0;
			}
			return -1;
		}

		class InFileReserver
		{
		private:
			// todo - rename.
			static long long Get(long long position, long long length, char ch, const wiz::load_data::LoadDataOption& option) {
				long long x = (position << 32) + (length << 3) + 0;

				if (length != 1) {
					return x;
				}

				if (option.Left == ch) {
					x += 2; // 010
				}
				else if (option.Right == ch) {
					x += 4; // 100
				}
				else if (option.Assignment == ch) {
					x += 6;
				}

				return x;
			}

			static long long GetIdx(long long x) {
				return (x >> 32) & 0x00000000FFFFFFFF;
			}
			static long long GetLength(long long x) {
				return (x & 0x00000000FFFFFFF8) >> 3;
			}
			static long long GetType(long long x) { //to enum or enum class?
				return (x & 6) >> 1;
			}
			static bool IsToken2(long long x) {
				return (x & 1);
			}

			static void _Scanning(const char* text, long long num, const long long length,
				long long*& token_arr, long long& _token_arr_size, const LoadDataOption& option) {

				long long token_arr_size = 0;

				{
					int state = 0;

					long long token_first = 0;
					long long token_last = -1;

					long long token_arr_count = 0;

					for (long long i = 0; i < length; ++i) {
						const char ch = text[i];

						switch (ch) {
						case '\"':
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;
							}

							token_first = i;
							token_last = i;

							token_first = i + 1;
							token_last = i + 1;

							{//
								token_arr[num + token_arr_count] = 1;
								token_arr[num + token_arr_count] += Get(i + num, 1, ch, option);
								token_arr_count++;
							}
							break;
						case '\\':
						{//
							token_arr[num + token_arr_count] = 1;
							token_arr[num + token_arr_count] += Get(i + num, 1, ch, option);
							token_arr_count++;
						}
						break;
						case '\n':
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;
							}
							token_first = i + 1;
							token_last = i + 1;

							{//
								token_arr[num + token_arr_count] = 1;
								token_arr[num + token_arr_count] += Get(i + num, 1, ch, option);
								token_arr_count++;
							}
							break;
						case '\0':
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;
							}
							token_first = i + 1;
							token_last = i + 1;

							{//
								token_arr[num + token_arr_count] = 1;
								token_arr[num + token_arr_count] += Get(i + num, 1, ch, option);
								token_arr_count++;
							}
							break;
						case '#':
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;
							}
							token_first = i + 1;
							token_last = i + 1;

							{//
								token_arr[num + token_arr_count] = 1;
								token_arr[num + token_arr_count] += Get(i + num, 1, ch, option);
								token_arr_count++;
							}

							break;
						case ' ':
						case '\t':
						case '\r':
						case '\v':
						case '\f':
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;
							}
							token_first = i + 1;
							token_last = i + 1;

							break;
						case '{':
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;
							}

							token_first = i;
							token_last = i;

							token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
							token_arr_count++;

							token_first = i + 1;
							token_last = i + 1;
							break;
						case '}':
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;
							}
							token_first = i;
							token_last = i;

							token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
							token_arr_count++;

							token_first = i + 1;
							token_last = i + 1;
							break;
						case '=':
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;
							}
							token_first = i;
							token_last = i;

							token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
							token_arr_count++;

							token_first = i + 1;
							token_last = i + 1;
							break;
						}
					}

					if (length - 1 - token_first + 1 > 0) {
						token_arr[num + token_arr_count] = Get(token_first + num, length - 1 - token_first + 1, text[token_first], option);
						token_arr_count++;
					}
					token_arr_size = token_arr_count;
				}

				{
					_token_arr_size = token_arr_size;
				}
			}

			static void ScanningNew(const char* text, const long long length, const int thr_num,
				long long*& _token_arr, long long& _token_arr_size, const LoadDataOption& option)
			{
				std::vector<std::thread> thr(thr_num);
				std::vector<long long> start(thr_num);
				std::vector<long long> last(thr_num);

				{
					start[0] = 0;

					for (int i = 1; i < thr_num; ++i) {
						start[i] = length / thr_num * i;

						for (long long x = start[i]; x <= length; ++x) {
							if (isWhitespace(text[x]) || '\0' == text[x] ||
								option.Left == text[x] || option.Right == text[x] || option.Assignment == text[x]) {
								start[i] = x;
								break;
							}
						}
					}
					for (int i = 0; i < thr_num - 1; ++i) {
						last[i] = start[i + 1];
						for (long long x = last[i]; x <= length; ++x) {
							if (isWhitespace(text[x]) || '\0' == text[x] ||
								option.Left == text[x] || option.Right == text[x] || option.Assignment == text[x]) {
								last[i] = x;
								break;
							}
						}
					}
					last[thr_num - 1] = length + 1;
				}
				long long real_token_arr_count = 0;

				long long* tokens = new long long[length + 1];
				long long token_count = 0;

				std::vector<long long> token_arr_size(thr_num);

				for (int i = 0; i < thr_num; ++i) {
					thr[i] = std::thread(_Scanning, text + start[i], start[i], last[i] - start[i], std::ref(tokens), std::ref(token_arr_size[i]), option);
				}

				for (int i = 0; i < thr_num; ++i) {
					thr[i].join();
				}

				{
					long long _count = 0;
					for (int i = 0; i < thr_num; ++i) {
						for (long long j = 0; j < token_arr_size[i]; ++j) {
							tokens[token_count] = tokens[start[i] + j];
							token_count++;
						}
					}
				}

				int state = 0;
				long long qouted_start;
				long long slush_start;

				for (long long i = 0; i < token_count; ++i) {
					const long long len = GetLength(tokens[i]);
					const char ch = text[GetIdx(tokens[i])];
					const long long idx = GetIdx(tokens[i]);
					const bool isToken2 = IsToken2(tokens[i]);

					if (isToken2) {
						if (0 == state && '\"' == ch) {
							state = 1;
							qouted_start = i;
						}
						else if (0 == state && option.LineComment == ch) {
							state = 2;
						}
						else if (1 == state && '\\' == ch) {
							state = 3;
							slush_start = idx;
						}
						else if (1 == state && '\"' == ch) {
							state = 0;

							{
								long long idx = GetIdx(tokens[qouted_start]);
								long long len = GetLength(tokens[qouted_start]);

								len = GetIdx(tokens[i]) - idx + 1;

								tokens[real_token_arr_count] = Get(idx, len, text[idx], option);
								real_token_arr_count++;
							}
						}
						else if (3 == state) {
							if (idx != slush_start + 1) {
								--i;
							}
							state = 1;
						}
						else if (2 == state && ('\n' == ch || '\0' == ch)) {
							state = 0;
						}
					}
					else if (0 == state) { // '\\' case?
						tokens[real_token_arr_count] = tokens[i];
						real_token_arr_count++;
					}
				}

				{
					if (0 != state) {
						std::cout << "[ERRROR] state [" << state << "] is not zero \n";
					}
				}


				{
					_token_arr = tokens;
					_token_arr_size = real_token_arr_count;
				}
			}


			static void Scanning(const char* text, const long long length,
				long long*& _token_arr, long long& _token_arr_size, const LoadDataOption& option) {

				long long* token_arr = new long long[length + 1];
				long long token_arr_size = 0;

				{
					int state = 0;

					long long token_first = 0;
					long long token_last = -1;

					long long token_arr_count = 0;

					for (long long i = 0; i <= length; ++i) {
						const char ch = text[i];

						if (0 == state) {
							if (option.LineComment == ch) {
								token_last = i - 1;
								if (token_last - token_first + 1 > 0) {
									token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
									token_arr_count++;
								}

								state = 3;
							}
							else if ('\"' == ch) {
								state = 1;
							}
							else if (isWhitespace(ch) || '\0' == ch) {
								token_last = i - 1;
								if (token_last - token_first + 1 > 0) {
									token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
									token_arr_count++;
								}
								token_first = i + 1;
								token_last = i + 1;
							}
							else if (option.Left == ch) {
								token_last = i - 1;
								if (token_last - token_first + 1 > 0) {
									token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
									token_arr_count++;
								}

								token_first = i;
								token_last = i;

								token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;

								token_first = i + 1;
								token_last = i + 1;
							}
							else if (option.Right == ch) {
								token_last = i - 1;
								if (token_last - token_first + 1 > 0) {
									token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
									token_arr_count++;
								}
								token_first = i;
								token_last = i;

								token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;

								token_first = i + 1;
								token_last = i + 1;

							}
							else if (option.Assignment == ch) {
								token_last = i - 1;
								if (token_last - token_first + 1 > 0) {
									token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
									token_arr_count++;
								}
								token_first = i;
								token_last = i;

								token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;

								token_first = i + 1;
								token_last = i + 1;
							}
						}
						else if (1 == state) {
							if ('\\' == ch) {
								state = 2;
							}
							else if ('\"' == ch) {
								state = 0;
							}
						}
						else if (2 == state) {
							state = 1;
						}
						else if (3 == state) {
							if ('\n' == ch || '\0' == ch) {
								state = 0;

								token_first = i + 1;
								token_last = i + 1;
							}
						}
					}

					token_arr_size = token_arr_count;

					if (0 != state) {
						std::cout << "[" << state << "] state is not zero.\n";
					}
				}

				{
					_token_arr = token_arr;
					_token_arr_size = token_arr_size;
				}
			}


			static std::pair<bool, int> Scan(std::ifstream& inFile, const int num, const wiz::load_data::LoadDataOption& option, int thr_num,
				const char*& _buffer, long long* _buffer_len, long long*& _token_arr, long long* _token_arr_len)
			{
				if (inFile.eof()) {
					return { false, 0 };
				}

				long long* arr_count = nullptr; //
				long long arr_count_size = 0;

				std::string temp;
				char* buffer = nullptr;
				long long file_length;

				{
					inFile.seekg(0, inFile.end);
					unsigned long long length = inFile.tellg();
					inFile.seekg(0, inFile.beg);

					Utility::BomType x = Utility::ReadBom(inFile);
					//	wiz::Out << "length " << length << "\n";
					if (x == Utility::BomType::UTF_8) {
						length = length - 3;
					}

					file_length = length;
					buffer = new char[file_length + 1]; // 

					// read data as a block:
					inFile.read(buffer, file_length);

					buffer[file_length] = '\0';

					{
						//int a = clock();
						long long* token_arr;
						long long token_arr_size;

						if (thr_num == 1) {
							Scanning(buffer, file_length, token_arr, token_arr_size, option);
						}
						else {
							ScanningNew(buffer, file_length, thr_num, token_arr, token_arr_size, option);
						}

						//int b = clock();
					//	std::cout << b - a << "ms\n";
						_buffer = buffer;
						_token_arr = token_arr;
						*_token_arr_len = token_arr_size;
						*_buffer_len = file_length;
					}
				}

				return{ true, 1 };
			}

			static std::pair<bool, int> Scan(const int num, const wiz::load_data::LoadDataOption& option, int thr_num,
				const std::string& str, long long* _buffer_len, long long*& _token_arr, long long* _token_arr_len)
			{
				long long* arr_count = nullptr; //
				long long arr_count_size = 0;

				const char* buffer = nullptr;
				
				long long file_length;

				{
					buffer = str.c_str();
					file_length = str.size();

					Utility::BomInfo stBom{ 0, };

					Utility::BomType x = Utility::ReadBom(buffer, std::min((long long)5, (long long)str.size()), stBom);
					//	wiz::Out << "length " << length << "\n";
					if (x == Utility::BomType::UTF_8) {
						file_length = file_length - 3;
						buffer = buffer + 3;
					}

					
					{
						//int a = clock();
						long long* token_arr;
						long long token_arr_size;

						if (thr_num == 1) {
							Scanning(buffer, file_length, token_arr, token_arr_size, option);
						}
						else {
							ScanningNew(buffer, file_length, thr_num, token_arr, token_arr_size, option);
						}

						_token_arr = token_arr;
						*_token_arr_len = token_arr_size;
						*_buffer_len = file_length;
					}
				}

				return{ true, 1 };
			}

		private: public:
			std::ifstream* pInFile = nullptr;
		
			std::string str;
			int Num;
		public:
			explicit InFileReserver(const std::string& str)
			: str(str) {
				Num = 1;
			}
			explicit InFileReserver(std::ifstream& inFile)
			{
				pInFile = &inFile;
				Num = 1;
			}
			bool end()const { return pInFile->eof(); } //
		public:
			bool operator() (const wiz::load_data::LoadDataOption& option, int thr_num, const char*& buffer, long long* buffer_len, long long*& token_arr, long long* token_arr_len)
			{
				bool x;
				if (pInFile) {
					x = Scan(*pInFile, Num, option, thr_num, buffer, buffer_len, token_arr, token_arr_len).second > 0;
				}
				else {
					x = Scan(Num, option, thr_num, str, buffer_len, token_arr, token_arr_len).second > 0;
					if (x) {
						buffer = str.c_str();
					}
				}
					//	std::cout << *token_arr_len << "\n";
				return x;
			}
		};
		
		class InFileReserverJson
		{
		private:
			// todo - rename.
			static long long Get(long long position, long long length, char ch, const wiz::load_data::LoadDataOption2& option) {
				long long x = (position << 32) + (length << 3) + 0;

				if (length != 1) {
					return x;
				}

				if (option.Left == ch || option.Left2 == ch) {
					x += 2; // 010
				}
				else if (option.Right == ch || option.Right2 == ch) {
					x += 4; // 100
				}
				else if (option.Assignment == ch) {
					x += 6;
				}

				return x;
			}

			static long long GetIdx(long long x) {
				return (x >> 32) & 0x00000000FFFFFFFF;
			}
			static long long GetLength(long long x) {
				return (x & 0x00000000FFFFFFF8) >> 3;
			}
			static long long GetType(long long x) { //to enum or enum class?
				return (x & 6) >> 1;
			}
			static bool IsToken2(long long x) {
				return (x & 1);
			}

			static void _Scanning(char* text, long long num, const long long length,
				long long*& token_arr, long long& _token_arr_size, const LoadDataOption2& option) {

				long long token_arr_size = 0;

				{
					int state = 0;

					long long token_first = 0;
					long long token_last = -1;

					long long token_arr_count = 0;

					for (long long i = 0; i < length; ++i) {
						const char ch = text[i];

						switch (ch) {
						case '\"':
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;
							}

							token_first = i;
							token_last = i;

							token_first = i + 1;
							token_last = i + 1;

							{//
								token_arr[num + token_arr_count] = 1;
								token_arr[num + token_arr_count] += Get(i + num, 1, ch, option);
								token_arr_count++;
							}
							break;
						case '\\':
						{//
							token_arr[num + token_arr_count] = 1;
							token_arr[num + token_arr_count] += Get(i + num, 1, ch, option);
							token_arr_count++;
						}
						break;
						case '\n':
						case '\0':
						case ' ':
						case '\t':
						case '\r':
						case '\v':
						case '\f':
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;
							}
							token_first = i + 1;
							token_last = i + 1;

							break;
						case '{':
						case '[':
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;
							}

							token_first = i;
							token_last = i;

							token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
							token_arr_count++;

							token_first = i + 1;
							token_last = i + 1;
							break;
						case '}':
						case ']':
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;
							}
							token_first = i;
							token_last = i;

							token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
							token_arr_count++;

							token_first = i + 1;
							token_last = i + 1;
							break;
						case ':':
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;
							}
							token_first = i;
							token_last = i;

							token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
							token_arr_count++;

							token_first = i + 1;
							token_last = i + 1;
							break;
						case ',':
							token_last = i - 1;
							if (token_last - token_first + 1 > 0) {
								token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;
							}
							token_first = i;
							token_last = i;

							token_arr[num + token_arr_count] = Get(token_first + num, token_last - token_first + 1, text[token_first], option);
							token_arr_count++;

							token_first = i + 1;
							token_last = i + 1;
							break;
						}
					}

					if (length - 1 - token_first + 1 > 0) {
						token_arr[num + token_arr_count] = Get(token_first + num, length - 1 - token_first + 1, text[token_first], option);
						token_arr_count++;
					}
					token_arr_size = token_arr_count;
				}

				{
					_token_arr_size = token_arr_size;
				}
			}

			static void ScanningNew(char* text, const size_t length, const int thr_num,
				long long*& _token_arr, long long& _token_arr_size, const LoadDataOption2& option)
			{
				std::vector<std::thread> thr(thr_num);
				std::vector<long long> start(thr_num);
				std::vector<long long> last(thr_num);

				{
					start[0] = 0;

					for (int i = 1; i < thr_num; ++i) {
						start[i] = length / thr_num * i;

						for (size_t x = start[i]; x <= length; ++x) {
							if (isWhitespace(text[x]) || '\0' == text[x] ||
								option.Left == text[x] || option.Right == text[x] ||
								option.Left2 == text[x] || option.Right2 == text[x] ||
								option.Assignment == text[x]) {
								start[i] = x;
								break;
							}
						}
					}
					for (int i = 0; i < thr_num - 1; ++i) {
						last[i] = start[i + 1];
						for (size_t x = last[i]; x <= length; ++x) {
							if (isWhitespace(text[x]) || '\0' == text[x] ||
								option.Left == text[x] || option.Right == text[x] ||
								option.Left2 == text[x] || option.Right2 == text[x] ||
								option.Assignment == text[x]) {
								last[i] = x;
								break;
							}
						}
					}
					last[thr_num - 1] = length + 1;
				}
				long long real_token_arr_count = 0;

				long long* tokens = new long long[length + 1];
				long long token_count = 0;

				std::vector<long long> token_arr_size(thr_num);

				for (int i = 0; i < thr_num; ++i) {
					thr[i] = std::thread(_Scanning, text + start[i], start[i], last[i] - start[i], std::ref(tokens), std::ref(token_arr_size[i]), option);
				}

				for (int i = 0; i < thr_num; ++i) {
					thr[i].join();
				}

				{
					long long _count = 0;
					for (int i = 0; i < thr_num; ++i) {
						for (long long j = 0; j < token_arr_size[i]; ++j) {
							tokens[token_count] = tokens[start[i] + j];
							token_count++;
						}
					}
				}

				bool var = false;
				bool val = false;
				bool parse_state = 0;
				bool expect_comma = false;
				bool comma_on = false;

				std::stack<char> _stack;
				_stack.push(option.Left);

				int state = 0;
				long long qouted_start;
				long long slush_start;

				for (long long i = 0; i < token_count; ++i) {
					long long len = GetLength(tokens[i]);
					char ch = text[GetIdx(tokens[i])];
					long long idx = GetIdx(tokens[i]);
					const bool isToken2 = IsToken2(tokens[i]);
					const long long type = GetType(tokens[i]);

					bool pass = false;

					if (isToken2) {
						if (0 == state && '\"' == ch) {
							state = 1;
							qouted_start = i;
						}
						else if (1 == state && '\\' == ch) {
							state = 3;
							slush_start = idx;
						}
						else if (1 == state && '\"' == ch) {
							state = 0;

							{
								long long _idx = GetIdx(tokens[qouted_start]);
								long long _len = GetLength(tokens[qouted_start]);

								_len = GetIdx(tokens[i]) - _idx + 1;

								tokens[real_token_arr_count] = Get(_idx, _len, text[_idx], option);
								real_token_arr_count++;

								pass = true;

								len = _len;
								idx = _idx;
								ch = text[_idx];
							}
						}
						else if (3 == state) {
							if (idx != slush_start + 1) {
								--i;
							}
							state = 1;
						}
					}
					else if (0 == state) {
						tokens[real_token_arr_count] = tokens[i];
						real_token_arr_count++;

						pass = true;
					}

					if (pass) {
						if (_stack.empty()) {
							std::cout << "Syntax Error in pre-parsing1\n";
						}

						if (expect_comma) {
							if (len == 1 && ch == ',') {
								real_token_arr_count--;
								expect_comma = false;
								comma_on = true;
								continue;
							}
							else {
								if (type == 2 || type == 3) {
									//
								}
								else {
									std::cout << "Syntax Error in pre-parsing2\n";
								}
							}
						}
						else {
							if (len == 1 && ch == ',') {
								std::cout << "Syntax Error in pre-parsing3.5\n";
							}
						}

						expect_comma = false;


						// pre-parsing? - checking syntax errors. for comma.
						if (0 == parse_state) {
							if (1 == type) { // LEFT
								_stack.push(ch);
								comma_on = false;
							}
							else if (2 == type) { // RIGHT
								if (comma_on) {
									std::cout << "Syntax Error in pre-parsing3.75\n";
								}

								if (_stack.top() == option.Left && ch == option.Right) {
									_stack.pop();
								}
								else if (_stack.top() == option.Left2 && ch == option.Right2) {
									_stack.pop();
								}
								else {
									std::cout << "Syntax Error in pre-parsing4\n";
								}

								expect_comma = true;
							}
							else if (3 == type) { // ASSIGN
								if (comma_on) {
									std::cout << "Syntax Error in pre-parsing4.5\n";
								}

								if (_stack.top() == option.Left2) { // array
									std::cout << "Syntax Error in pre-parsing5\n";
								}
								if (!var) {
									std::cout << "Syntax Error in pre-parsing6\n";
								}
								parse_state = 1;
							}
							else {
								if (1 == len && ch == ',') {
									std::cout << "Syntax Error in pre-parsing7\n";
								}

								if (_stack.top() == option.Left) {
									var = true;
									val = false;
								}
								else { // _stack.top() == option.Left2
									var = false;
									val = true;
								}

								if (var) {
									if (text[idx] == '\"' &&
										text[idx + len - 1] == '\"' && len >= 2) {
										//
									}
									else {
										std::cout << "Syntax Error in pre-parsing8\n";
									}
								}
								else if (val) {
									expect_comma = true;
									val = false;
								}

								comma_on = false;
							}
						}
						else if (1 == parse_state) {
							if (comma_on) {
								std::cout << "Syntax Error in pre-parsing8.5\n";
							}

							if (1 == type) { // LEFT
								_stack.push(ch);
								var = false;
								val = false;
							}
							else if (0 == type) {
								if (len == 1 && ch == ',') {
									std::cout << "Syntax Error in pre-parsing9\n";
								}
								else {
									var = false;
									val = true;

									if (val) {
										expect_comma = true;
										val = false;
									}
								}
							}
							else {
								// error
								std::cout << "Syntax Error in pre-parsing10\n";
							}

							parse_state = 0;
						}
					}
				}

				if (comma_on) {
					std::cout << "Syntax Error in pre-parsing11\n";
				}

				{
					if (1 != _stack.size()) {
						std::cout << "_stack.size() != 1\n";
					}

					if (0 != state) {
						std::cout << "[ERROR] state [" << state << "] is not zero \n";
					}
				}


				{
					_token_arr = tokens;
					_token_arr_size = real_token_arr_count;
				}
			}


			static void Scanning(char* text, const size_t length,
				long long*& _token_arr, long long& _token_arr_size, const LoadDataOption2& option) {

				long long* token_arr = new long long[length + 1];
				long long token_arr_size = 0;

				{
					int state = 0;

					long long token_first = 0;
					long long token_last = -1;

					long long token_arr_count = 0;

					for (size_t i = 0; i <= length; ++i) {
						const char ch = text[i];

						if (0 == state) {
							if ('\"' == ch) {
								state = 1;
							}
							else if (isWhitespace(ch) || '\0' == ch) {
								token_last = i - 1;
								if (token_last - token_first + 1 > 0) {
									token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
									token_arr_count++;
								}
								token_first = i + 1;
								token_last = i + 1;
							}
							else if (option.Left == ch || option.Left2 == ch) {
								token_last = i - 1;
								if (token_last - token_first + 1 > 0) {
									token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
									token_arr_count++;
								}

								token_first = i;
								token_last = i;

								token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;

								token_first = i + 1;
								token_last = i + 1;
							}
							else if (option.Right == ch || option.Right2 == ch) {
								token_last = i - 1;
								if (token_last - token_first + 1 > 0) {
									token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
									token_arr_count++;
								}
								token_first = i;
								token_last = i;

								token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;

								token_first = i + 1;
								token_last = i + 1;

							}
							else if (option.Assignment == ch) {
								token_last = i - 1;
								if (token_last - token_first + 1 > 0) {
									token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
									token_arr_count++;
								}
								token_first = i;
								token_last = i;

								token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;

								token_first = i + 1;
								token_last = i + 1;
							}
							else if (option.Comma == ch) {
								token_last = i - 1;
								if (token_last - token_first + 1 > 0) {
									token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
									token_arr_count++;
								}
								token_first = i;
								token_last = i;

								token_arr[token_arr_count] = Get(token_first, token_last - token_first + 1, text[token_first], option);
								token_arr_count++;

								token_first = i + 1;
								token_last = i + 1;
							}
						}
						else if (1 == state) {
							if ('\\' == ch) {
								state = 2;
							}
							else if ('\"' == ch) {
								state = 0;
							}
						}
						else if (2 == state) {
							state = 1;
						}
						else if (3 == state) {
							if ('\n' == ch || '\0' == ch) {
								state = 0;

								token_first = i + 1;
								token_last = i + 1;
							}
						}

					}

					std::stack<char> _stack;
					_stack.push(option.Left);
					bool expect_comma = false;
					bool comma_on = false;
					int parse_state = 0;
					bool var = false;
					bool val = false;
					long long real_token_arr_count = 0;

					for (long long i = 0; i < token_arr_count; ++i) {
						if (_stack.empty()) {
							std::cout << "Syntax Error in pre-parsing1\n";
						}

						const long long type = GetType(token_arr[i]);
						const char ch = text[GetIdx(token_arr[i])];
						const long long len = GetLength(token_arr[i]);
						const long long idx = GetIdx(token_arr[i]);


						if (expect_comma) {
							if (len == 1 && ch == ',') {
								real_token_arr_count--;
								expect_comma = false;
								comma_on = true;
								continue;
							}
							else {
								if (type == 2 || type == 3) {
									//
								}
								else {
									std::cout << "Syntax Error in pre-parsing2\n";
								}
							}
						}
						else {
							if (len == 1 && ch == ',') {
								std::cout << "Syntax Error in pre-parsing3.5\n";
							}
						}

						expect_comma = false;


						// pre-parsing? - checking syntax errors. for comma.
						if (0 == parse_state) {
							if (1 == type) { // LEFT
								_stack.push(ch);
								comma_on = false;
							}
							else if (2 == type) { // RIGHT
								if (comma_on) {
									std::cout << "Syntax Error in pre-parsing3.75\n";
								}

								if (_stack.top() == option.Left && ch == option.Right) {
									_stack.pop();
								}
								else if (_stack.top() == option.Left2 && ch == option.Right2) {
									_stack.pop();
								}
								else {
									std::cout << "Syntax Error in pre-parsing4\n";
								}

								expect_comma = true;
							}
							else if (3 == type) { // ASSIGN
								if (comma_on) {
									std::cout << "Syntax Error in pre-parsing4.5\n";
								}

								if (_stack.top() == option.Left2) { // array
									std::cout << "Syntax Error in pre-parsing5\n";
								}
								if (!var) {
									std::cout << "Syntax Error in pre-parsing6\n";
								}
								parse_state = 1;
							}
							else {
								if (1 == len && ch == ',') {
									std::cout << "Syntax Error in pre-parsing7\n";
								}

								if (_stack.top() == option.Left) {
									var = true;
									val = false;
								}
								else {
									var = false;
									val = true;
								}

								if (var) {
									if (text[idx] == '\"' &&
										text[idx + len - 1] == '\"' && len >= 2) {
										//
									}
									else {
										std::cout << "Syntax Error in pre-parsing8\n";
									}
								}
								else if (val) {
									expect_comma = true;
									val = false;
								}

								comma_on = false;
							}
						}
						else if (1 == parse_state) {
							if (comma_on) {
								std::cout << "Syntax Error in pre-parsing8.5\n";
							}

							if (1 == type) { // LEFT
								_stack.push(ch);
								var = false;
								val = false;
							}
							else if (0 == type) {
								if (len == 1 && ch == ',') {
									std::cout << "Syntax Error in pre-parsing9\n";
								}
								else {
									var = false;
									val = true;

									if (val) {
										expect_comma = true;
										val = false;
									}
								}
							}
							else {
								// error
								std::cout << "Syntax Error in pre-parsing10\n";
							}

							parse_state = 0;
						}
					}

					if (comma_on) {
						std::cout << "Syntax Error in pre-parsing11\n";
					}

					token_arr_size = real_token_arr_count;

					if (0 != state) {
						std::cout << "[" << state << "] state is not zero.\n";
					}
				}

				{
					_token_arr = token_arr;
					_token_arr_size = token_arr_size;
				}
			}



			static std::pair<bool, int> Scan(std::ifstream& inFile, const int num, const wiz::load_data::LoadDataOption2& option, int thr_num,
				char*& _buffer, long long* _buffer_len, long long*& _token_arr, long long* _token_arr_len)
			{
				if (inFile.eof()) {
					return { false, 0 };
				}

				long long* arr_count = nullptr; //
				long long arr_count_size = 0;

				std::string temp;
				char* buffer = nullptr;
				long long file_length;

				{
					inFile.seekg(0, inFile.end);
					long long length = inFile.tellg();
					inFile.seekg(0, inFile.beg);

					Utility::BomType x = Utility::ReadBom(inFile);
					//	wiz::Out << "length " << length << "\n";
					if (x == Utility::BomType::UTF_8) {
						length = length - 3;
#if _WIN32
						SetConsoleOutputCP(65001); // UTF-8 Codepage 
#endif
					}

					file_length = length;
					buffer = new char[file_length + 1]; // 

					// read data as a block:
					inFile.read(buffer, file_length);

					buffer[file_length] = '\0';

					{
						//int a = clock();
						long long* token_arr;
						long long token_arr_size;

						if (thr_num == 1) {
							Scanning(buffer, file_length, token_arr, token_arr_size, option);
						}
						else {
							ScanningNew(buffer, file_length, thr_num, token_arr, token_arr_size, option);
						}

						//int b = clock();
					//	std::cout << b - a << "ms\n";
						_buffer = buffer;
						_token_arr = token_arr;
						*_token_arr_len = token_arr_size;
						*_buffer_len = file_length;
					}
				}

				return{ true, 1 };
			}

		private:
			std::ifstream* pInFile;
		public:
			int Num;
		public:
			explicit InFileReserverJson(std::ifstream& inFile)
			{
				pInFile = &inFile;
				Num = 1;
			}
			bool end()const { return pInFile->eof(); } //
		public:
			bool operator() (const wiz::load_data::LoadDataOption2& option, int thr_num, char*& buffer, long long* buffer_len, long long*& token_arr, long long* token_arr_len)
			{
				bool x = Scan(*pInFile, Num, option, thr_num, buffer, buffer_len, token_arr, token_arr_len).second > 0;

				//	std::cout << *token_arr_len << "\n";
				return x;
			}
		};
	}
}

#endif