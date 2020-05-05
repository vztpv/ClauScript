
#ifndef LOAD_DATA_H_INCLUDED
#define LOAD_DATA_H_INCLUDED


#include <vector>
#include <stack>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>
#include <set>
#include <map>

#include "global.h"
//#include <wiz/Dictionary.h> /// change to map

#include "cpp_string.h"
#include "queues.h"
#include "stacks.h"
//#include <wiz/deck.h>

#include <string>

#include "load_data_types.h"
#include "load_data_utility.h"
#include "load_data_reservers.h"


//const std::string LEFT = "{";
//const std::string RIGHT = "}";
//const std::string EQ_STR = "="; // EQ 충돌 -> EQ_STR로 변경

#include "array_map.h"

namespace wiz {
	class EventInfo
	{
	public:
		wiz::load_data::UserType* eventUT;
		wiz::ArrayStack< wiz::load_data::UserType* > nowUT; //
		wiz::ArrayStack<int> userType_idx;
		wiz::ArrayMap<std::string, std::string> parameters;
		wiz::ArrayMap<std::string, std::string> locals;
		std::string id; //
		wiz::ArrayStack<std::string> conditionStack;
		wiz::ArrayStack<int> state;
		std::string return_value;
		std::string option;
	public:
		EventInfo() : eventUT(nullptr), return_value("")
		{

		}
	};

	class ExecuteData
	{
	public:
		wiz::load_data::UserType* pEvents;
		EventInfo info; // chk!
		bool chkInfo;
		wiz::Map2<std::string, wiz::load_data::UserType>* pObjectMap;
		wiz::Map2<std::string, wiz::load_data::UserType>* pModule;

		long long depth;

		bool noUseInput;
		bool noUseOutput;
	public:
		explicit ExecuteData()
			: pEvents(nullptr), pObjectMap(nullptr), pModule(nullptr), chkInfo(false), depth(0),
			noUseInput(false), noUseOutput(false)
		{
			//
		}
	};

	class SortInfo // need to rename
	{
	public:
		std::string data;
		int iElement; // 2 : userType, // 1 : item
		size_t idx; // for stable? - chk!!
	public:
		SortInfo(wiz::StringBuilder* builder = nullptr) : idx(-1), iElement(-1) { }
		SortInfo(const std::string& data, int iElement, size_t idx)
			: data(data), iElement(iElement), idx(idx)
		{

		}
		// for sorting..
		bool operator<(const SortInfo& info) const
		{
			std::string temp = wiz::load_data::Utility::Compare(this->data, info.data);

			if (this->data == "") {
				return false;
			}
			if (info.data == "") {
				return true;
			}

			if (temp == "< 0") { return true; }
			else if (temp == "> 0") { return false; }
			else if (temp == "== 0") {
				return idx < info.idx;
			}
			else {
				throw "temp is not valid in sortinfo";
			}
		}
	};

	class SortInfo2 // need to rename, sortinfo by dsc..
	{
	public:
		std::string data;
		int iElement; // 2 : userType, // 1 : item
		size_t idx; // for stable? - chk!!
	public:
		SortInfo2(wiz::StringBuilder* builder = nullptr) : idx(-1), iElement(-1) { }

		SortInfo2(const std::string& data, int iElement, size_t idx)
			: data(data), iElement(iElement), idx(idx)
		{

		}
		// for sorting..
		bool operator<(const SortInfo2& info) const
		{
			if (this->data == "" && info.data == "") {
				return false;
			}
			if (this->data == "") {
				return true;
			}

			if (info.data == "") {
				return false;
			}

			std::string temp = wiz::load_data::Utility::Compare(this->data, info.data);
			if (temp == "< 0") { return false; }
			else if (temp == "> 0") { return true; }
			else if (temp == "== 0") {
				return idx < info.idx;
			}
			else {
				wiz::Out << "sortInfo2" << ENTER;
				wiz::Out << data << " " << info.data << ENTER;
				throw "temp is not valid in sortinfo2";
			}
		}
	};


	class Option
	{
	public:
		wiz::Map2<std::string, std::pair<std::vector<std::string>, bool>>* _map = nullptr; // todo - fixed max size? and rename
		//std::vector<std::thread*> waits;
		wiz::Map2<std::string, wiz::load_data::UserType>* objectMap = nullptr; // std::string -> wiz::load_data::UserType
		wiz::Map2<std::string, wiz::load_data::UserType>* moduleMap = nullptr;
		std::string* module_value = nullptr;
		// data, event load..
		wiz::ArrayStack<EventInfo>* eventStack = nullptr;
		wiz::Map2<std::string, int>* convert = nullptr;
		std::vector<wiz::load_data::UserType*>* _events = nullptr;
		wiz::load_data::UserType* events = nullptr;
		wiz::load_data::UserType* Main = nullptr;


	public:
		virtual ~Option()
		{
			if (_map) {
				delete _map;
			}
			if (objectMap) {
				delete objectMap;
			}
			if (moduleMap) {
				delete moduleMap;
			}
			if (module_value) {
				delete module_value;
			}
			if (eventStack) {
				delete eventStack;
			}
			if (convert) {
				delete convert;
			}
			if (_events) {
				delete _events;
			}
			if (events) {
				delete events;
			}
			if (Main) {
				delete Main;
			}
		}
	};
}

namespace wiz {
	namespace load_data {
		class EXCUTE_MODULE_BASE
		{
		public:
			virtual std::string excute_module(const std::string& MainStr,
				wiz::load_data::UserType* _global, const ExecuteData& excuteData, Option& opt, int chk)
			{
				return ""; // error!
			}
		};
	}

	class ClauText : public wiz::load_data::EXCUTE_MODULE_BASE {
	public:
		ClauText(const ClauText&) = delete;
		ClauText& operator=(const ClauText&) = delete;

		ClauText()
		{
			//
		}
		virtual ~ClauText()
		{
			//
		}

	public:

		void ShellMode(wiz::load_data::UserType& global);

		virtual std::string excute_module(const std::string& MainStr, wiz::load_data::UserType* _global, const ExecuteData& excuteData, Option& option, int chk);
	};
}

inline wiz::ClauText clauText;

namespace wiz {

	namespace load_data {
		// for json
		class LoadData2
		{
			enum {
				TYPE_LEFT = 1, // 01
				TYPE_RIGHT = 2, // 10
				TYPE_ASSIGN = 3 // 11
			};
		private:
			static long long check_syntax_error1(long long str, int* err) {
				long long len = GetLength(str);
				long long type = GetType(str);

				if (1 == len && (type == TYPE_LEFT || type == TYPE_RIGHT ||
					type == TYPE_ASSIGN)) {
					*err = -4;
				}
				return str;
			}
		public:
			static int Merge(UserType* next, UserType* ut, UserType** ut_next)
			{
				//check!!
				while (ut->GetIListSize() >= 1 && ut->GetUserTypeListSize() >= 1
					&& (ut->GetUserTypeList(0)->IsVirtual()))
				{
					ut = ut->GetUserTypeList(0);
				}

				bool chk_ut_next = false;

				while (true) {
					int itCount = 0;
					int utCount = 0;

					UserType* _ut = ut;
					UserType* _next = next;


					if (ut_next && _ut == *ut_next) {
						*ut_next = _next;
						chk_ut_next = true;
					}

					for (int i = 0; i < _ut->GetIListSize(); ++i) {
						if (_ut->IsUserTypeList(i)) {
							if (_ut->GetUserTypeList(utCount)->IsVirtual()) {
								_ut->GetUserTypeList(utCount)->SetName("");
							}
							else {
								{
									_next->LinkUserType(_ut->GetUserTypeList(utCount));
									_ut->GetUserTypeList(utCount) = nullptr;
								}
							}
							utCount++;
						}
						else if (_ut->IsItemList(i)) {
							_next->AddItemType(std::move(_ut->GetItemList(itCount)));
							itCount++;
						}
					}
					_ut->Remove();

					ut = ut->GetParent();
					next = next->GetParent();


					if (next && ut) {
						//
					}
					else {
						// right_depth > left_depth
						if (!next && ut) {
							return -1;
						}
						else if (next && !ut) {
							return 1;
						}

						return 0;
					}
				}
			}
		private:
			static bool __LoadData(const char* buffer, const long long* token_arr, long long token_arr_len, UserType* _global, const wiz::load_data::LoadDataOption2* _option,
				int start_state, int last_state, UserType** next, long long* lines, long long lines_len, int* err)
			{

				std::vector<long long> varVec;
				std::vector<long long> valVec;

				long long start = 0;

				if (token_arr_len <= 0) {
					return false;
				}

				UserType& global = *_global;
				wiz::load_data::LoadDataOption2 option = *_option;

				int state = start_state;
				int braceNum = 0;
				std::vector< UserType* > nestedUT(1);
				long long var = 0, val = 0;

				nestedUT.reserve(10);
				nestedUT[0] = &global;


				long long count = 0;
				const long long* x = token_arr;
				const long long* x_next = x;

				for (long long i = 0; i < token_arr_len; ++i) {
					x = x_next;
					{
						x_next = x + 1;
					}
					if (count > 0) {
						count--;
						continue;
					}
					long long len = GetLength(token_arr[i]);

					switch (state)
					{
					case 0:
					{
						// Left 1
						if (len == 1 && (-1 != Equal(TYPE_LEFT, GetType(token_arr[i])))) {
							if (!varVec.empty()) {
								nestedUT[braceNum]->ReserveIList(nestedUT[braceNum]->GetIListSize() + varVec.size());
								nestedUT[braceNum]->ReserveItemList(nestedUT[braceNum]->GetItemListSize() + varVec.size());

								for (size_t x = 0; x < varVec.size(); ++x) {
									auto info1 = GetLineInfo((varVec[x]), lines, lines_len, start);
									auto info2 = GetLineInfo((valVec[x]), lines, lines_len, start);

									nestedUT[braceNum]->AddItem(buffer + GetIdx(varVec[x]), GetLength(varVec[x]), info1,
										buffer + GetIdx(valVec[x]), GetLength(valVec[x]), info2);
									{
										std::string temp = nestedUT[braceNum]->GetItemList(nestedUT[braceNum]->GetItemListSize() - 1).GetName().ToString();
										temp = wiz::load_data::Utility::Convert(std::move(temp));

										nestedUT[braceNum]->GetItemList(nestedUT[braceNum]->GetItemListSize() - 1).SetName(wiz::DataType(temp));
									}
									{
										std::string temp = nestedUT[braceNum]->GetItemList(nestedUT[braceNum]->GetItemListSize() - 1).Get().ToString();
										temp = wiz::load_data::Utility::Convert(std::move(temp));

										nestedUT[braceNum]->GetItemList(nestedUT[braceNum]->GetItemListSize() - 1).Set(0, wiz::DataType(temp));
									}
								}

								varVec.clear();
								valVec.clear();
							}

							UserType temp("");

							nestedUT[braceNum]->AddUserTypeItem(temp);
							UserType* pTemp = nestedUT[braceNum]->GetUserTypeList(nestedUT[braceNum]->GetUserTypeListSize() - 1);

							braceNum++;

							/// new nestedUT
							if (nestedUT.size() == braceNum) { /// changed 2014.01.23..
								nestedUT.push_back(nullptr);
							}

							/// initial new nestedUT.
							nestedUT[braceNum] = pTemp;
							///

							state = 0;
						}
						// Right 2
						else if (len == 1 && (-1 != Equal(TYPE_RIGHT, GetType(token_arr[i])))) {

							state = 0;

							if (!varVec.empty()) {

								{
									nestedUT[braceNum]->ReserveIList(nestedUT[braceNum]->GetIListSize() + varVec.size());
									nestedUT[braceNum]->ReserveItemList(nestedUT[braceNum]->GetItemListSize() + varVec.size());

									for (size_t x = 0; x < varVec.size(); ++x) {
										auto info1 = GetLineInfo(varVec[x], lines, lines_len, start);
										auto info2 = GetLineInfo(valVec[x], lines, lines_len, start);

										nestedUT[braceNum]->AddItem(buffer + GetIdx(varVec[x]), GetLength(varVec[x]), info1,
											buffer + GetIdx(valVec[x]), GetLength(valVec[x]), info2);
										{
											std::string temp = nestedUT[braceNum]->GetItemList(nestedUT[braceNum]->GetItemListSize() - 1).GetName().ToString();
											temp = wiz::load_data::Utility::Convert(std::move(temp));

											nestedUT[braceNum]->GetItemList(nestedUT[braceNum]->GetItemListSize() - 1).SetName(wiz::DataType(temp));
										}
										{
											std::string temp = nestedUT[braceNum]->GetItemList(nestedUT[braceNum]->GetItemListSize() - 1).Get().ToString();
											temp = wiz::load_data::Utility::Convert(std::move(temp));

											nestedUT[braceNum]->GetItemList(nestedUT[braceNum]->GetItemListSize() - 1).Set(0, wiz::DataType(temp));
										}
									}
								}

								varVec.clear();
								valVec.clear();
							}

							if (braceNum == 0) {
								UserType ut;
								ut.AddUserTypeItem(UserType()); // json -> "var_name" = val  // clautext, # is line comment delimiter.
								UserType* pTemp = ut.GetUserTypeList(0);
								pTemp->SetIsVirtual(true);

								if (buffer[GetIdx(token_arr[i])] == option.Right) {
									pTemp->SetIsObject(true); // Object
								}
								else if (buffer[GetIdx(token_arr[i])] == option.Right2) {
									pTemp->SetIsObject(false); // Array
								}

								int utCount = 0;
								int itCount = 0;
								auto max = nestedUT[braceNum]->GetIListSize();
								for (auto i = 0; i < max; ++i) {
									if (nestedUT[braceNum]->IsUserTypeList(i)) {
										ut.GetUserTypeList(0)->AddUserTypeItem(std::move(*(nestedUT[braceNum]->GetUserTypeList(utCount))));
										utCount++;
									}
									else {
										ut.GetUserTypeList(0)->AddItemType(std::move(nestedUT[braceNum]->GetItemList(itCount)));
										itCount++;
									}
								}

								nestedUT[braceNum]->Remove();
								nestedUT[braceNum]->AddUserTypeItem(std::move(*(ut.GetUserTypeList(0))));

								braceNum++;
							}

							{
								if (braceNum < nestedUT.size()) {
									nestedUT[braceNum] = nullptr;
								}
								braceNum--;
							}
						}
						else {
							if (x < token_arr + token_arr_len - 1) {
								long long _len = GetLength(token_arr[i + 1]);
								// EQ 3
								if (_len == 1 && -1 != Equal(TYPE_ASSIGN, GetType(token_arr[i + 1]))) {
									var = token_arr[i];

									state = 1;

									{
										count = 1;
									}
								}
								else {
									// val
									if (x <= token_arr + token_arr_len - 1) {

										val = token_arr[i];

										varVec.push_back(check_syntax_error1(var, err));
										valVec.push_back(check_syntax_error1(val, err));

										val = 0;

										state = 0;

									}
								}
							}
							else
							{
								// var1
								if (x <= token_arr + token_arr_len - 1)
								{
									val = token_arr[i];
									varVec.push_back(check_syntax_error1(var, err));
									valVec.push_back(check_syntax_error1(val, err));
									val = 0;

									state = 0;
								}
							}
						}
					}
					break;
					case 1:
					{
						// LEFT 1
						if (len == 1 && (-1 != Equal(TYPE_LEFT, GetType(token_arr[i])))) {
							nestedUT[braceNum]->ReserveIList(nestedUT[braceNum]->GetIListSize() + varVec.size());
							nestedUT[braceNum]->ReserveItemList(nestedUT[braceNum]->GetItemListSize() + varVec.size());

							for (size_t x = 0; x < varVec.size(); ++x) {
								auto info1 = GetLineInfo((varVec[x]), lines, lines_len, start);
								auto info2 = GetLineInfo((valVec[x]), lines, lines_len, start);

								nestedUT[braceNum]->AddItem(buffer + GetIdx(varVec[x]), GetLength(varVec[x]), info1,
									buffer + GetIdx(valVec[x]), GetLength(valVec[x]), info2);

								{
									std::string temp = nestedUT[braceNum]->GetItemList(nestedUT[braceNum]->GetItemListSize() - 1).GetName().ToString();
									temp = wiz::load_data::Utility::Convert(std::move(temp));

									nestedUT[braceNum]->GetItemList(nestedUT[braceNum]->GetItemListSize() - 1).SetName(wiz::DataType(temp));
								}
								{
									std::string temp = nestedUT[braceNum]->GetItemList(nestedUT[braceNum]->GetItemListSize() - 1).Get().ToString();
									temp = wiz::load_data::Utility::Convert(std::move(temp));

									nestedUT[braceNum]->GetItemList(nestedUT[braceNum]->GetItemListSize() - 1).Set(0, wiz::DataType(temp));
								}
							}


							varVec.clear();
							valVec.clear();

							///
							{
								auto info1 = GetLineInfo((var), lines, lines_len, start);
								nestedUT[braceNum]->AddUserTypeItem(UserType(buffer + GetIdx(var), GetLength(var), info1));

								{
									{
										std::string temp = nestedUT[braceNum]->GetUserTypeList(nestedUT[braceNum]->GetUserTypeListSize() - 1)->GetName().ToString();
										temp = wiz::load_data::Utility::Convert(std::move(temp));

										nestedUT[braceNum]->GetUserTypeList(nestedUT[braceNum]->GetUserTypeListSize() - 1)->SetName(wiz::DataType(temp));
									}
								}

								UserType* pTemp = nestedUT[braceNum]->GetUserTypeList(nestedUT[braceNum]->GetUserTypeListSize() - 1);
								var = 0;
								braceNum++;

								/// new nestedUT
								if (nestedUT.size() == braceNum) {
									nestedUT.push_back(nullptr);
								}

								/// initial new nestedUT.
								nestedUT[braceNum] = pTemp;
							}
							///
							state = 0;
						}
						else {
							if (x <= token_arr + token_arr_len - 1) {
								val = token_arr[i];

								varVec.push_back(check_syntax_error1(var, err));
								valVec.push_back(check_syntax_error1(val, err));
								var = 0; val = 0;

								state = 0;
							}
						}
					}
					break;
					default:
						// syntax err!!
						*err = -1;
						return false; // throw "syntax error ";
						break;
					}
				}

				if (next) {
					*next = nestedUT[braceNum];
				}

				if (varVec.empty() == false) {
					nestedUT[braceNum]->ReserveIList(nestedUT[braceNum]->GetIListSize() + varVec.size());
					nestedUT[braceNum]->ReserveItemList(nestedUT[braceNum]->GetItemListSize() + varVec.size());

					for (size_t x = 0; x < varVec.size(); ++x) {
						auto info1 = GetLineInfo(varVec[x], lines, lines_len, start);
						auto info2 = GetLineInfo(valVec[x], lines, lines_len, start);

						nestedUT[braceNum]->AddItem(buffer + GetIdx(varVec[x]), GetLength(varVec[x]), info1,
							buffer + GetIdx(valVec[x]), GetLength(valVec[x]), info2);
					}


					varVec.clear();
					valVec.clear();
				}

				if (state != last_state) {
					*err = -2;
					return false;
					// throw std::string("error final state is not last_state!  : ") + toStr(state);
				}
				if (x > token_arr + token_arr_len) {
					*err = -3;
					return false;
					//throw std::string("error x > buffer + buffer_len: ");
				}

				return true;
			}


			static long long FindDivisionPlace(const char* buffer, const long long* token_arr, long long start, long long last, const wiz::load_data::LoadDataOption2& option)
			{
				for (long long a = last; a >= start; --a) {
					long long len = GetLength(token_arr[a]);
					long long val = GetType(token_arr[a]);


					if (len == 1 && (-1 != Equal(TYPE_RIGHT, val))) { // right
						return a;
					}

					bool pass = false;
					if (len == 1 && (-1 != Equal(TYPE_LEFT, val))) { // left
						return a;
					}
					else if (len == 1 && -1 != Equal(TYPE_ASSIGN, val)) { // assignment
						//
						pass = true;
					}

					if (a < last && pass == false) {
						long long len = GetLength(token_arr[a + 1]);
						long long val = GetType(token_arr[a + 1]);

						if (!(len == 1 && -1 != Equal(TYPE_ASSIGN, val))) // assignment
						{ // NOT
							return a;
						}
					}
				}
				return -1;
			}

			static bool _LoadData(InFileReserverJson& reserver, UserType& global, wiz::load_data::LoadDataOption2 option, const int lex_thr_num, const int parse_num, bool load_schema) // first, strVec.empty() must be true!!
			{
				const int pivot_num = parse_num - 1;
				char* buffer = nullptr;
				long long* token_arr = nullptr;
				long long buffer_total_len;
				long long token_arr_len = 0;
				long long lines_len = 0;
				long long* lines = nullptr;
				{
					int a = clock();

					bool success = reserver(option, lex_thr_num, buffer, &buffer_total_len, token_arr, &token_arr_len, lines, lines_len, load_schema);


					int b = clock();
				//	std::cout << "scan " << b - a << "ms\n";

					//	{
					//		for (long long i = 0; i < token_arr_len; ++i) {
					//			std::string(buffer + GetIdx(token_arr[i]), GetLength(token_arr[i]));
				//				if (0 == GetIdx(token_arr[i])) {
					//				std::cout << "chk";
					//			}
					//		}
					//	}

					if (!success) {
						return false;
					}
					if (token_arr_len <= 0) {
						if (buffer) {
							delete[] buffer;
						}
						if (token_arr) {
							delete[] token_arr;
						}
						if (lines) {
							free(lines);
						}
						return true;
					}
				}

				UserType* before_next = nullptr;
				UserType _global;

				bool first = true;
				long long sum = 0;

				{
					std::set<long long> _pivots;
					std::vector<long long> pivots;
					const long long num = token_arr_len; //

					if (pivot_num > 0) {
						std::vector<long long> pivot;
						pivots.reserve(pivot_num);
						pivot.reserve(pivot_num);

						for (int i = 0; i < pivot_num; ++i) {
							pivot.push_back(FindDivisionPlace(buffer, token_arr, (num / (pivot_num + 1)) * (i), (num / (pivot_num + 1)) * (i + 1) - 1, option));
						}

						for (size_t i = 0; i < pivot.size(); ++i) {
							if (pivot[i] != -1) {
								_pivots.insert(pivot[i]);
							}
						}

						for (auto& x : _pivots) {
							pivots.push_back(x);
						}
					}

					std::vector<UserType*> next(pivots.size() + 1, nullptr);

					{
						std::vector<UserType> __global(pivots.size() + 1);

						std::vector<std::thread> thr(pivots.size() + 1);
						std::vector<int> err(pivots.size() + 1, 0);
						{
							long long idx = pivots.empty() ? num - 1 : pivots[0];
							long long _token_arr_len = idx - 0 + 1;

							thr[0] = std::thread(__LoadData, buffer, token_arr, _token_arr_len, &__global[0], &option, 0, 0, &next[0], lines,
								lines_len, &err[0]);
						}

						for (size_t i = 1; i < pivots.size(); ++i) {
							long long _token_arr_len = pivots[i] - (pivots[i - 1] + 1) + 1;

							thr[i] = std::thread(__LoadData, buffer, token_arr + pivots[i - 1] + 1, _token_arr_len, &__global[i], &option, 0, 0, &next[i],
								lines, lines_len, &err[i]);

						}

						if (pivots.size() >= 1) {
							long long _token_arr_len = num - 1 - (pivots.back() + 1) + 1;

							thr[pivots.size()] = std::thread(__LoadData, buffer, token_arr + pivots.back() + 1, _token_arr_len, &__global[pivots.size()],
								&option, 0, 0, &next[pivots.size()], lines, lines_len, &err[pivots.size()]);
						}

						// wait
						for (size_t i = 0; i < thr.size(); ++i) {
							thr[i].join();
						}

						for (size_t i = 0; i < err.size(); ++i) {
							switch (err[i]) {
							case 0:
								break;
							case -1:
							case -4:
								std::cout << "Syntax Error\n";
								break;
							case -2:
								std::cout << "error final state is not last_state!\n";
								break;
							case -3:
								std::cout << "error x > buffer + buffer_len:\n";
								break;
								// -4, -5?
							default:
								std::cout << "unknown parser error\n";
								break;
							}
						}

						// Merge
						try {
							if (__global[0].GetUserTypeListSize() > 0 && __global[0].GetUserTypeList(0)->IsVirtual()) {
								std::cout << "not valid file1\n";
								throw 1;
							}
							if (next.back()->GetParent() != nullptr) {
								std::cout << "not valid file2\n";
								throw 2;
							}

							int err = Merge(&_global, &__global[0], &next[0]);
							if (-1 == err || (pivots.size() == 0 && 1 == err)) {
								std::cout << "not valid file3\n";
								throw 3;
							}

							for (size_t i = 1; i < pivots.size() + 1; ++i) {
								// linearly merge and error check...
								int err = Merge(next[i - 1], &__global[i], &next[i]);
								if (-1 == err) {
									std::cout << "not valid file4\n";
									throw 4;
								}
								else if (i == pivots.size() && 1 == err) {
									std::cout << "not valid file5\n";
									throw 5;
								}
							}
						}
						catch (...) {
							delete[] buffer;
							delete[] token_arr;
							free(lines);
							buffer = nullptr;
							throw "in Merge, error";
						}

						before_next = next.back();
					}
				}

				delete[] buffer;
				delete[] token_arr;
				free(lines);

				if (!(_global.GetIListSize() == 1)) {
					return false;
				}

				global = std::move(_global);

				return true;
			}

			static bool _LoadData2(InFileReserverJsonSchema& reserver, UserType& global, wiz::load_data::LoadDataOption2 option, const int lex_thr_num, const int parse_num, bool load_schema) // first, strVec.empty() must be true!!
			{
				const int pivot_num = parse_num - 1;
				char* buffer = nullptr;
				long long* token_arr = nullptr;
				long long buffer_total_len;
				long long token_arr_len = 0;
				long long* lines = nullptr;
				long long lines_len = 0;

				{
					int a = clock();

					bool success = reserver(option, lex_thr_num, buffer, &buffer_total_len, token_arr, &token_arr_len, lines, lines_len);


					int b = clock();
					//	std::cout << "scan " << b - a << "ms\n";

						//	{
						//		for (long long i = 0; i < token_arr_len; ++i) {
						//			std::string(buffer + GetIdx(token_arr[i]), GetLength(token_arr[i]));
					//				if (0 == GetIdx(token_arr[i])) {
						//				std::cout << "chk";
						//			}
						//		}
						//	}

					if (!success) {
						return false;
					}
					if (token_arr_len <= 0) {
						if (buffer) {
							delete[] buffer;
						}
						if (token_arr) {
							delete[] token_arr;
						}
						if (lines) {
							free(lines);
						}
						return true;
					}
				}

				UserType* before_next = nullptr;
				UserType _global;

				bool first = true;
				long long sum = 0;

				{
					std::set<long long> _pivots;
					std::vector<long long> pivots;
					const long long num = token_arr_len; //

					if (pivot_num > 0) {
						std::vector<long long> pivot;
						pivots.reserve(pivot_num);
						pivot.reserve(pivot_num);

						for (int i = 0; i < pivot_num; ++i) {
							pivot.push_back(FindDivisionPlace(buffer, token_arr, (num / (pivot_num + 1)) * (i), (num / (pivot_num + 1)) * (i + 1) - 1, option));
						}

						for (size_t i = 0; i < pivot.size(); ++i) {
							if (pivot[i] != -1) {
								_pivots.insert(pivot[i]);
							}
						}

						for (auto& x : _pivots) {
							pivots.push_back(x);
						}
					}

					std::vector<UserType*> next(pivots.size() + 1, nullptr);

					{
						std::vector<UserType> __global(pivots.size() + 1);

						std::vector<std::thread> thr(pivots.size() + 1);
						std::vector<int> err(pivots.size() + 1, 0);
						{
							long long idx = pivots.empty() ? num - 1 : pivots[0];
							long long _token_arr_len = idx - 0 + 1;

							thr[0] = std::thread(__LoadData, buffer, token_arr, _token_arr_len, &__global[0], &option, 0, 0, &next[0],
								lines, lines_len, &err[0]);
						}

						for (size_t i = 1; i < pivots.size(); ++i) {
							long long _token_arr_len = pivots[i] - (pivots[i - 1] + 1) + 1;

							thr[i] = std::thread(__LoadData, buffer, token_arr + pivots[i - 1] + 1, _token_arr_len, &__global[i], &option, 0, 0, &next[i],
								lines, lines_len, &err[i]);

						}

						if (pivots.size() >= 1) {
							long long _token_arr_len = num - 1 - (pivots.back() + 1) + 1;

							thr[pivots.size()] = std::thread(__LoadData, buffer, token_arr + pivots.back() + 1, _token_arr_len, &__global[pivots.size()],
								&option, 0, 0, &next[pivots.size()], lines, lines_len, &err[pivots.size()]);
						}

						// wait
						for (size_t i = 0; i < thr.size(); ++i) {
							thr[i].join();
						}

						for (size_t i = 0; i < err.size(); ++i) {
							switch (err[i]) {
							case 0:
								break;
							case -1:
							case -4:
								std::cout << "Syntax Error\n";
								break;
							case -2:
								std::cout << "error final state is not last_state!\n";
								break;
							case -3:
								std::cout << "error x > buffer + buffer_len:\n";
								break;
								// -4, -5?
							default:
								std::cout << "unknown parser error\n";
								break;
							}
						}

						// Merge
						try {
							if (__global[0].GetUserTypeListSize() > 0 && __global[0].GetUserTypeList(0)->IsVirtual()) {
								std::cout << "not valid file1\n";
								throw 1;
							}
							if (next.back()->GetParent() != nullptr) {
								std::cout << "not valid file2\n";
								throw 2;
							}

							int err = Merge(&_global, &__global[0], &next[0]);
							if (-1 == err || (pivots.size() == 0 && 1 == err)) {
								std::cout << "not valid file3\n";
								throw 3;
							}

							for (size_t i = 1; i < pivots.size() + 1; ++i) {
								// linearly merge and error check...
								int err = Merge(next[i - 1], &__global[i], &next[i]);
								if (-1 == err) {
									std::cout << "not valid file4\n";
									throw 4;
								}
								else if (i == pivots.size() && 1 == err) {
									std::cout << "not valid file5\n";
									throw 5;
								}
							}
						}
						catch (...) {
							delete[] buffer;
							delete[] token_arr;
							free(lines);
							buffer = nullptr;
							throw "in Merge, error";
						}

						before_next = next.back();
					}
				}

				delete[] buffer;
				delete[] token_arr;
				free(lines);

				if (!(_global.GetIListSize() == 1)) {
					return false;
				}

				global = std::move(_global);

				return true;
			}
		public:
			static bool LoadDataFromFile(const std::string& fileName, UserType& global, int lex_thr_num=0, int parse_thr_num=0, bool load_schema = false) /// global should be empty
			{
				if (lex_thr_num <= 0) {
					lex_thr_num = std::thread::hardware_concurrency();
				}
				if (lex_thr_num <= 0) {
					lex_thr_num = 1;
				}

				if (parse_thr_num <= 0) {
					parse_thr_num = std::thread::hardware_concurrency();
				}
				if (parse_thr_num <= 0) {
					parse_thr_num = 1;
				}

				bool success = true;
				std::ifstream inFile;
				inFile.open(fileName, std::ios::binary);


				if (true == inFile.fail())
				{
					inFile.close(); return false;
				}

				UserType globalTemp;

				try {

					InFileReserverJson ifReserver(inFile);
					wiz::load_data::LoadDataOption2 option;

					char* buffer = nullptr;
					ifReserver.Num = 1 << 19;
					//	strVec.reserve(ifReserver.Num);
					// cf) empty file..
					if (false == _LoadData(ifReserver, globalTemp, option, lex_thr_num, parse_thr_num, load_schema))
					{
						inFile.close();
						return false; // return true?
					}

					inFile.close();
				}
				catch (const char* err) { std::cout << err << "\n"; inFile.close(); return false; }
				catch (const std::string& e) { std::cout << e << "\n"; inFile.close(); return false; }
				catch (const std::exception& e) { std::cout << e.what() << "\n"; inFile.close(); return false; }
				catch (...) { std::cout << "not expected error" << "\n"; inFile.close(); return false; }


				global = std::move(globalTemp);

				return true;
			}
			static bool LoadDataFromFile2(const std::string& fileName, UserType& global, int lex_thr_num = 0, int parse_thr_num = 0, bool load_schema = false) /// global should be empty
			{
				if (lex_thr_num <= 0) {
					lex_thr_num = std::thread::hardware_concurrency();
				}
				if (lex_thr_num <= 0) {
					lex_thr_num = 1;
				}

				if (parse_thr_num <= 0) {
					parse_thr_num = std::thread::hardware_concurrency();
				}
				if (parse_thr_num <= 0) {
					parse_thr_num = 1;
				}

				bool success = true;
				std::ifstream inFile;
				inFile.open(fileName, std::ios::binary);


				if (true == inFile.fail())
				{
					inFile.close(); return false;
				}

				UserType globalTemp;

				try {

					InFileReserverJsonSchema ifReserver(inFile);
					wiz::load_data::LoadDataOption2 option;

					char* buffer = nullptr;
					ifReserver.Num = 1 << 19;
					//	strVec.reserve(ifReserver.Num);
					// cf) empty file..
					if (false == _LoadData2(ifReserver, globalTemp, option, lex_thr_num, parse_thr_num, load_schema))
					{
						inFile.close();
						return false; // return true?
					}

					inFile.close();
				}
				catch (const char* err) { std::cout << err << "\n"; inFile.close(); return false; }
				catch (const std::string& e) { std::cout << e << "\n"; inFile.close(); return false; }
				catch (const std::exception& e) { std::cout << e.what() << "\n"; inFile.close(); return false; }
				catch (...) { std::cout << "not expected error" << "\n"; inFile.close(); return false; }


				global = std::move(globalTemp);

				return true;
			}
			static bool LoadWizDB(UserType& global, const std::string& fileName, const int thr_num) {
				UserType globalTemp = UserType("global");

				// Scan + Parse 
				if (false == LoadDataFromFile(fileName, globalTemp, thr_num, thr_num)) { return false; }
				//std::cout << "LoadData2 End" << "\n";

				global = std::move(globalTemp);
				return true;
			}
			// SaveQuery
			static bool SaveWizDB(const UserType& global, const std::string& fileName, const bool append = false) {
				std::ofstream outFile;
				if (fileName.empty()) { return false; }
				if (false == append) {
					outFile.open(fileName);
					if (outFile.fail()) { return false; }
				}
				else {
					outFile.open(fileName, std::ios::app);
					if (outFile.fail()) { return false; }

					outFile << "\n";
				}


#if _WIN32
				if (65001 == GetConsoleOutputCP()) {
					outFile << (char)0xEF << (char)0xBB << (char)0xBF;
				}
#endif

				/// saveFile
				global.Save1(outFile); // cf) friend

				outFile.close();

				return true;
			}

			static bool SaveWizDB2(const UserType& global, const std::string& fileName, const bool append = false) {
				std::ofstream outFile;
				if (fileName.empty()) { return false; }
				if (false == append) {
					outFile.open(fileName);
					if (outFile.fail()) { return false; }
				}
				else {
					outFile.open(fileName, std::ios::app);
					if (outFile.fail()) { return false; }

					outFile << "\n";
				}

#if _WIN32
				if (65001 == GetConsoleOutputCP()) {
					outFile << (char)0xEF << (char)0xBB << (char)0xBF;
				}
#endif

				/// saveFile
				global.Save2(outFile); // cf) friend

				outFile.close();

				return true;
			}
		};

		class LoadData
		{
		public:
			inline static EXCUTE_MODULE_BASE* pExcuteModule = &clauText;

		public:
			// global is empty?
			static bool LoadDataFromFileWithJson(const std::string& fileName, UserType& global, int scan_thr = 0, int parse_thr = 0) /// global should be empty
			{
				return wiz::load_data::LoadData2::LoadDataFromFile(fileName, global, scan_thr, parse_thr);
			}
			
			// my version schema...
			static bool LoadDataFromFileWithJsonSchema(const std::string& fileName, UserType& global, int scan_thr = 0, int parse_thr = 0) /// global should be empty
			{
				return wiz::load_data::LoadData2::LoadDataFromFile2(fileName, global, scan_thr, parse_thr, true);
			}
			
		public:

			static bool LoadDataFromString(const std::string& str, UserType& ut)
			{
				bool success = true;

				UserType globalTemp;

				try {

					InFileReserver ifReserver(str);
					wiz::load_data::LoadDataOption option;

					//	strVec.reserve(ifReserver.Num);
					// cf) empty file..
					if (false == _LoadData(ifReserver, globalTemp, option, 1, 1))
					{
						return false; // return true?
					}
				}
				catch (const char* err) { std::cout << err << "\n";  return false; }
				catch (const std::string& e) { std::cout << e << "\n";  return false; }
				catch (const std::exception& e) { std::cout << e.what() << "\n";  return false; }
				catch (...) { std::cout << "not expected error" << "\n"; return false; }


				ut = std::move(globalTemp);

				return true;
			}

			enum {
				TYPE_LEFT = 1, // 01
				TYPE_RIGHT = 2, // 10
				TYPE_ASSIGN = 3 // 11
			};
		private:
			static long long check_syntax_error1(long long str, int* err) {
				long long len = GetLength(str);
				long long type = GetType(str);

				if (1 == len && (type == TYPE_LEFT || type == TYPE_RIGHT ||
					type == TYPE_ASSIGN)) {
					*err = -4;
				}
				return str;
			}
		public:
			static int Merge(UserType* next, UserType* ut, UserType** ut_next)
			{
				//check!!
				while (ut->GetIListSize() >= 1 && ut->GetUserTypeListSize() >= 1
					&& (ut->GetUserTypeList(0)->GetName() == "#"))
				{
					ut = ut->GetUserTypeList(0);
				}

				bool chk_ut_next = false;

				while (true) {
					int itCount = 0;
					int utCount = 0;

					UserType* _ut = ut;
					UserType* _next = next;


					if (ut_next && _ut == *ut_next) {
						*ut_next = _next;
						chk_ut_next = true;
					}

					for (int i = 0; i < _ut->GetIListSize(); ++i) {
						if (_ut->IsUserTypeList(i)) {
							if (_ut->GetUserTypeList(utCount)->GetName() == "#") {
								_ut->GetUserTypeList(utCount)->SetName("");
							}
							else {
								{
									_next->LinkUserType(_ut->GetUserTypeList(utCount));
									_ut->GetUserTypeList(utCount) = nullptr;
								}
							}
							utCount++;
						}
						else if (_ut->IsItemList(i)) {
							_next->AddItemType(std::move(_ut->GetItemList(itCount)));
							itCount++;
						}
					}
					_ut->Remove();

					ut = ut->GetParent();
					next = next->GetParent();


					if (next && ut) {
						//
					}
					else {
						// right_depth > left_depth
						if (!next && ut) {
							return -1;
						}
						else if (next && !ut) {
							return 1;
						}

						return 0;
					}
				}
			}
		private:
			static long long GetIdx(long long x) {
				return (x >> 33) & 0x000000007FFFFFFF;
			}
			static long long GetLength(long long x) {
				return (x & 0x00000001FFFFFFF0) >> 4;
			}
			static long long GetType(long long x) { //to enum or enum class?
				return (x & 0xE) >> 1;
			}
		private:
			static bool __LoadData(const char* buffer, const long long* token_arr, long long token_arr_len, UserType* _global, const wiz::load_data::LoadDataOption* _option,
				int start_state, int last_state, UserType** next, int* err)
			{

				std::vector<long long> varVec;
				std::vector<long long> valVec;


				if (token_arr_len <= 0) {
					return false;
				}
				
				UserType& global = *_global;
				wiz::load_data::LoadDataOption option = *_option;

				int state = start_state;
				int braceNum = 0;
				std::vector< UserType* > nestedUT(1);
				long long var = 0, val = 0;

				nestedUT.reserve(10);
				nestedUT[0] = &global;


				long long count = 0;
				const long long* x = token_arr;
				const long long* x_next = x;

				for (long long i = 0; i < token_arr_len; ++i) {
					x = x_next;
					{
						x_next = x + 1;
					}
					if (count > 0) {
						count--;
						continue;
					}
					long long len = GetLength(token_arr[i]);

					switch (state)
					{
					case 0:
					{
						// Left 1
						if (len == 1 && (-1 != Equal(TYPE_LEFT, GetType(token_arr[i])))) {
							if (!varVec.empty()) {
								nestedUT[braceNum]->ReserveIList(nestedUT[braceNum]->GetIListSize() + varVec.size());
								nestedUT[braceNum]->ReserveItemList(nestedUT[braceNum]->GetItemListSize() + varVec.size());

								for (size_t x = 0; x < varVec.size(); ++x) {
									nestedUT[braceNum]->AddItem(buffer + GetIdx(varVec[x]), GetLength(varVec[x]),
										buffer + GetIdx(valVec[x]), GetLength(valVec[x]));
								}

								varVec.clear();
								valVec.clear();
							}

							UserType temp("");

							nestedUT[braceNum]->AddUserTypeItem(temp);
							UserType* pTemp = nullptr;
							nestedUT[braceNum]->GetLastUserTypeItemRef(pTemp);

							braceNum++;

							/// new nestedUT
							if (nestedUT.size() == braceNum) { /// changed 2014.01.23..
								nestedUT.push_back(nullptr);
							}

							/// initial new nestedUT.
							nestedUT[braceNum] = pTemp;
							///

							state = 0;
						}
						// Right 2
						else if (len == 1 && (-1 != Equal(TYPE_RIGHT, GetType(token_arr[i])))) {
							state = 0;

							if (!varVec.empty()) {

								{
									nestedUT[braceNum]->ReserveIList(nestedUT[braceNum]->GetIListSize() + varVec.size());
									nestedUT[braceNum]->ReserveItemList(nestedUT[braceNum]->GetItemListSize() + varVec.size());

									for (size_t x = 0; x < varVec.size(); ++x) {
										nestedUT[braceNum]->AddItem(buffer + GetIdx(varVec[x]), GetLength(varVec[x]),
											buffer + GetIdx(valVec[x]), GetLength(valVec[x]));
									}
								}

								varVec.clear();
								valVec.clear();
							}

							if (braceNum == 0) {
								UserType ut;
								ut.AddUserTypeItem(UserType("#", 1)); // json -> "var_name" = val  // clautext, # is line comment delimiter.
								UserType* pTemp = nullptr;
								ut.GetLastUserTypeItemRef(pTemp);
								int utCount = 0;
								int itCount = 0;
								auto max = nestedUT[braceNum]->GetIListSize();
								for (auto i = 0; i < max; ++i) {
									if (nestedUT[braceNum]->IsUserTypeList(i)) {
										ut.GetUserTypeList(0)->AddUserTypeItem(std::move(*(nestedUT[braceNum]->GetUserTypeList(utCount))));
										utCount++;
									}
									else {
										ut.GetUserTypeList(0)->AddItemType(std::move(nestedUT[braceNum]->GetItemList(itCount)));
										itCount++;
									}
								}

								nestedUT[braceNum]->Remove();
								nestedUT[braceNum]->AddUserTypeItem(std::move(*(ut.GetUserTypeList(0))));

								braceNum++;
							}

							{
								if (braceNum < nestedUT.size()) {
									nestedUT[braceNum] = nullptr;
								}
								braceNum--;
							}
						}
						else {
							if (x < token_arr + token_arr_len - 1) {
								long long _len = GetLength(token_arr[i + 1]);
								// EQ 3
								if (_len == 1 && -1 != Equal(TYPE_ASSIGN, GetType(token_arr[i + 1]))) {
									var = token_arr[i];

									state = 1;

									{
										count = 1;
									}
								}
								else {
									// var1
									if (x <= token_arr + token_arr_len - 1) {

										val = token_arr[i];

										varVec.push_back(check_syntax_error1(var, err));
										valVec.push_back(check_syntax_error1(val, err));

										val = 0;

										state = 0;

									}
								}
							}
							else
							{
								// var1
								if (x <= token_arr + token_arr_len - 1)
								{
									val = token_arr[i];
									varVec.push_back(check_syntax_error1(var, err));
									valVec.push_back(check_syntax_error1(val, err));
									val = 0;

									state = 0;
								}
							}
						}
					}
					break;
					case 1:
					{
						// LEFT 1
						if (len == 1 && (-1 != Equal(TYPE_LEFT, GetType(token_arr[i])))) {
							nestedUT[braceNum]->ReserveIList(nestedUT[braceNum]->GetIListSize() + varVec.size());
							nestedUT[braceNum]->ReserveItemList(nestedUT[braceNum]->GetItemListSize() + varVec.size());

							for (size_t x = 0; x < varVec.size(); ++x) {
								nestedUT[braceNum]->AddItem(buffer + GetIdx(varVec[x]), GetLength(varVec[x]),
									buffer + GetIdx(valVec[x]), GetLength(valVec[x]));
							}


							varVec.clear();
							valVec.clear();

							///
							{
								nestedUT[braceNum]->AddUserTypeItem(UserType(buffer + GetIdx(var), GetLength(var)));
								UserType* pTemp = nullptr;
								nestedUT[braceNum]->GetLastUserTypeItemRef(pTemp);
								var = 0;
								braceNum++;

								/// new nestedUT
								if (nestedUT.size() == braceNum) {
									nestedUT.push_back(nullptr);
								}

								/// initial new nestedUT.
								nestedUT[braceNum] = pTemp;
							}
							///
							state = 0;
						}
						else {
							if (x <= token_arr + token_arr_len - 1) {
								val = token_arr[i];

								varVec.push_back(check_syntax_error1(var, err));
								valVec.push_back(check_syntax_error1(val, err));
								var = 0; val = 0;

								state = 0;
							}
						}
					}
					break;
					default:
						// syntax err!!
						*err = -1;
						return false; // throw "syntax error ";
						break;
					}
				}

				if (next) {
					*next = nestedUT[braceNum];
				}

				if (varVec.empty() == false) {
					nestedUT[braceNum]->ReserveIList(nestedUT[braceNum]->GetIListSize() + varVec.size());
					nestedUT[braceNum]->ReserveItemList(nestedUT[braceNum]->GetItemListSize() + varVec.size());

					for (size_t x = 0; x < varVec.size(); ++x) {
						nestedUT[braceNum]->AddItem(buffer + GetIdx(varVec[x]), GetLength(varVec[x]),
							buffer + GetIdx(valVec[x]), GetLength(valVec[x]));
					}


					varVec.clear();
					valVec.clear();
				}

				if (state != last_state) {
					*err = -2;
					return false;
					// throw std::string("error final state is not last_state!  : ") + toStr(state);
				}
				if (x > token_arr + token_arr_len) {
					*err = -3;
					return false;
					//throw std::string("error x > buffer + buffer_len: ");
				}

				return true;
			}


			static long long FindDivisionPlace(const char* buffer, const long long* token_arr, long long start, long long last, const wiz::load_data::LoadDataOption& option)
			{
				for (long long a = last; a >= start; --a) {
					long long len = GetLength(token_arr[a]);
					long long val = GetType(token_arr[a]);


					if (len == 1 && (-1 != Equal(TYPE_RIGHT, val))) { // right
						return a;
					}

					bool pass = false;
					if (len == 1 && (-1 != Equal(TYPE_LEFT, val))) { // left
						return a;
					}
					else if (len == 1 && -1 != Equal(TYPE_ASSIGN, val)) { // assignment
						//
						pass = true;
					}

					if (a < last && pass == false) {
						long long len = GetLength(token_arr[a + 1]);
						long long val = GetType(token_arr[a + 1]);

						if (!(len == 1 && -1 != Equal(TYPE_ASSIGN, val))) // assignment
						{ // NOT
							return a;
						}
					}
				}
				return -1;
			}

			static bool _LoadData(InFileReserver& reserver, UserType& global, wiz::load_data::LoadDataOption option, const int lex_thr_num, const int parse_num) // first, strVec.empty() must be true!!
			{
				const int pivot_num = parse_num - 1;
				const char* buffer = nullptr;
				long long* token_arr = nullptr;
				long long buffer_total_len;
				long long token_arr_len = 0;

				{
					int a = clock();

					bool success = reserver(option, lex_thr_num, buffer, &buffer_total_len, token_arr, &token_arr_len);


					int b = clock();
					//std::cout << "scan " << b - a << "ms\n";

					//	{
					//		for (long long i = 0; i < token_arr_len; ++i) {
					//			std::string(buffer + GetIdx(token_arr[i]), GetLength(token_arr[i]));
				//				if (0 == GetIdx(token_arr[i])) {
					//				std::cout << "chk";
					//			}
					//		}
					//	}

					if (!success) {
						return false;
					}
					if (token_arr_len <= 0) {
						if (reserver.pInFile) {
							delete[] buffer;
						}
						if (token_arr) {
							delete[] token_arr;
						}
						return true;
					}
				}


				if (token_arr_len < 100) {
					int err;
					UserType _global;
					UserType* next = nullptr;

					__LoadData(buffer, token_arr, token_arr_len, &_global, &option, 0, 0, &next, &err);

					if (reserver.pInFile) {
						delete[] buffer;
					}
					delete[] token_arr;

					global = std::move(_global);

					return true;
				}

				UserType* before_next = nullptr;
				UserType _global;

				bool first = true;
				long long sum = 0;

				{
					std::set<long long> _pivots;
					std::vector<long long> pivots;
					const long long num = token_arr_len; //

					if (pivot_num > 0) {
						std::vector<long long> pivot;
						pivots.reserve(pivot_num);
						pivot.reserve(pivot_num);

						for (int i = 0; i < pivot_num; ++i) {
							pivot.push_back(FindDivisionPlace(buffer, token_arr, (num / (pivot_num + 1)) * (i), (num / (pivot_num + 1)) * (i + 1) - 1, option));
						}

						for (int i = 0; i < pivot.size(); ++i) {
							if (pivot[i] != -1) {
								_pivots.insert(pivot[i]);
							}
						}

						for (auto& x : _pivots) {
							pivots.push_back(x);
						}
					}

					std::vector<UserType*> next(pivots.size() + 1, nullptr);

					{
						std::vector<UserType> __global(pivots.size() + 1);

						std::vector<std::thread> thr(pivots.size() + 1);
						std::vector<int> err(pivots.size() + 1, 0);
						{
							long long idx = pivots.empty() ? num - 1 : pivots[0];
							long long _token_arr_len = idx - 0 + 1;

							thr[0] = std::thread(__LoadData, buffer, token_arr, _token_arr_len, &__global[0], &option, 0, 0, &next[0], &err[0]);
						}

						for (int i = 1; i < pivots.size(); ++i) {
							long long _token_arr_len = pivots[i] - (pivots[i - 1] + 1) + 1;

							thr[i] = std::thread(__LoadData, buffer, token_arr + pivots[i - 1] + 1, _token_arr_len, &__global[i], &option, 0, 0, &next[i], &err[i]);

						}

						if (pivots.size() >= 1) {
							long long _token_arr_len = num - 1 - (pivots.back() + 1) + 1;

							thr[pivots.size()] = std::thread(__LoadData, buffer, token_arr + pivots.back() + 1, _token_arr_len, &__global[pivots.size()],
								&option, 0, 0, &next[pivots.size()], &err[pivots.size()]);
						}

						// wait
						for (int i = 0; i < thr.size(); ++i) {
							thr[i].join();
						}

						for (int i = 0; i < err.size(); ++i) {
							switch (err[i]) {
							case 0:
								break;
							case -1:
							case -4:
								std::cout << "Syntax Error\n";
								break;
							case -2:
								std::cout << "error final state is not last_state!\n";
								break;
							case -3:
								std::cout << "error x > buffer + buffer_len:\n";
								break;
							default:
								std::cout << "unknown parser error\n";
								break;
							}
						}

						// Merge
						try {
							if (__global[0].GetUserTypeListSize() > 0 && __global[0].GetUserTypeList(0)->GetName() == "#") {
								std::cout << "not valid file1\n";
								throw 1;
							}
							if (next.back()->GetParent() != nullptr) {
								std::cout << "not valid file2\n";
								throw 2;
							}

							int err = Merge(&_global, &__global[0], &next[0]);
							if (-1 == err || (pivots.size() == 0 && 1 == err)) {
								std::cout << "not valid file3\n";
								throw 3;
							}

							for (int i = 1; i < pivots.size() + 1; ++i) {
								// linearly merge and error check...
								int err = Merge(next[i - 1], &__global[i], &next[i]);
								if (-1 == err) {
									std::cout << "not valid file4\n";
									throw 4;
								}
								else if (i == pivots.size() && 1 == err) {
									std::cout << "not valid file5\n";
									throw 5;
								}
							}
						}
						catch (...) {
							if (reserver.pInFile) {
								delete[] buffer;
							}
							delete[] token_arr;
							buffer = nullptr;
							throw "in Merge, error";
						}

						before_next = next.back();
					}
				}

				if (reserver.pInFile) {
					delete[] buffer;
				}
				delete[] token_arr;

				global = std::move(_global);

				return true;
			}
		public:
			static bool LoadDataFromFile(const std::string& fileName, UserType& global, int lex_thr_num = 0, int parse_thr_num = 0) /// global should be empty
			{
				if (lex_thr_num <= 0) {
					lex_thr_num = std::thread::hardware_concurrency();
				}
				if (lex_thr_num <= 0) {
					lex_thr_num = 1;
				}

				if (parse_thr_num <= 0) {
					parse_thr_num = std::thread::hardware_concurrency();
				}
				if (parse_thr_num <= 0) {
					parse_thr_num = 1;
				}

				bool success = true;
				std::ifstream inFile;
				inFile.open(fileName, std::ios::binary);


				if (true == inFile.fail())
				{
					inFile.close(); return false;
				}

				UserType globalTemp;

				try {

					InFileReserver ifReserver(inFile);
					wiz::load_data::LoadDataOption option;

					ifReserver.Num = 1 << 19;
					//	strVec.reserve(ifReserver.Num);
					// cf) empty file..
					if (false == _LoadData(ifReserver, globalTemp, option, lex_thr_num, parse_thr_num))
					{
						inFile.close();
						return false; // return true?
					}

					inFile.close();
				}
				catch (const char* err) { std::cout << err << "\n"; inFile.close(); return false; }
				catch (const std::string& e) { std::cout << e << "\n"; inFile.close(); return false; }
				catch (const std::exception& e) { std::cout << e.what() << "\n"; inFile.close(); return false; }
				catch (...) { std::cout << "not expected error" << "\n"; inFile.close(); return false; }


				global = std::move(globalTemp);

				return true;
			}
		
		private:
			UserType global; // ToDo - remove!
		public:
			/// To Do.. static function -> ~.function.!
			// InitQuery or LoadQuery
			explicit LoadData() { InitWizDB(); }

			explicit LoadData(EXCUTE_MODULE_BASE* pExcuteModule) {
				this->pExcuteModule = pExcuteModule;
			}
			/// need testing!
			LoadData(const LoadData& ld)
				: global(ld.global)
			{
				//global = ld.global;
			}
			virtual ~LoadData() { AllRemoveWizDB(); }

			/// need testing!
			LoadData& operator=(const LoadData& ld)
			{
				if (this == &ld) { return *this; }

				global = ld.global;
				return *this;
			}
			//
			bool InitWizDB() {
				return InitWizDB(global);
			}
			// allRemove Query 
			bool AllRemoveWizDB() {
				return AllRemoveWizDB(global);
			}
			// AddQuery AddData, AddUserTypeData
			bool AddData(const std::string& position, const std::string& data,  const ExecuteData& excuteData) {
				return AddData(global, position, data,  excuteData);
			}
			// 
			bool AddNoNameUserType(const std::string& position, const std::string& data,  const ExecuteData& excuteData)
			{
				return AddNoNameUserType(global, position, data,  excuteData);
			}
			// SetQuery
			bool SetData(const std::string& position, const std::string& varName, const std::string& data,  const ExecuteData& excuteData)
			{
				return SetData(global, position, varName, data,  excuteData);
			}
			/// 
			std::string GetData(const std::string& position,  const ExecuteData& excuteData) {
				return GetData(global, position,  excuteData);
			}
			std::string GetItemListData(const std::string& position,  const ExecuteData& excuteData)
			{
				return GetItemListData(global, position,  excuteData);
			}
			std::string GetItemListNamesData(const std::string& position,  const ExecuteData& excuteData)
			{
				return GetItemListNamesData(global, position,  excuteData);
			}
			std::string GetUserTypeListNamesData(const std::string& position,  const ExecuteData& excuteData)
			{
				return GetUserTypeListNamesData(global, position,  excuteData);
			}
			/// varName = val - do
			/// varName = { val val val } - GetData(position+"/varName", ""); 
			/// varName = { var = val } - GetData(position+"/varname", var);
			std::string GetData(const std::string& position, const std::string& varName,  const ExecuteData& excuteData) // 
			{
				return GetData(global, position, varName,  excuteData);
			}
			bool Remove(const std::string& position, const std::string& var,  const ExecuteData& excuteData) {
				return Remove(global, position, var,  excuteData);
			}

			bool LoadWizDB(const std::string& fileName) {
				return LoadWizDB(global, fileName);
			}
			// SaveQuery
			bool SaveWizDB(const std::string& fileName, const std::string& option = "0") { /// , int option
				return SaveWizDB(global, fileName, option);
			}

			/// To Do - ExistItem, ExistUserType, SetUserType GetUserType
			bool ExistData(const std::string& position, const std::string& varName,  const ExecuteData& excuteData) // 
			{
				return ExistData(global, position, varName,  excuteData);
			}

			/// ToDo - recursive function
			std::string SearchItem(const std::string& var,  const ExecuteData& excuteData)
			{
				return SearchItem(global, var,  excuteData, "root");
			}
			std::string SearchUserType(const std::string& var,  const ExecuteData& excuteData)
			{
				return SearchUserType(global, var,  excuteData);
			}

			//cf) /$ and /$/ ??
			static std::string GetRealDir(const std::string& dir, const wiz::load_data::UserType* ut)
			{
				std::vector<std::string> real_dir;
				std::string result;
				const wiz::load_data::UserType* before;

				before = ut;
				ut = ut->GetParent();
				if (ut) {
					for (auto i = 0LL; i < ut->GetUserTypeListSize(); ++i) {
						if (ut->GetUserTypeList(i) == before) {
							real_dir.push_back("$ut" + wiz::toStr(i));
							break;
						}
					}
				}

				while (nullptr != ut) {
					before = ut;
					ut = ut->GetParent();
					if (ut) {
						for (auto i = 0LL; i < ut->GetUserTypeListSize(); ++i) {
							if (ut->GetUserTypeList(i) == before) {
								real_dir.push_back("$ut" + wiz::toStr(i));
								break;
							}
						}
					}
				}

				result += "/./";

				for (int i = real_dir.size() - 1; i >= 0; --i) {
					result += real_dir[i] + "/";
				}

				return result;
			}
			/*
			{
				std::vector<std::string> real_dir;
				std::string result;


				while (nullptr != ut->GetParent()) {
					std::string str = wiz::ToString(ut->GetName());
					
					if (str.empty()) {
						str = "_"; // check!
					}
					
					real_dir.push_back(str);

					ut = ut->GetParent();
				}

				result += "/./";

				for (int i = real_dir.size() - 1; i >= 0; --i) {
					result += real_dir[i] + "/";
				}

				return result;
			}
			*/

			static void _Iterate(Option& opt, UserType& global, const std::string& dir, const std::vector<wiz::load_data::UserType*>& ut, UserType* eventsTemp, const std::string& recursive, const ExecuteData& excuteData)
			{
				for (int i = 0; i < ut.size(); ++i) {
					int itemCount = 0;
					int utCount = 0;

					for (int j = 0; j < ut[i]->GetItemListSize(); ++j) {
						//if (ut[i]->IsItemList(j)) 
						{
							ExecuteData _excuteData;
							//_excuteData.info = excuteData.info;
							_excuteData.pModule = excuteData.pModule;
							_excuteData.pObjectMap = excuteData.pObjectMap;
							_excuteData.pEvents = eventsTemp;
							_excuteData.depth = excuteData.depth;
							_excuteData.noUseInput = excuteData.noUseInput;
							_excuteData.noUseOutput = excuteData.noUseOutput;
							//_excuteData.chkInfo = true;

							
							auto x = eventsTemp->GetUserTypeList(eventsTemp->GetUserTypeListSize() - 1)->GetUserTypeItem("$call");
							for (int k = 0; k < x.size() && k + itemCount < ut[i]->GetItemListSize(); ++k) {
								x[k]->GetItemList(1).Set(0, ut[i]->GetItemList(itemCount + k).GetName());
								x[k]->GetItemList(2).Set(0, ut[i]->GetItemList(itemCount + k).Get(0));
								x[k]->GetItemList(3).Set(0, "FALSE");
								x[k]->GetItemList(4).Set(0, GetRealDir(dir, ut[i]));
								wiz::DataType temp;
								temp.SetInt(j);
								x[k]->GetItemList(5).Set(0, temp);
							}
							std::string result = pExcuteModule->excute_module("Main = { $call = { id = NONE__  } }", &global, _excuteData, opt, 0);

							if (result.empty() == false) {
								UserType resultUT;
								wiz::load_data::LoadData::LoadDataFromString(result, resultUT);

								auto name = resultUT.GetItem("name");
								if (name.empty() == false) {
									ut[i]->GetItemList(itemCount).SetName(ToString(name[0].Get(0)));
								}

								auto value = resultUT.GetItem("value");
								if (value.empty() == false) {
									ut[i]->GetItemList(itemCount).Set(0, value[0].Get(0));
								}
							}

							itemCount++;
							//itemCount += ut[i]->GetItemListSize();
							//j = itemCount - 1;
						}
					}

					for (int j = 0; j < ut[i]->GetUserTypeListSize(); ++j) {
						ExecuteData _excuteData;
						//_excuteData.info = excuteData.info;
						_excuteData.pModule = excuteData.pModule;
						_excuteData.pObjectMap = excuteData.pObjectMap;
						_excuteData.pEvents = eventsTemp;
						_excuteData.depth = excuteData.depth + 1;
						_excuteData.noUseInput = excuteData.noUseInput;
						_excuteData.noUseOutput = excuteData.noUseOutput;
						//_excuteData.chkInfo = true;


						auto x = eventsTemp->GetUserTypeList(eventsTemp->GetUserTypeListSize() - 1)->GetUserTypeItem("$call");
						for (int k = 0; k < x.size(); ++k) {
							x[k]->GetItemList(1).Set(0, ut[i]->GetUserTypeList(utCount)->GetName());
							x[k]->GetItemList(2).Set(0, "NONE"); // check..
							x[k]->GetItemList(3).Set(0, "TRUE");
							const std::string name = ut[i]->GetUserTypeList(utCount)->GetName().ToString();
							x[k]->GetItemList(4).Set(0, GetRealDir(dir + (name.empty() ? "_" : name) + "/", ut[i]->GetUserTypeList(utCount)));
							
							wiz::DataType temp;
							temp.SetInt(j);
							x[k]->GetItemList(5).Set(0, temp);
						}

						Option opt;
 						std::string result = pExcuteModule->excute_module("Main = { $call = { id = NONE__  } }", &global, _excuteData, opt, 0);

						if (result.empty() == false) {
							UserType resultUT;
							wiz::load_data::LoadData::LoadDataFromString(result, resultUT);

							auto name = resultUT.GetItem("name");
							if (name.empty() == false) {
								ut[i]->GetUserTypeList(utCount)->SetName(ToString(name[0].Get(0)));
							}
						}

						//// recursive
						if ("TRUE" == recursive) {
							const std::string name = wiz::ToString(ut[i]->GetUserTypeList(utCount)->GetName());
							_Iterate(opt, global, dir + (name.empty() ? "_" : name) + "/",
								std::vector<UserType*>{ ut[i]->GetUserTypeList(utCount) }, eventsTemp, recursive, excuteData);
						}

						utCount++;
					}
				}
			}
			// new function! - check UserType::Find().second[0] ?
			static void Iterate(wiz::load_data::UserType& global, const std::string& dir, const std::vector<std::string>& events, const std::string& recursive, const std::string& before_value, const ExecuteData& excuteData)
			{
				std::vector<wiz::load_data::UserType*> ut = wiz::load_data::UserType::Find(&global, dir).second; // chk first?
				wiz::load_data::UserType eventsTemp = *excuteData.pEvents;

				Option opt;
				std::string statements2; // = " return_values = { } ";
					
				statements2 += " Event = { id = NONE__  $local = { temp } ";
				statements2 += " $assign = { $local.temp data = { " + before_value + " } } ";

				//for (int t = 0; t < 100; ++t) {
					for (int i = 0; i < events.size(); ++i) {

						statements2 += " $call = { id = ";

						statements2 += events[i];
						statements2 += " name = __name ";
						statements2 += " value = __value ";
						statements2 += " is_user_type = __is_user_type ";
						statements2 += " real_dir = __real_dir ";
						statements2 += " before_value = { $local.temp } ";
						statements2 += " idx = __idx  "; // removal?
						statements2 += " } ";

						statements2 += " $assign = { $local.temp data = { $return_value = { } } } ";
					//	statements2 += " $insert2 = { where = { /./return_values/ } data = { $return_value = { } } } ";
					}
//}
				statements2 += " } ";

				wiz::load_data::LoadData::AddData(eventsTemp, "/root", statements2, ExecuteData());

				_Iterate(opt, global, dir, ut, &eventsTemp, recursive, excuteData);

				eventsTemp.RemoveUserTypeList(eventsTemp.GetUserTypeListSize() - 1);
			//	eventsTemp->RemoveUserTypeList(eventsTemp->GetUserTypeListSize() - 1);
			}
			
			// chk remove!
			static void _Iterate2(UserType& global, const std::string& dir, const std::vector<wiz::load_data::UserType*>& ut, UserType* eventsTemp, const std::string& recursive, const ExecuteData& excuteData)
			{
				for (int i = 0; i < ut.size(); ++i) {
					int itemCount = ut[i]->GetItemListSize();
					int utCount = ut[i]->GetUserTypeListSize();

					for (int j = ut[i]->GetItemListSize() - 1; j >= 0; --j) {
						itemCount--;
						//if (ut[i]->IsItemList(j)) 
						{
							ExecuteData _excuteData;
							//_excuteData.info = excuteData.info;
							_excuteData.pModule = excuteData.pModule;
							_excuteData.pObjectMap = excuteData.pObjectMap;
							_excuteData.pEvents = eventsTemp;
							_excuteData.depth = excuteData.depth;
							_excuteData.noUseInput = excuteData.noUseInput;
							_excuteData.noUseOutput = excuteData.noUseOutput;
							//_excuteData.chkInfo = true;

							auto x = eventsTemp->GetUserTypeList(eventsTemp->GetUserTypeListSize() - 1)->GetUserTypeItem("$call");
							for (int k = 0; k < x.size(); ++k) {
								x[k]->GetItemList(1).Set(0, ut[i]->GetItemList(itemCount).GetName());
								x[k]->GetItemList(2).Set(0, ut[i]->GetItemList(itemCount).Get(0));
								x[k]->GetItemList(3).Set(0, "FALSE");
								x[k]->GetItemList(4).Set(0, GetRealDir(dir, ut[i]));
							}

							Option opt;
							std::string result = pExcuteModule->excute_module("Main = { $call = { id = NONE__  } }", &global, _excuteData, opt, 0);

							if (result.empty() == false) {
								UserType resultUT;
								wiz::load_data::LoadData::LoadDataFromString(result, resultUT);

								auto name = resultUT.GetItem("name");
								if (name.empty() == false) {
									ut[i]->GetItemList(itemCount).SetName(ToString(name[0].Get(0)));
								}

								auto value = resultUT.GetItem("value");
								if (value.empty() == false) {
									ut[i]->GetItemList(itemCount).Set(0, value[0].Get(0));
								}

								auto remove = resultUT.GetItem("remove");
								if (remove.empty() == false && remove[0].Get(0) == "TRUE") {
									ut[i]->RemoveItemList(itemCount);
								}
							}
						}
					}
					for (int j = ut[i]->GetUserTypeListSize() - 1; j >= 0; --j) {
						utCount--;

						ExecuteData _excuteData;
						//_excuteData.info = excuteData.info;
						_excuteData.pModule = excuteData.pModule;
						_excuteData.pObjectMap = excuteData.pObjectMap;
						_excuteData.pEvents = eventsTemp;
						_excuteData.depth = excuteData.depth + 1;
						_excuteData.noUseInput = excuteData.noUseInput;
						_excuteData.noUseOutput = excuteData.noUseOutput;
						//_excuteData.chkInfo = true;


						auto x = eventsTemp->GetUserTypeList(eventsTemp->GetUserTypeListSize() - 1)->GetUserTypeItem("$call");
						for (int k = 0; k < x.size(); ++k) {
							x[k]->GetItemList(1).Set(0, ut[i]->GetUserTypeList(utCount)->GetName());
							x[k]->GetItemList(2).Set(0, "NONE"); // check..
							x[k]->GetItemList(3).Set(0, "TRUE");
							const std::string name = ut[i]->GetUserTypeList(utCount)->GetName().ToString();
							x[k]->GetItemList(4).Set(0, GetRealDir(dir + (name.empty() ? "_" : name) + "/", ut[i]->GetUserTypeList(utCount)));
						}

						Option opt;
						std::string result = pExcuteModule->excute_module("Main = { $call = { id = NONE__  } }", &global, _excuteData, opt, 0);

						if (result.empty() == false) {
							UserType resultUT;
							wiz::load_data::LoadData::LoadDataFromString(result, resultUT);

							auto name = resultUT.GetItem("name");
							if (name.empty() == false) {
								ut[i]->GetUserTypeList(utCount)->SetName(ToString(name[0].Get(0)));
							}

							auto remove = resultUT.GetItem("remove");
							if (remove.empty() == false && remove[0].Get(0) == "TRUE") {
								ut[i]->RemoveUserTypeList(utCount);
								continue;
							}
						}

						//// recursive
						if ("TRUE" == recursive) {
							const std::string name = wiz::ToString(ut[i]->GetUserTypeList(utCount)->GetName());
							_Iterate2(global, dir + (name.empty() ? "_" : name) + "/",
								std::vector<UserType*>{ ut[i]->GetUserTypeList(utCount) }, eventsTemp, recursive, excuteData);
						}
					}
				}
			}
			// new function! - check UserType::Find().second[0] ?
			// todo - beforevalue! - as parameter.
			static void Iterate2(wiz::load_data::UserType& global, const std::string& dir, const std::vector<std::string>& events, const std::string& recursive, const ExecuteData& excuteData)
			{
				std::vector<wiz::load_data::UserType*> ut = wiz::load_data::UserType::Find(&global, dir).second; // chk first?
				wiz::load_data::UserType eventsTemp = *excuteData.pEvents;

				std::string statements2 = " Event = { id = NONE__  $local = { temp } ";
				statements2 += " $assign = { $local.temp data = { empty } } ";

				for (int i = 0; i < events.size(); ++i) {

					statements2 += " $call = { id = ";

					statements2 += events[i];
					statements2 += " name = __name ";
					statements2 += " value = __value ";
					statements2 += " is_user_type = __is_user_type ";
					statements2 += " real_dir = __real_dir ";
					statements2 += " before_value = { $local.temp } ";
					statements2 += " } ";

					statements2 += " $assign = { $local.temp data = { $return_value = { } } } ";
				}
				statements2 += " } ";

				wiz::load_data::LoadData::AddData(eventsTemp, "/root", statements2, ExecuteData());

				_Iterate2(global, dir, ut, &eventsTemp, recursive, excuteData);

				eventsTemp.RemoveUserTypeList(eventsTemp.GetUserTypeListSize() - 1);

			}
			
			// using dir, name
			static void _Iterate3(Option& opt, UserType& global, const std::string& dir, const std::string& name, const std::vector<wiz::load_data::UserType*>& ut, UserType* eventsTemp, const std::string& recursive, const ExecuteData& excuteData)
			{
				for (int i = 0; i < ut.size(); ++i) {
					std::vector<int> itemData = ut[i]->GetItemPtr(name);
					std::vector<int> utData = ut[i]->GetUserTypeItemPtr(name);
					int itemNum = 0;
					int utNum = 0;
					for (int j = 0; j < itemData.size(); ++j) {
						//if (ut[i]->IsItemList(j)) 
						{
							ExecuteData _excuteData;
							//_excuteData.info = excuteData.info;
							_excuteData.pModule = excuteData.pModule;
							_excuteData.pObjectMap = excuteData.pObjectMap;
							_excuteData.pEvents = eventsTemp;
							_excuteData.depth = excuteData.depth;
							_excuteData.noUseInput = excuteData.noUseInput;
							_excuteData.noUseOutput = excuteData.noUseOutput;
							//_excuteData.chkInfo = true;

							auto x = eventsTemp->GetUserTypeList(eventsTemp->GetUserTypeListSize() - 1)->GetUserTypeItem("$call");
							for (int k = 0; k < x.size(); ++k) {
								x[k]->GetItemList(1).Set(0, ut[i]->GetItemList(itemData[itemNum]).GetName());
								x[k]->GetItemList(2).Set(0, ut[i]->GetItemList(itemData[itemNum]).Get(0));
								x[k]->GetItemList(3).Set(0, "FALSE");
								x[k]->GetItemList(4).Set(0, GetRealDir(dir, ut[i]));
								wiz::DataType temp;
								temp.SetInt(itemData[itemNum]);
								x[k]->GetItemList(5).Set(0, temp);
							}
							std::string result = pExcuteModule->excute_module("Main = { $call = { id = NONE__  } }", &global, _excuteData, opt, 0);

							if (result.empty() == false) {
								UserType resultUT;
								wiz::load_data::LoadData::LoadDataFromString(result, resultUT);

								auto name = resultUT.GetItem("name");
								if (name.empty() == false) {
									ut[i]->GetItemList(itemData[itemNum]).SetName(ToString(name[0].Get(0)));
								}

								auto value = resultUT.GetItem("value");
								if (value.empty() == false) {
									ut[i]->GetItemList(itemData[itemNum]).Set(0, value[0].Get(0));
								}
							}

							itemNum++;
						}
					}

					for (int j = 0; j < ut[i]->GetUserTypeListSize(); ++j) {
						ExecuteData _excuteData;
						//_excuteData.info = excuteData.info;
						_excuteData.pModule = excuteData.pModule;
						_excuteData.pObjectMap = excuteData.pObjectMap;
						_excuteData.pEvents = eventsTemp;
						_excuteData.depth = excuteData.depth + 1;
						_excuteData.noUseInput = excuteData.noUseInput;
						_excuteData.noUseOutput = excuteData.noUseOutput;
						//_excuteData.chkInfo = true;

						if (j == utData[utNum]) {
							auto x = eventsTemp->GetUserTypeList(eventsTemp->GetUserTypeListSize() - 1)->GetUserTypeItem("$call");
							for (int k = 0; k < x.size(); ++k) {
								x[k]->GetItemList(1).Set(0, ut[i]->GetUserTypeList(utData[utNum])->GetName());
								x[k]->GetItemList(2).Set(0, "NONE"); // check..
								x[k]->GetItemList(3).Set(0, "TRUE");
								const std::string name = ut[i]->GetUserTypeList(utData[utNum])->GetName().ToString();
								x[k]->GetItemList(4).Set(0, GetRealDir(dir + (name.empty() ? "_" : name) + "/", ut[i]->GetUserTypeList(utData[utNum])));

								wiz::DataType temp;
								temp.SetInt(utData[utNum]);
								x[k]->GetItemList(5).Set(0, temp);
							}

							Option opt;
							std::string result = pExcuteModule->excute_module("Main = { $call = { id = NONE__  } }", &global, _excuteData, opt, 0);

							if (result.empty() == false) {
								UserType resultUT;
								wiz::load_data::LoadData::LoadDataFromString(result, resultUT);

								auto name = resultUT.GetItem("name");
								if (name.empty() == false) {
									ut[i]->GetUserTypeList(utData[utNum])->SetName(ToString(name[0].Get(0)));
								}
							}

							utNum++;
						}

						//// recursive
						if ("TRUE" == recursive) {
							const std::string _name = wiz::ToString(ut[i]->GetUserTypeList(j)->GetName());
							_Iterate3(opt, global, dir + (_name.empty() ? "_" : _name) + "/", name,
								std::vector<UserType*>{ ut[i]->GetUserTypeList(j) }, eventsTemp, recursive, excuteData);
						}
					}
				}
			}
			// new function! - check UserType::Find().second[0] ?
			static void Iterate3(wiz::load_data::UserType& global, const std::string& dir, const std::string& name, const std::vector<std::string>& events, const std::string& recursive, const std::string& before_value, const ExecuteData& excuteData)
			{
				std::vector<wiz::load_data::UserType*> ut = wiz::load_data::UserType::Find(&global, dir).second; // chk first?
				wiz::load_data::UserType eventsTemp = *excuteData.pEvents;

				Option opt;
				std::string statements2; // = " return_values = { } ";

				statements2 += " Event = { id = NONE__  $local = { temp } ";
				statements2 += " $assign = { $local.temp data = { " + before_value + " } } ";

				//for (int t = 0; t < 100; ++t) {
				for (int i = 0; i < events.size(); ++i) {

					statements2 += " $call = { id = ";

					statements2 += events[i];
					statements2 += " name = __name ";
					statements2 += " value = __value ";
					statements2 += " is_user_type = __is_user_type ";
					statements2 += " real_dir = __real_dir ";
					statements2 += " before_value = { $local.temp } ";
					statements2 += " idx = __idx  ";
					statements2 += " } ";

					statements2 += " $assign = { $local.temp data = { $return_value = { } } } ";
					//	statements2 += " $insert2 = { where = { /./return_values/ } data = { $return_value = { } } } ";
				}
				//}
				statements2 += " } ";

				wiz::load_data::LoadData::AddData(eventsTemp, "/root", statements2, ExecuteData());

				_Iterate3(opt, global, dir, name, ut, &eventsTemp, recursive, excuteData);

				eventsTemp.RemoveUserTypeList(eventsTemp.GetUserTypeListSize() - 1);
				//	eventsTemp->RemoveUserTypeList(eventsTemp->GetUserTypeListSize() - 1);
			}


			
			static void _RIterate(UserType& global, const std::string& dir, const std::vector<wiz::load_data::UserType*>& ut, UserType* eventsTemp, const std::string& recursive, const ExecuteData& excuteData)
			{
				for (int i = 0; i < ut.size(); ++i) {
					int itemCount = ut[i]->GetItemListSize();
					int utCount = ut[i]->GetUserTypeListSize();

					for (int j = ut[i]->GetItemListSize() - 1; j >= 0; --j) {
						itemCount--;
						//if (ut[i]->IsItemList(j)) 
						{
							ExecuteData _excuteData;
							//_excuteData.info = excuteData.info;
							_excuteData.pModule = excuteData.pModule;
							_excuteData.pObjectMap = excuteData.pObjectMap;
							_excuteData.pEvents = eventsTemp;
							_excuteData.depth = excuteData.depth;
							_excuteData.noUseInput = excuteData.noUseInput;
							_excuteData.noUseOutput = excuteData.noUseOutput;
							//_excuteData.chkInfo = true;

							auto x = eventsTemp->GetUserTypeList(eventsTemp->GetUserTypeListSize() -
								(eventsTemp->GetUserTypeListSize() - excuteData.pEvents->GetUserTypeListSize()))->GetUserTypeItem("$call");
							for (int k = 0; k < x.size(); ++k) {
								x[k]->GetItemList(1).Set(0, ut[i]->GetItemList(itemCount).GetName());
								x[k]->GetItemList(2).Set(0, ut[i]->GetItemList(itemCount).Get(0));
								x[k]->GetItemList(3).Set(0, "FALSE");
								x[k]->GetItemList(4).Set(0, GetRealDir(dir, ut[i]));
							}

							Option opt;
							std::string result = pExcuteModule->excute_module("Main = { $call = { id = NONE__  } }", &global, _excuteData, opt, 0);

							if (result.empty() == false) {
								UserType resultUT;
								wiz::load_data::LoadData::LoadDataFromString(result, resultUT);

								auto name = resultUT.GetItem("name");
								if (name.empty() == false) {
									ut[i]->GetItemList(itemCount).SetName(ToString(name[0].Get(0)));
								}

								auto value = resultUT.GetItem("value");
								if (value.empty() == false) {
									ut[i]->GetItemList(itemCount).Set(0, value[0].Get(0));
								}
							}
						}
					}
					for (int j = ut[i]->GetUserTypeListSize() - 1; j >= 0; --j) {
						utCount--;

						ExecuteData _excuteData;
						//_excuteData.info = excuteData.info;
						_excuteData.pModule = excuteData.pModule;
						_excuteData.pObjectMap = excuteData.pObjectMap;
						_excuteData.pEvents = eventsTemp;
						_excuteData.depth = excuteData.depth + 1;
						_excuteData.noUseInput = excuteData.noUseInput;
						_excuteData.noUseOutput = excuteData.noUseOutput;
						//_excuteData.chkInfo = true;


						auto x = eventsTemp->GetUserTypeList(eventsTemp->GetUserTypeListSize() - 1)->GetUserTypeItem("$call");
						for (int k = 0; k < x.size(); ++k) {
							x[k]->GetItemList(1).Set(0, ut[i]->GetUserTypeList(utCount)->GetName());
							x[k]->GetItemList(2).Set(0, "NONE"); // check..
							x[k]->GetItemList(3).Set(0, "TRUE");
							const std::string name = ut[i]->GetUserTypeList(utCount)->GetName().ToString();
							x[k]->GetItemList(4).Set(0, GetRealDir(dir + (name.empty() ? "_" : name) + "/", ut[i]->GetUserTypeList(utCount)));
						}

						Option opt;
						std::string result = pExcuteModule->excute_module("Main = { $call = { id = NONE__  } }", &global, _excuteData, opt, 0);

						if (result.empty() == false) {
							UserType resultUT;
							wiz::load_data::LoadData::LoadDataFromString(result, resultUT);

							auto name = resultUT.GetItem("name");
							if (name.empty() == false) {
								ut[i]->GetUserTypeList(utCount)->SetName(ToString(name[0].Get(0)));
							}
						}

						//// recursive
						if ("TRUE" == recursive) {
							const std::string name = wiz::ToString(ut[i]->GetUserTypeList(utCount)->GetName());
							_RIterate(global, dir + (name.empty() ? "_" : name) + "/",
								std::vector<UserType*>{ ut[i]->GetUserTypeList(utCount) }, eventsTemp, recursive, excuteData);
						}
					}
				}
			}

			// new function! - check UserType::Find().second[0] ?
			static void RIterate(wiz::load_data::UserType& global, const std::string& dir, const std::vector<std::string>& events, const std::string& recursive, const ExecuteData& excuteData)
			{
				std::vector<wiz::load_data::UserType*> ut = wiz::load_data::UserType::Find(&global, dir).second; // chk first?
				wiz::load_data::UserType eventsTemp = *excuteData.pEvents;

				std::string statements2 = " Event = { id = NONE__  $local = { temp } ";
				statements2 += " $assign = { $local.temp data = { empty } } ";

				for (int i = 0; i < events.size(); ++i) {

					statements2 += " $call = { id = ";

					statements2 += events[i];
					statements2 += " name = __name ";
					statements2 += " value = __value ";
					statements2 += " is_user_type = __is_user_type ";
					statements2 += " real_dir = __real_dir ";
					statements2 += " before_value = { $local.temp } ";
					statements2 += " } ";

					statements2 += " $assign = { $local.temp data = { $return_value = { } } } ";
				}
				statements2 += " } ";

				wiz::load_data::LoadData::AddData(eventsTemp, "/root", statements2, ExecuteData());

				_RIterate(global, dir, ut, &eventsTemp, recursive, excuteData);

				eventsTemp.RemoveUserTypeList(eventsTemp.GetUserTypeListSize() - 1);
			}
		private:
			void SearchItem(std::vector<std::string>& positionVec, const std::string& var, const std::string& nowPosition,
				UserType* ut,  const ExecuteData& excuteData)
			{
				SearchItem(global, positionVec, var, nowPosition, ut,  excuteData);
			}
			void SearchUserType(std::vector<std::string>& positionVec, const std::string& var, const std::string& nowPosition,
				UserType* ut,  const ExecuteData& excuteData)
			{
				SearchUserType(global, positionVec, var, nowPosition, ut,  excuteData);
			}
		private:
			// chk - Search(item or usertype) : add ~~~ (option?)?? and ToBool4? // chk more thinking!!
			static void SearchItem(UserType& global, std::vector<std::string>& positionVec, const std::string& var, const std::string& nowPosition,
				UserType* ut,  const ExecuteData& excuteData)
			{
				std::string _var = var;
				if (_var == " " || _var == "_") { _var = ""; }

				for (int i = 0; i < ut->GetUserTypeListSize(); ++i) {
					std::string temp = wiz::ToString(ut->GetUserTypeList(i)->GetName());
					if (temp == "") { temp = " "; }
					SearchItem(
						global,
						positionVec,
						_var,
						nowPosition + "/" + temp,
						ut->GetUserTypeList(i),
						excuteData
					);
				}
			}

			static void SearchUserType(UserType& global, std::vector<std::string>& positionVec, const std::string& var, const std::string& nowPosition,
				UserType* ut,  const ExecuteData& excuteData)
			{
				std::string _var = var;
				if (_var == " " || _var == "_") {
					_var = "";
				}
				

				for (int i = 0; i < ut->GetUserTypeListSize(); ++i) {
					std::string temp = wiz::ToString(ut->GetUserTypeList(i)->GetName());

					if (temp == "") { temp = " "; }
					SearchUserType(
						global,
						positionVec,
						_var,
						nowPosition + "/" + temp,
						ut->GetUserTypeList(i),
						excuteData
					);
				}
			}

		public:
			static bool InitWizDB(UserType& global) {
				global = UserType("global");
				return true;
			}
			// allRemove Query 
			static bool AllRemoveWizDB(UserType& global) {
				global = UserType("");
				return true;
			}
			// AddQuery AddData, AddUserTypeData
			static bool AddDataAtFront(UserType& global, const std::string& position, const std::string& data,  const ExecuteData& excuteData) {
				UserType utTemp = UserType("global");
				bool isTrue = false;

				if (false == LoadDataFromString(data, utTemp))
				{
					return false;
				}
				auto finded = UserType::Find(&global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						int item_n = utTemp.GetItemListSize();
						int user_n = utTemp.GetUserTypeListSize();

						for (int k = utTemp.GetIListSize() - 1; k >= 0; --k) {
							if (utTemp.IsItemList(k)) {
								finded.second[i]->AddItemAtFront(wiz::ToString(utTemp.GetItemList(item_n - 1).GetName()), ToString(utTemp.GetItemList(item_n - 1).Get(0)));
								item_n--;
							}
							else {
								finded.second[i]->AddUserTypeItemAtFront(*utTemp.GetUserTypeList(user_n - 1));
								user_n--;
							}
						}
						isTrue = true;
					}
					return isTrue;
				}
				else {
					UserType::Find(&global, position);
					return false;
				}
			}
			static bool AddData(UserType& global, const std::string& position, const std::string& data,  const ExecuteData& excuteData) {
				UserType utTemp = UserType("global");
				bool isTrue = false;

				if (false == LoadDataFromString(data, utTemp))
				{
					return false;
				}
				auto finded = UserType::Find(&global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						int item_n = 0;
						int user_n = 0;

						for (int k = 0; k < utTemp.GetIListSize(); ++k) {
							if (utTemp.IsItemList(k)) {
								finded.second[i]->AddItem(utTemp.GetItemList(item_n).GetName(), utTemp.GetItemList(item_n).Get(0));
								item_n++;
							}
							else {
								finded.second[i]->AddUserTypeItem(*utTemp.GetUserTypeList(user_n));
								user_n++;
							}
						}
						isTrue = true;
					}
					return isTrue;
				}
				else {
					return false;
				}
			}
			static bool Insert(UserType& global, const std::string& position, const int idx, const std::string& data,  const ExecuteData& excuteData) {
				UserType utTemp = UserType("global");
				bool isTrue = false;

				if (false == LoadDataFromString(data, utTemp))
				{
					return false;
				}
				auto finded = UserType::Find(&global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						int item_n = utTemp.GetIListSize();
						int user_n = utTemp.GetUserTypeListSize();

						
						for (int k = utTemp.GetIListSize() - 1; k >= 0; --k) {
							if (utTemp.IsItemList(k)) {
								finded.second[i]->InsertItemByIlist(idx, wiz::ToString(utTemp.GetItemList(item_n - 1).GetName()),
									ToString(utTemp.GetItemList(item_n - 1).Get(0)));
								item_n--;
							}
							else {
								finded.second[i]->InsertUserTypeByIlist(idx, *utTemp.GetUserTypeList(user_n - 1));
								user_n--;
							}
						}
						isTrue = true;
					}
					return isTrue;
				}
				else {
					return false;
				}
			}
			static bool AddNoNameUserType(UserType& global, const std::string& position, const std::string& data,  const ExecuteData& excuteData)
			{
				UserType utTemp = UserType("");
				bool isTrue = false;

				if (false == LoadDataFromString(data, utTemp))
				{
					return false;
				}
				auto finded = UserType::Find(&global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						int item_n = 0;
						int user_n = 0;

					
						finded.second[i]->AddUserTypeItem(utTemp);

						isTrue = true;
					}
					return isTrue;
				}
				else {
					return false;
				}
			}

			// todo - find example code?  a/b/c/d/e/f/ ??
			static bool AddUserType(UserType& global, const std::string& position, const std::string& var, const std::string& data,  const ExecuteData& excuteData)
			{
				bool isTrue = false;
				auto finded = UserType::Find(&global, position);
				if (finded.first) {
					std::string temp = var;
					if (temp == "") { temp = " "; }
					std::vector<std::string> tokens = tokenize(temp, '/');
					UserType utTemp = UserType("");
					if (false == LoadDataFromString(data, utTemp))
					{
						return false;
					}

					for (size_t i = 0; i < tokens.size(); ++i) {
						std::string utName = tokens[i];

						std::vector<std::string> strVec;
						
						if (utName == " " || utName == "_") { utName = ""; }

						if (utName.size() >= 3 && utName[0] == '[' && utName[utName.size() - 1] == ']')
						{
							std::vector<std::string> tokens2 = tokenize(utName.substr(1, utName.size() - 2), '~');

							strVec.push_back(tokens2[0]);
							strVec.push_back(tokens2[1]);
						}

						long long int a = 0, b = 0, Min = 0, Max = 0;
						bool chkInt = false;

						if (strVec.size() == 2 && Utility::IsInteger(strVec[0]) && Utility::IsInteger(strVec[1])) {
							chkInt = true;
							a = atoll(strVec[0].c_str());
							b = atoll(strVec[1].c_str());
							Min = std::min(a, b);
							Max = std::max(a, b);
						}

						for (auto x = Min; x <= Max; ++x)
						{
							if (strVec.size() == 2 && chkInt)
							{
								utName = std::to_string(x);
							}
							else {}
							utTemp.SetName(utName);

							for (int i = 0; i < finded.second.size(); ++i) {
								int item_n = 0;
								int user_n = 0;

								finded.second[i]->AddUserTypeItem(utTemp);

								isTrue = true; // chk!!
							}

							// prevent from infinity loop.
							if (x == Max) { break; }
						}
					}
					return isTrue;
				}
				else {
					return false;
				}
			}
			/// SetData - Re Do!
			static bool SetData(UserType& global, const std::string& position, const std::string& varName, const std::string& data,  const ExecuteData& excuteData)
			{
				auto finded = UserType::Find(&global, position);
				bool isTrue = false;

				if (finded.first) {
					std::string temp = varName;
					if (temp == "") { temp = " "; }
					auto tokenizer = tokenize(temp, '/');
					UserType utTemp("");
					if (false == LoadDataFromString(data, utTemp)) {
						return false;
					}
					
					for(size_t i =0 ; i < tokenizer.size(); ++i) {
						std::string _varName = tokenizer[i];
						/// todo - if varName is "" then data : val val val ... 
						if (_varName == "" || _varName == " " || _varName == "_") { // re?
							const int n = utTemp.GetItem("").size();
							for (int i = 0; i < finded.second.size(); ++i) {
								finded.second[i]->RemoveItemList("");

								for (int j = 0; j < n; ++j) {
									finded.second[i]->AddItem("", utTemp.GetItem("")[j].Get(0));
								}
								isTrue = true;
							}
						}
						else {
							std::vector<std::string> strVec;

							if (_varName.size() >= 3 && _varName[0] == '[' && _varName[_varName.size() - 1] == ']')
							{
								std::vector<std::string> tokens2 = tokenize(_varName.substr(1, _varName.size() - 2), '~');

								strVec.push_back(tokens2[0]);
								strVec.push_back(tokens2[1]);
							}

							long long int a = 0, b = 0, Min = 0, Max = 0;
							bool chkInt = false;

							if (strVec.size() == 2 && Utility::IsInteger(strVec[0]) && Utility::IsInteger(strVec[1])) {
								chkInt = true;
								a = atoll(strVec[0].c_str());
								b = atoll(strVec[1].c_str());
								Min = std::min(a, b);
								Max = std::max(a, b);
							}
							for (long long x = Min; x <= Max; ++x) {
								if (strVec.size() == 2 && chkInt)
								{
									_varName = std::to_string(x);
								}
								else {}

								for (int i = 0; i < finded.second.size(); ++i) {
									if (wiz::String::startsWith(_varName, "$it")) {
										int index = atoi(wiz::String::substring(_varName, 3).c_str());

										finded.second[i]->SetItem(index, data);
										isTrue = true;
									}
									else {
										finded.second[i]->SetItem(_varName, data); /// chk
										isTrue = true;
									}
								}

								// prevent from infinity loop.
								if (x == Max) { break; }
							}
						}
					}
					return isTrue;
				}
				else {
					return false;
				}
			}

			static bool SetData(UserType& global, const std::string& position, const int var_idx, const std::string& data,  const ExecuteData& excuteData)
			{
				auto finded = UserType::Find(&global, position);
				bool isTrue = false;

				if (finded.first) {
					UserType utTemp("");
					if (false == LoadDataFromString(data, utTemp)) {
						return false;
					}
					long long int a = 0, b = 0, Min = 0, Max = 0;

					for (long long x = Min; x <= Max; ++x) {
						for (int i = 0; i < finded.second.size(); ++i) {
							finded.second[i]->SetItem(var_idx, data); /// chk
							isTrue = true;
						}

						// prevent from infinity loop.
						if (x == Max) { break; }
					}
					return isTrue;
				}
				else {
					return false;
				}
			}
			/// 
			static std::string GetData(UserType& global, const std::string& position,  const ExecuteData& excuteData) {
				std::string str;
				auto finded = UserType::Find(&global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						str = str + finded.second[i]->ToString() + "\n";
					}
					return str;
				}
				else {
					return "";
				}
			}
			static std::string GetItemListData(UserType& global, const std::string& position,  const ExecuteData& excuteData)
			{
				std::string str;
				auto finded = UserType::Find(&global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						str = str + finded.second[i]->ItemListToString() + "\n";
					}
					return str;
				}
				else {
					return "";
				}
			}
			static std::string GetItemListNamesData(UserType& global, const std::string& position,  const ExecuteData& excuteData)
			{
				std::string str;
				auto finded = UserType::Find(&global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						str = str + finded.second[i]->ItemListNamesToString() + "\n";
					}
					return str;
				}
				else {
					return "";
				}
			}
			static std::string GetUserTypeListNamesData(UserType& global, const std::string& position,  const ExecuteData& excuteData)
			{
				std::string str;
				auto finded = UserType::Find(&global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						str = str + finded.second[i]->UserTypeListNamesToString() + "\n";
					}
					return str;
				}
				else {
					return "";
				}
			}
			/// varName = val - do
			/// varName = { val val val } - GetData(position+"/varName", ""); 
			/// varName = { var = val } - GetData(position+"/varname", var);
			static std::string GetData(UserType& global, const std::string& position, const std::string& varName,  const ExecuteData& excuteData) // 
			{
				std::string str;
				std::string _var = varName;
				if (_var == " " || _var == "_") { _var = ""; }

				auto finded = UserType::Find(&global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
					
						if (wiz::String::startsWith(_var, "$it")) {
							int itemIdx = stoi(wiz::String::substring(_var, 3));

							str = str + ToString(finded.second[i]->GetItemList(itemIdx).Get(0)) + "\n";
						}
						else {
							const int num = finded.second[i]->GetItem(_var).size();
							for (int k = 0; k < num; ++k) {
								str = str + ToString(finded.second[i]->GetItem(_var)[k].Get(0)) + "\n";
							}
						}
					}
				}
				return str;
			}

			static bool Remove(UserType& global, const std::string& position, const std::string& var,  const ExecuteData& excuteData) {
				auto finded = UserType::Find(&global, position);
				bool isTrue = false;

				if (finded.first) {
					std::string temp = var;
					if (temp == "") { temp = " "; }
					auto tokens = tokenize(temp, '/');
					for (size_t i = 0; i < tokens.size(); ++i) {
						std::string _var = tokens[i];
						if (_var == " " || _var == "_") { _var = ""; }
						std::vector<std::string> strVec;

						if (_var.size() >= 3 && _var[0] == '[' && _var[_var.size() - 1] == ']')
						{
							auto tokens2 = tokenize(_var, '~');

							strVec.push_back(tokens2[0]);
							strVec.push_back(tokens2[1]);
						}

						long long int a = 0, b = 0, Min = 0, Max = 0;
						bool chkInt = false;

						if (strVec.size() == 2 && Utility::IsInteger(strVec[0]) && Utility::IsInteger(strVec[1])) {
							chkInt = true;
							a = atoll(strVec[0].c_str());
							b = atoll(strVec[1].c_str());
							Min = std::min(a, b);
							Max = std::max(a, b);
						}
						for (long long x = Min; x <= Max; ++x) {
							if (strVec.size() == 2 && chkInt)
							{
								_var = std::to_string(x);
							}
							else {}

							for (int i = 0; i < finded.second.size(); ++i) {
								UserType* temp = finded.second[i];


								temp->RemoveItemList(_var);
								temp->RemoveUserTypeList(_var);
								isTrue = true;
							}

							// prevent from infinity loop.
							if (x == Max) { break; }
						}
					}
					return isTrue;
				}
				else {
					return false;
				}
			}
			static bool Remove(UserType& global, const std::string& position,  const ExecuteData& excuteData) {
				auto finded = UserType::Find(&global, position);
				bool isTrue = false;

				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						UserType* temp = finded.second[i];

					
						temp->Remove();
						isTrue = true;
					}
					return isTrue;
				}
				else {
					return false;
				}
			}
			static bool RemoveUserType(UserType& global, const std::string& position, const std::string& name,  const ExecuteData& excuteData) {
				auto finded = UserType::Find(&global, position);
				bool isTrue = false;

				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						UserType* temp = finded.second[i];

						

						temp->RemoveUserTypeList(name);
						isTrue = true;
					}
					return isTrue;
				}
				else {
					return false;
				}
			}


			static bool RemoveItemType(UserType& global, const std::string& position, const std::string& name,  const ExecuteData& excuteData) {
				auto finded = UserType::Find(&global, position);
				bool isTrue = false;

				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						UserType* temp = finded.second[i];


						temp->RemoveItemList(name);
						isTrue = true;
					}
					return isTrue;
				}
				else {
					return false;
				}
			}

			// todo - static bool Remove(UserType& global, const std::string& positiion, oonst int idx,  const ExecuteData& excuteData)
			static bool Remove(UserType& global, const std::string& position, const int idx,  const ExecuteData& excuteData) {
				auto finded = UserType::Find(&global, position);
				bool isTrue = false;

				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						UserType* temp = finded.second[i];

						temp->RemoveList(idx);
						isTrue = true;
					}
					return isTrue;
				}
				else {
					return false;
				}
			}

			static bool LoadWizDB(UserType& global, const std::string& fileName) {
				UserType globalTemp = UserType("global");

				// Scan + Parse 
				if (false == LoadDataFromFile(fileName, globalTemp)) { return false; }
				wiz::Out << "LoadData End" << ENTER;

				global = std::move(globalTemp);
				return true;
			}
			// SaveQuery
			static bool SaveWizDB(const UserType& global, const std::string& fileName, const std::string& option = "0", const std::string& option2 = "") { /// , int option
				std::ofstream outFile;
				if (fileName.empty()) { return false; }
				if (option2 == "") {
					outFile.open(fileName);
					if (outFile.fail()) { return false; }
#if _WIN32
					if (65001 == GetConsoleOutputCP()) {
						outFile << (char)0xEF << (char)0xBB << (char)0xBF;
					}
#endif
				}
				else {
					outFile.open(fileName, std::ios::app);
					if (outFile.fail()) { return false; }

					outFile << "\n";
				}


				/// saveFile
				if (option == "1") { // for eu4.
					global.Save1(outFile); // cf) friend
				}
				else if (option == "2") {
					global.Save2(outFile);
				}
				else if (option == "3") {
					global.SaveWithJson(outFile);
				}
				else if (option == "3_2") {
					global.SaveWithJson2(outFile);
				}
				

				outFile.close();

				return true;
			}

			/// To Do - ExistItem, ExistUserType, SetUserType GetUserType
			static bool ExistData(UserType& global, const std::string& position, const std::string& varName,  const ExecuteData& excuteData) // 
			{
				int count = 0;
				std::string _var = varName;
				if (_var == " " || _var == "_") { _var = ""; }

				auto finded = UserType::Find(&global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
					
						if (wiz::String::startsWith(_var, "$it")) {
							int itemIdx = stoi(wiz::String::substring(_var, 3));
							count = count + (finded.second[i]->GetItemList(itemIdx).size());
						}
						else {
							count = count + (finded.second[i]->GetItem(_var).size());
						}
					}
				}
				return 0 != count;
			}
			static bool ExistUserType(UserType& global, const std::string& position, 
				const ExecuteData& excuteData) // 
			{
				int count = 0;

				auto finded = UserType::Find(&global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						
						count = count + (finded.second[i]->GetUserTypeListSize());
					}
				}
				return 0 != count;
			}
			static bool ExistOneUserType(UserType& global, const std::string& position, 
				const ExecuteData& excuteData) // 
			{
				int count = 0;

				auto finded = UserType::Find(&global, position);
				if (finded.second[0] == &global) {
					return true;
				}
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						
						count = count + (finded.second[i]->GetUserTypeListSize());
					}
				}
				return 1 == count;
			}
			static bool ExistItem(UserType& global, const std::string& position, const std::string& varName, 
				const ExecuteData& excuteData) // 
			{
				int count = 0;
				std::string _var = varName;
				if (_var == " " || _var == "_") { _var = ""; }

				auto finded = UserType::Find(&global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						
						if (wiz::String::startsWith(_var, "$it")) {
							int itemIdx = stoi(wiz::String::substring(_var, 3));
							count = count + (finded.second[i]->GetItemList(itemIdx).size());
						}
						else {
							count = count + (finded.second[i]->GetItem(_var).size());
						}
					}
				}
				return 0 != count;
			}

			/// ToDo - global, position, var, condition + var is " "!
			// "root" -> position.
			static std::string SearchItem(UserType& global, const std::string& var, 
				const ExecuteData& excuteData,
				const std::string& start_dir = "root",
				StringBuilder* builder = nullptr)
			{
				std::vector<std::string> positionVec;
				std::string temp;

				SearchItem(global, positionVec, var, start_dir, &global,  excuteData);

				for (int i = 0; i < positionVec.size(); ++i)
				{
					temp = temp + positionVec[i] + "\n";
				}

				return temp;
			}
			static std::string SearchUserType(UserType& global, const std::string& var, 
				const ExecuteData& excuteData)
			{
				std::vector<std::string> positionVec;
				std::string temp;

				SearchUserType(global, positionVec, var, "root", &global,  excuteData);

				for (int i = 0; i < positionVec.size(); ++i)
				{
					temp = temp + positionVec[i] + "\n";
				}

				return temp;
			}
		
			static void RemoveUserTypeTotal(UserType& global, const std::string& ut_name, const std::string& nowPosition,
				UserType* ut,
				const ExecuteData& excuteData, bool recursive)
			{
				std::string _var = ut_name;

				if (_var == " " || _var == "_") { _var = ""; }

				for (int i = 0; i < ut->GetUserTypeListSize(); ++i) {
					if (ut->GetUserTypeList(i)->GetName().ToString() == _var) {
						{
							ut->RemoveUserTypeList(i);
							--i;
						}
					}
				}
				/*
				if (recursive) {
					for (int i = 0; i < ut->GetUserTypeListSize(); ++i) {
						std::string temp = ut->GetUserTypeList(i)->GetName();
						if (temp == "") { temp = " "; }
						RemoveUserTypeTotal(
							global,
							_var,
							nowPosition + "/" + temp,
							ut->GetUserTypeList(i),
							
							excuteData,
							builder,
							recursive
						);
					}
				}
				*/
			}
	public:
		static void RemoveUserTypeTotal(UserType& global, const std::string& ut_name, const std::string& start_dir,
			const ExecuteData& excuteData, bool recursive)
		{
			auto temp = wiz::load_data::UserType::Find(&global, start_dir);
			if (temp.first) {
				for (int i = 0; i < temp.second.size(); ++i) // chk!!
				{
					UserType* ut = temp.second[i];
					RemoveUserTypeTotal(global, ut_name, start_dir, ut, excuteData, recursive);

				}
			}
		}

		public:
			// only one exist or do not exist
			static inline wiz::load_data::ItemType<WIZ_STRING_TYPE> GetItem(const wiz::load_data::UserType* ut, const std::string& name) {
				return wiz::load_data::ItemType<WIZ_STRING_TYPE>(ut->GetItem(name)[0].GetName(), ToString(ut->GetItem(name)[0].Get(0)));
			}

			static inline std::vector<wiz::load_data::UserType*> GetUserType(const wiz::load_data::UserType* ut, const std::string& name)
			{
				return ut->GetUserTypeItem(name);
			}
			static std::string Find(wiz::load_data::UserType* ut, const std::string& str)
			{ // std::string 대신 vector<std::string> ??
				int count = 0;
				int idx = -1;
				for (int i = str.size() - 1; i >= 0; --i) {
					if ('/' == str[i]) {
						if (count == 0) {
							idx = i;
						}
						count++;
					}
				}

				std::string result;

				if (count == 1)
				{
					return "";
				}
				else {
					auto x = wiz::load_data::UserType::Find(ut,
						wiz::String::substring(str, 0, idx));
					if (x.first == false) { return ""; }
					for (int i = 0; i < x.second.size(); ++i) {
						std::string itemName = wiz::String::substring(str, idx + 1);
						if (wiz::String::startsWith(itemName, "$it") && itemName.size() >= 4) {
							int itemIdx = stoi(wiz::String::substring(itemName, 3));

							result += ToString(x.second[i]->GetItemList(itemIdx).Get(0));
							if (i < x.second.size() - 1) {
								result += "\n";
							}
						}
						else {
							if (itemName == "_") {
								itemName = "";
							}
							auto temp = x.second[i]->GetItem(itemName);
							if (!temp.empty()) {
								for (int j = 0; j < temp.size(); ++j) {
									result += ToString(temp[j].Get(0));
									if (j < temp.size() - 1) {
										result += " ";
									}
								}
								if (i < x.second.size() - 1) {
									result += "\n";
								}
							}
						}
					}
				}
				return result;
			}

			static inline bool Exist(wiz::load_data::UserType* ut, const std::string& dir)
			{
				auto x = wiz::load_data::UserType::Find(ut, dir);
				return x.first;
			}
			// to do - rename!
			static std::pair<std::string, std::string> Find2(wiz::load_data::UserType* ut, const std::string& str)
			{
				int idx = -1;
				for (int i = str.size() - 1; i >= 0; --i) {
					if ('/' == str[i]) {
						idx = i;
						break;
					}
				}
				return{ wiz::String::substring(str, 0, idx), wiz::String::substring(str,idx + 1) };
			}

			static inline std::string FindParameters(const wiz::ArrayMap<std::string, std::string>& parameters, const std::string& operand)
			{
				wiz::ArrayMap<std::string, std::string>::const_iterator x;
				for (int i = 0; i < parameters.size(); ++i) {
					if (wiz::String::startsWith(operand, "$parameter.")
						&& (x = parameters.find(wiz::String::substring(operand, 11))) != parameters.end()) {
						return x->second;
					}
				}
				return "";
			}
			static inline std::string FindLocals(const wiz::ArrayMap<std::string, std::string>& locals, const std::string& operand)
			{
				if (wiz::String::startsWith(operand, "$local.") && locals.end() != locals.find(wiz::String::substring(operand, 7)))
				{
					return locals.at(wiz::String::substring(operand, 7));
				}
				return "";
			}
			//need to renewal. add $AND $OR $NOT

			/// remove /, parameter chk!!
			static bool operation(wiz::load_data::UserType* now, wiz::load_data::UserType& global, const WIZ_STRING_TYPE& str,
				wiz::ArrayStack<WIZ_STRING_TYPE>& operandStack, const ExecuteData& excuteData);

			static WIZ_STRING_TYPE ToBool4(wiz::load_data::UserType* now, wiz::load_data::UserType& global, const wiz::load_data::UserType& temp, const ExecuteData& excuteData);
			static WIZ_STRING_TYPE ToBool4(wiz::load_data::UserType* now, wiz::load_data::UserType& global, const wiz::load_data::ItemType<WIZ_STRING_TYPE>& temp, const ExecuteData& excuteData);
		//private:
			static WIZ_STRING_TYPE _ToBool4(wiz::load_data::UserType* now, wiz::load_data::UserType& global, const wiz::load_data::UserType& temp, const ExecuteData& excuteData);
			static WIZ_STRING_TYPE ToBool4(wiz::load_data::UserType* now, wiz::load_data::UserType& global, const std::string& temp, const ExecuteData& excuteData);
		};


	
	}
}

#endif // LOAD_DATA_H_INCLUDED
