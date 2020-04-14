#define _CRT_SECURE_NO_WARNINGS
#define ARRAY_QUEUE std::deque // chk?
#define VECTOR std::vector

#include <iostream>
#include <cmath>

#ifdef _MSC_VER 
#include <conio.h>
#endif

#include "wiz/STRINGBUILDER.H"
#include "wiz/load_data.h"

using namespace std::literals;

namespace wiz {
	namespace load_data {
		bool LoadData::operation(wiz::load_data::UserType* now, wiz::load_data::UserType& global, const WIZ_STRING_TYPE& str,
			wiz::ArrayStack<WIZ_STRING_TYPE>& operandStack, const ExecuteData& excuteData)
		{
			if (!operandStack.empty() && operandStack.top() == "ERROR") {
				return false;
			}

			int operandNum = 0;
			int count = 0; // for brace!
			{
				for (int i = operandStack.size() - 1; i >= 0; --i) {
					if ("{"sv == operandStack[i]) {
						count++;
					}
					if ("}"sv == operandStack[i]) {
						count--;
						if (count < 0) {
							break;
						}
						else {
							operandNum++;
						}
					}
					else {
						operandNum++;
					}
				}
			}

			for (int i = 0; i < operandNum; ++i) {
				if ("_" == operandStack[operandStack.size() - 1 - i]) {
					operandStack[operandStack.size() - 1 - i] = "";
				}
			}

			if ("$EQ"sv == str) {
				WIZ_STRING_TYPE x, y;
				int idx = -1;
				x = operandStack.pop();
				y = operandStack.pop();

				if (wiz::load_data::Utility::Compare(x.ToString(), y.ToString()) == "== 0") {
					operandStack.push("TRUE");
				}
				else {
					operandStack.push("FALSE");
				}
			}
			else if ("$NOTEQ"sv == str)
			{
				WIZ_STRING_TYPE x, y;
				x = operandStack.pop();
				y = operandStack.pop();

				if (wiz::load_data::Utility::Compare(x.ToString(), y.ToString()) != "== 0") {
					operandStack.push("TRUE");
				}
				else {
					operandStack.push("FALSE");
				}
			}
			else if ("$AND"sv == str)
			{
				WIZ_STRING_TYPE x, y;
				x = operandStack.pop();
				y = operandStack.pop();

				if (x == "TRUE" && y == "TRUE") {
					operandStack.push("TRUE");
				}
				else {
					operandStack.push("FALSE");
				}
			}
			else if ("$AND_ALL"sv == str) {
				std::vector<WIZ_STRING_TYPE> store(operandNum);
				for (int i = 0; i < operandNum; ++i) {
					store[i] = operandStack.pop();
				}
				for (int i = 0; i < store.size(); ++i) {
					if ("TRUE" != store[i]) {
						operandStack.push("FALSE");
						return true;
					}
				}
				operandStack.push("TRUE");
			}
			else if ("$OR"sv == str)
			{
				WIZ_STRING_TYPE x, y;
				x = operandStack.pop();
				y = operandStack.pop();

				if (x == "TRUE" || y == "TRUE") {
					operandStack.push("TRUE");
				}
				else {
					operandStack.push("FALSE");
				}
			}
			else if ("$OR_ALL"sv == str)
			{
				std::vector<WIZ_STRING_TYPE> store;

				for (int i = 0; i < operandNum; ++i) {
					store.push_back(operandStack.pop());
				}
				for (int i = 0; i < store.size(); ++i) {
					if ("TRUE" == store[i]) {
						operandStack.push("TRUE");
						return true;
					}
				}
				operandStack.push("FALSE");
			}
			else if ("$NOT"sv == str)
			{
				WIZ_STRING_TYPE x;
				x = operandStack.pop();

				if (x == "TRUE") {
					operandStack.push("FALSE");
				}
				else {
					operandStack.push("TRUE");
				}
			}
			else if ("$COMP<"sv == str)
			{
				WIZ_STRING_TYPE x, y;
				x = operandStack.pop();
				y = operandStack.pop();

				if (wiz::load_data::Utility::Compare(x.ToString(), y.ToString()) == "< 0") {
					operandStack.push("TRUE");
				}
				else
				{
					operandStack.push("FALSE");
				}
			}
			else if ("$COMP>"sv == str)
			{
				WIZ_STRING_TYPE x, y;
				x = operandStack.pop();
				y = operandStack.pop();

				if (wiz::load_data::Utility::Compare(x.ToString(), y.ToString()) == "> 0") {
					operandStack.push("TRUE");
				}
				else
				{
					operandStack.push("FALSE");
				}
			}
			else if ("$COMP<EQ"sv == str)
			{
				std::string x, y;
				x = operandStack.pop().ToString();
				y = operandStack.pop().ToString();

				if (wiz::load_data::Utility::Compare(x, y) == "< 0" || wiz::load_data::Utility::Compare(x, y) == "== 0") {
					operandStack.push("TRUE");
				}
				else
				{
					operandStack.push("FALSE");
				}
			}
			else if ("$COMP>EQ"sv == str)
			{
				std::string x, y;
				x = operandStack.pop().ToString();
				y = operandStack.pop().ToString();

				if (wiz::load_data::Utility::Compare(x, y) == "> 0" || wiz::load_data::Utility::Compare(x, y) == "== 0") {
					operandStack.push("TRUE");
				}
				else
				{
					operandStack.push("FALSE");
				}
			}
			else if ("$add"sv == str) // todo! = int operator double => double operator double!
			{
				auto x = operandStack.pop();
				auto y = operandStack.pop();

				if (x.GetType() == 5 && y.GetType() == 5) {
					DataType temp;
					temp.SetFloat(x.ToFloat() + y.ToFloat());
					operandStack.push(temp);
				}
				else if (x.GetType() == 3 && y.GetType() == 3) {
					DataType temp;
					temp.SetInt(x.ToInt() + y.ToInt());
					operandStack.push(temp);
				}
				else {

					operandStack.push("ERROR");
				}
			}
			else if ("$multiple"sv == str) // todo! = int operator double => double operator double!
			{
				std::string _x, _y;
				auto x = operandStack.pop();
				auto y = operandStack.pop();

				_x = x.ToString();
				_y = y.ToString();

				if (x.GetType() == 5 && y.GetType() == 5) {
					DataType temp;
					temp.SetFloat(x.ToFloat() * y.ToFloat());
					operandStack.push(temp);
				}
				else if (x.GetType() == 3 && y.GetType() == 3) {
					DataType temp;
					temp.SetInt(x.ToInt() * y.ToInt());
					operandStack.push(temp);
				}
				else
				{

					operandStack.push("ERROR");
				}
			}
			else if ("$divide"sv == str) // todo! = int operator double => double operator double!
			{
				std::string _x, _y;
				auto x = operandStack.pop();
				auto y = operandStack.pop();

				_x = x.ToString();
				_y = y.ToString();

				if (x.GetType() == 5 && y.GetType() == 5) {
					DataType temp;
					temp.SetFloat(x.ToFloat() / y.ToFloat());
					operandStack.push(temp);
				}
				else if (x.GetType() == 3 && y.GetType() == 3) {
					DataType temp;
					temp.SetInt(x.ToInt() / y.ToInt());
					operandStack.push(temp);
				}
				else
				{

					operandStack.push("ERROR");
				}

			}
			else if ("$modular"sv == str)
			{
				WIZ_STRING_TYPE x, y;
				x = operandStack.pop();
				y = operandStack.pop();
				std::string _x = x.ToString();
				std::string _y = y.ToString();

				if (x.GetType() == 3 && y.GetType() == 3) {
					DataType temp;
					temp.SetInt(x.ToInt() % y.ToInt());
					operandStack.push(temp);
				}
				else
				{
					operandStack.push("ERROR");
				}
			}
			else if ("$rand"sv == str)
			{
				std::string x, y;
				x = operandStack.pop().ToString();
				y = operandStack.pop().ToString();

				if (wiz::load_data::Utility::GetType(x) == wiz::load_data::Utility::GetType(y) && (wiz::load_data::Utility::GetType(y) == "INTEGER")) { /// only integer -> BigInteger
					int _x = atoi(x.c_str());
					int _y = atoi(y.c_str());
					if (_y - _x + 1 <= 0) {
						operandStack.push("ERROR x / 0");

					}
					else {
						int _z = wiz::Rand2<int>() % (_y - _x + 1) + _x; // _x <= _z <= _y
						operandStack.push(wiz::toStr(_z));
					}
				}
				else
				{
					operandStack.push("ERROR");
				}
			}
			else if ("$concat"sv == str)
			{
				{
					std::string x, y;
					x = operandStack.pop().ToString();
					y = operandStack.pop().ToString();

					operandStack.push(x + y);
				}
			}
			else if ("$concat2"sv == str) /// with space
			{
				std::string x, y;
				x = operandStack.pop().ToString();
				y = operandStack.pop().ToString();

				operandStack.push(x + " " + y);
			}
			else if ("$concat_all"sv == str)
			{
				{
					std::string result;

					for (int i = 0; i < operandNum; ++i) {
						result = result + operandStack.pop().ToString();
						//	cout << "test" <<  result << endl;
					}
					//cout << "----" << endl;
					operandStack.push(result);
				}
			}
			else if ("$concat_all2"sv == str)
			{
				WIZ_STRING_TYPE result;

				for (int i = 0; i < operandNum; ++i) {
					result += operandStack.pop();
					if (i < operandNum - 1) {
						result += " ";
					}
				}
				operandStack.push(result);
			}
			else if ("$concat3"sv == str) { // for special case? "abc" "def" "ghi" -> "abc def ghi"
				WIZ_STRING_TYPE result;

				for (int i = 0; i < operandNum; ++i) {
					std::string temp = operandStack.pop().ToString();
					if (temp.size() >= 3 && temp.back() == temp.front() && temp.back() == '\"') {
						// pass
					}
					else {
						operandStack.push("ERROR in $concat3, 1. must be \" \" ");
						return false;
					}
					if (i < operandNum - 1) {
						temp.erase(temp.begin() + temp.size() - 1);

						if (0 != i) {
							temp.erase(temp.begin());
						}

						result += temp;
						result += " ";
					}
					else {
						temp.erase(temp.begin());

						result += temp;
					}
				}

				operandStack.push(result);
			}
			else if ("$return_value"sv == str)
			{
				operandStack.push(excuteData.info.return_value);
			}
			// cf) empty test!!
			///ToDo - GetList -> // GetItemListIdxByIListIdx, GetUserTypeLisIdxtByIListIdx ?
			else if ("$back"sv == str) // ex) for x  = { 0 1 2 3 .. }, for usertaypelist? and mixed? and need more test!
			{
				WIZ_STRING_TYPE x = operandStack.pop();

				std::string value = wiz::load_data::LoadData::GetItemListData(global, x.ToString(), excuteData);
				wiz::load_data::UserType ut;
				wiz::load_data::LoadData::LoadDataFromString(value, ut);

				if (ut.IsItemList(ut.GetIListSize() - 1))

				{
					auto x = ut.GetItemList(ut.GetItemListSize() - 1);
					operandStack.push(ToString(x.Get(0)));
				}
				else {
					auto x = ut.GetUserTypeList(ut.GetUserTypeListSize() - 1);
					operandStack.push("\"" + x->ToString() + "\"");
				}
			}
			// pop_back or front - remove this function?
			else if ("$pop_back"sv == str) // and for usertypelist? and mixed?, usertype-> "~"
			{
				std::string x = operandStack.pop().ToString();

				WIZ_STRING_TYPE name;
				for (int i = x.size() - 1; i >= 0; --i)
				{
					if (x[i] == '/' && i != 0) {
						name = wiz::String::substring(x, i + 1);
						x = wiz::String::substring(x, 0, i - 1);
						break;
					}
				}

				if (x.empty()) {
					x = ".";
				}

				wiz::load_data::UserType* ut = nullptr;
				auto finded = wiz::load_data::UserType::Find(&global, x);
				ut = finded.second[0];

				if (ut->IsItemList(ut->GetIListSize() - 1))
				{
					auto x = ut->GetItemList(ut->GetItemListSize() - 1);
					operandStack.push(ToString(x.Get(0)));
					ut->RemoveItemList(ut->GetItemListSize() - 1);
				}
				else {
					auto x = ut->GetUserTypeList(ut->GetUserTypeListSize() - 1);
					operandStack.push("\"" + x->ToString() + "\"");
					ut->RemoveUserTypeList(ut->GetUserTypeListSize() - 1);
				}
			}
			// todo - $front, pop-front.
			else if ("$front"sv == str)
			{
				std::string x = operandStack.pop().ToString();

				std::string value = wiz::load_data::LoadData::GetItemListData(global, x, excuteData);
				wiz::load_data::UserType ut;
				wiz::load_data::LoadData::LoadDataFromString(value, ut);

				if (ut.IsItemList(0))
				{
					auto x = ut.GetItemList(0);
					operandStack.push(ToString(x.Get(0)));
				}
				else {
					auto x = ut.GetUserTypeList(0);
					operandStack.push("\"" + x->ToString() + "\"");
				}
			}
			else if ("$pop_front"sv == str)
			{
				std::string x = operandStack.pop().ToString();
				std::string name;
				for (int i = x.size() - 1; i >= 0; --i)
				{
					if (x[i] == '/' && i != 0) {
						name = wiz::String::substring(x, i + 1);
						x = wiz::String::substring(x, 0, i - 1);
						break;
					}
				}

				if (x.empty()) { x = "."; }

				wiz::load_data::UserType* ut = nullptr;
				auto finded = wiz::load_data::UserType::Find(&global, x);
				ut = finded.second[0];

				if (ut->IsItemList(0))
				{
					auto x = ut->GetItemList(0);
					operandStack.push(ToString(x.Get(0)));
					ut->RemoveItemList(0);
				}
				else {
					auto x = ut->GetUserTypeList(0);
					operandStack.push("\"" + x->ToString() + "\"");
					ut->RemoveUserTypeList(0);
				}
			}
			else if ("$get"sv == str)
			{
				std::string x = operandStack.pop().ToString();

				if ('@' == x[0]) { // chk..
					x.erase(x.begin());
				}

				if (wiz::String::startsWith(x, "now.") && nullptr != now)
				{
					x = wiz::String::substring(x, 4);
					x = "/./" + x;
					if ('/' == x[0])
					{
						std::string temp = Find(now, x); if (!temp.empty()) { x = temp; operandStack.push(x); return true; }
					}
				}
				else {
					if ('/' == x[0])
					{
						std::string temp = Find(&global, x); if (!temp.empty()) { x = temp; operandStack.push(x); return true; }
					}
				}

				{
					std::string temp = FindParameters(excuteData.info.parameters, x);
					if (!temp.empty()) { x = temp; operandStack.push(x); return true; }
				}
				{
					std::string temp = FindLocals(excuteData.info.locals, x);
					if (!temp.empty()) { x = temp; operandStack.push(x); return true; }
				}

				operandStack.push(std::string());
				return true;
			}
			else if ("$size"sv == str)
			{
				std::string x = operandStack.pop().ToString();

				if ('/' == x[0])
				{
				//	std::cout << global.ToString();

					wiz::load_data::UserType* ut = wiz::load_data::UserType::Find(&global, x).second[0];
					x = wiz::toStr(ut->GetItemListSize());
				}
				else
				{
					x = "FALSE";
				}

				operandStack.push(x);
			}
			else if ("$size2"sv == str)
			{
				std::string x = operandStack.pop().ToString();

				if ('/' == x[0])
				{
					wiz::load_data::UserType* ut = wiz::load_data::UserType::Find(&global, x).second[0];
					x = wiz::toStr(ut->GetUserTypeListSize());
				}
				else
				{
					x = "FALSE";
				}

				operandStack.push(x);
			}
			else if ("$element"sv == str) // for list
			{
				std::string x = operandStack.pop().ToString(); // list_name
				WIZ_STRING_TYPE y = operandStack.pop(); // idx
				int idx = stoi(y.ToString());

				if ('/' == x[0])
				{
					wiz::load_data::UserType* ut = wiz::load_data::UserType::Find(&global, x).second[0];
					x = ToString(ut->GetItemList(idx).Get(0));
				}
				else
				{
					x = ""; // NONE?
				}

				operandStack.push(x);
			}
			// it_name
			else if ("$name"sv == str) {
				std::string x = operandStack.pop().ToString(); // list_name
				WIZ_STRING_TYPE y = operandStack.pop(); // idx
				int idx = stoi(y.ToString());

				if ('/' == x[0])
				{
					wiz::load_data::UserType* ut = wiz::load_data::UserType::Find(&global, x).second[0];
					x = ToString(ut->GetItemList(idx).GetName());
				}
				else
				{
					x = ""; // NONE?
				}

				operandStack.push(x);
			}
			// ut_name
			else if ("$name2"sv == str) {
				std::string x = operandStack.pop().ToString(); // list_name
				WIZ_STRING_TYPE y = operandStack.pop(); // idx
				int idx = stoi(y.ToString());

				if ('/' == x[0])
				{
					wiz::load_data::UserType* ut = wiz::load_data::UserType::Find(&global, x).second[0];
					x = ToString(ut->GetUserTypeList(idx)->GetName());
				}
				else
				{
					x = ""; // "NONE";
				}

				operandStack.push(x);
			}
			else if ("$regex"sv == str) {
				std::string _str = operandStack.pop().ToString();
				std::string rgx_str = operandStack.pop().ToString();

				// " ~ " , "제거?
				if (rgx_str.size() > 2 && rgx_str[0] == rgx_str.back() && rgx_str[0] == '\"') {
					std::regex rgx(rgx_str.substr(1, rgx_str.size() - 2));

					if (std::regex_match(_str, rgx))
					{
						operandStack.push("TRUE");
					}
					else {
						operandStack.push("FALSE");
					}
				}
				else {
					operandStack.push("ERROR in $regex");
				}
			}
			else if ("$eval"sv == str) { //// now, no need ?
				std::string _str = operandStack.pop().ToString();

				bool chk = _str.size() >=2 && _str[0] == _str.back() && _str.back() == '\"';
				if (chk) {
				}
				else {
					operandStack.push("ERROR in $eval, must be \" \" ");
					return false;
				}
				_str = _str.substr(1, _str.size() - 2);
				{
					UserType ut;

					wiz::load_data::LoadData::LoadDataFromString(_str, ut);
					
					WIZ_STRING_TYPE result = ToBool4(now, global, ut, excuteData);

					operandStack.push(std::move(result));
				}
			}
			// big
			else if ("$is_quoted_str"sv == str)
			{
				std::string _str = operandStack.pop().ToString();
				if (_str.size() >= 2 && _str[0] == _str.back() && '\"' == _str[0])
				{
					operandStack.push("TRUE");
				}
				else {
					operandStack.push("FALSE");
				}
			}
			// small
			else if ("$is_quoted_str2"sv == str)
			{
				std::string _str = operandStack.pop().ToString();
				if (_str.size() >= 2 && _str[0] == _str.back() && '\'' == _str[0])
				{
					operandStack.push("TRUE");
				}
				else {
					operandStack.push("FALSE");
				}
			}
			else if ("$to_quoted_str"sv == str) {
				std::string _str = operandStack.pop().ToString();
				_str.push_back('\"');
				_str.insert(_str.begin(), '\"');
				operandStack.push(_str);
			}
			else if ("$to_quoted_str2"sv == str) {
				std::string _str = operandStack.pop().ToString();
				_str.push_back('\'');
				_str.insert(_str.begin(), '\'');
				operandStack.push(_str);
			}
			else if ("$add_small_quoted"sv == str) {
				std::string _str = operandStack.pop().ToString();
				_str.push_back('\'');
				_str.insert(_str.begin(), '\'');
				operandStack.push(_str);
			}
			else if ("$remove_quoted"sv == str) { // chk "" WIZ_STRING_TYPE problem?
				std::string _str = operandStack.pop().ToString();

				if (_str.size() > 0 && _str.front() == _str.back()
					&& '\"' == _str.back()
					)
				{
					_str = wiz::String::substring(_str, 1, _str.size() - 2);
				}

				operandStack.push(_str);
			}
			else if ("$remove_quoted2"sv == str) { // chk "" WIZ_STRING_TYPE problem?
				std::string _str = operandStack.pop().ToString();

				if (_str.size() > 0 && _str.front() == _str.back()
					&& '\'' == _str.back()
					)
				{
					_str = wiz::String::substring(_str, 1, _str.size() - 2);
				}

				operandStack.push(str);
			}
			else if ("$remove_small_quoted"sv == str) { // chk "" WIZ_STRING_TYPE problem?
				std::string _str = operandStack.pop().ToString();

				if (_str.size() > 0 && _str.front() == _str.back()
					&& '\'' == _str.back()
					)
				{
					_str = wiz::String::substring(_str, 1, _str.size() - 2);
				}

				operandStack.push(str);
			}
			else if ("$get_object_str"sv == str) {
				std::string object_name = operandStack.pop().ToString();
				object_name = wiz::String::substring(object_name, 1, object_name.size() - 2);
				std::string event_id = operandStack.pop().ToString();

				wiz::load_data::UserType ut = (*excuteData.pObjectMap)[object_name];

				bool pass = false;
				for (int i = 0; i < ut.GetUserTypeListSize(); ++i) {
					for (int j = 0; j < ut.GetUserTypeList(i)->GetItemListSize(); ++j) {
						if ("id" == ut.GetUserTypeList(i)->GetItemList(j).GetName()
							&& event_id == ToString(ut.GetUserTypeList(i)->GetItemList(j).Get(0))
							) {
							operandStack.push("Event = { " + ut.GetUserTypeList(i)->ToString() + " } ");
							return true;
						}
					}
				}
				operandStack.push("ERROR in $getOjbectStr");
			}
			else if ("$add_paren"sv == str) { // removal?
				std::string temp = operandStack.pop().ToString();
				operandStack.push(" { " + temp + " } ");
			}
			else if ("$get_item_name"sv == str) {
				wiz::load_data::UserType ut;
				std::string statement;

				for (int i = 0; i < operandNum; ++i) {
					statement = statement + operandStack.pop().ToString();
				}
				wiz::load_data::LoadData::LoadDataFromString(statement, ut);

				operandStack.push(wiz::ToString(ut.GetItemList(0).GetName()));
			}
			else if ("$get_item_value"sv == str) { // why?
				wiz::load_data::UserType ut;
				std::string statement;
				int idx = 0;

				for (int i = 0; i < operandNum - 1; ++i) {
					statement = statement + operandStack.pop().ToString();
				}
				idx = stoi(operandStack.pop().ToString());

				wiz::load_data::LoadData::LoadDataFromString(statement, ut);

				operandStack.push(ToString(ut.GetItemList(idx).Get(0)));
			}
			else if ("$get_item_size"sv == str) {
				wiz::load_data::UserType ut;
				std::string statement;

				for (int i = 0; i < operandNum; ++i) {
					statement = statement + operandStack.pop().ToString();
				}
				wiz::load_data::LoadData::LoadDataFromString(statement, ut);

				operandStack.push(wiz::_toString(ut.GetItem(wiz::ToString(ut.GetItemList(0).GetName())).size()));
			}
			else if ("$is_empty_string"sv == str) {
				operandStack.push(operandStack.pop().ToString().empty() ? "TRUE" : "FALSE");
			}
			else if ("$event_result"sv == str) {
				std::vector<std::string> eventVec;
				for (int i = 0; i < operandNum; ++i) {
					eventVec.push_back(operandStack.pop().ToString());
				}

				std::string statements2 = "Event = { id = NONE" + wiz::toStr(excuteData.depth + 1) + " $call = { ";
				for (int i = 0; i < eventVec.size(); ++i) {
					statements2 = statements2 + eventVec[i] + " ";
				}
				statements2 = statements2 + " } }";
				wiz::load_data::UserType* eventsTemp = excuteData.pEvents;
				wiz::load_data::LoadData::AddData(*eventsTemp, "/root", statements2, ExecuteData());
				//cout << " chk " << statements2 << endl;
				ExecuteData _excuteData;
				_excuteData.pModule = excuteData.pModule;
				_excuteData.pObjectMap = excuteData.pObjectMap;
				_excuteData.pEvents = eventsTemp;
				_excuteData.depth = excuteData.depth + 1;
				_excuteData.noUseInput = excuteData.noUseInput;
				_excuteData.noUseOutput = excuteData.noUseOutput;


				Option opt;
				operandStack.push(pExcuteModule->excute_module("Main = { $call = { id = NONE" + wiz::toStr(_excuteData.depth) + " } }", &global, _excuteData, opt, 0));

				{
					for (int idx = 0; idx < eventsTemp->GetUserTypeListSize(); ++idx) {
						if (ToString(eventsTemp->GetUserTypeList(idx)->GetItem("id")[0].Get(0)) == "NONE" + wiz::toStr(_excuteData.depth)) {
							eventsTemp->RemoveUserTypeList(idx);
							break;
						}
					}
				}
			}
			else if ("$get_item_value2"sv == str) {
				const int i = stoi(operandStack.pop().ToString());

				if (now) {
					operandStack.push(ToString(now->GetItemList(i).Get(0))); //chk
					return true;
				}
				else {
					operandStack.push("ERROR");
					return false;
				}
			}
			else if ("$space"sv == str) {
				operandStack.push(" ");
				return true;
			}
			else if ("$empty"sv == str) {
				operandStack.push("");
				return true;
			}
			else if ("$move_up"sv == str) {
			std::string dir;

				for (int i = 0; i < operandNum; ++i) {
					std::string temp = operandStack.pop().ToString();
					dir = dir + temp;
					//	cout << "temp is " << temp << endl;
				}

				//cout << " dir is  " << dir << endl;
				if (String::startsWith(dir, "/."))
				{
					dir = String::substring(dir, 3);
				}

				std::vector<std::string> tokenVec = tokenize(dir, '/');

				dir = "/./";
				if (tokenVec.empty()) { operandStack.push(dir); return true; }
				for (int i = 0; i < tokenVec.size() - 1; ++i) {
					dir = dir + tokenVec[i] + "/";
				}
				operandStack.push(dir);
				return true;
			}
			
			else if ("$lambda"sv == str) {
				std::vector<std::string> store;
				for (int i = 0; i < operandNum; ++i) {
					store.push_back(operandStack.pop().ToString());
					if (store[i][0] != store[i][store[i].size() - 1] || '\'' != store[i][0])
					{
						store[i] = "\'" + store[i] + "\'";
					}
				}
				wiz::load_data::UserType ut;
				wiz::load_data::LoadData::LoadDataFromString(store[0].substr(1, store[0].size() - 2), ut);


				if (operandNum >= 1) {
					std::vector<wiz::load_data::UserType*> param = ut.GetUserTypeItem("Event")[0]->GetUserTypeItem("$parameter");
					// in order.
					std::string mainStr = "Main = { $call = { id = NONE } } ";
					std::string eventStr = "Event = { id = NONE $call = { id = " +
						ut.GetUserTypeItem("Event")[0]->GetItem("id")[0].Get(0).ToString() + " ";

					// if opeandNum != ut[0].GetUserType("$parameter").size() then push error?
					for (int i = 0; i < param[0]->GetItemListSize(); ++i) {
						eventStr += param[0]->GetItemList(i).Get(0).ToString() + " = \'" + store[i + 1].substr(1, store[i + 1].size() - 2) + "\' ";
					}

					eventStr += " }  $return = { $return_value = { } } } ";

					eventStr += store[0].substr(1, store[0].size() - 2);

					ut.Remove();
					wiz::load_data::LoadData::LoadDataFromString(eventStr, ut);

					std::string result;
					{
						ExecuteData _excuteData;
						_excuteData.noUseInput = excuteData.noUseInput;
						_excuteData.noUseOutput = excuteData.noUseOutput;

						Option opt;
						result = pExcuteModule->excute_module(mainStr, &ut, _excuteData, opt, 0);
					}
					{
						wiz::load_data::UserType ut;
						wiz::load_data::LoadData::LoadDataFromString(result.substr(1, result.size() - 2), ut);

						if (0 == ut.GetUserTypeListSize()) {
							std::string param = ut.ToString();
							if (wiz::String::startsWith(param, "$parameter.")) { //
								param = param.substr(11);
								result = std::string("\'Event = { id = identity ") + "$parameter = { " + param + " } $return = { $parameter." + param + " } }\'";
							}
						}
					}

					operandStack.push(result);
				}

				return true;
			}
			
			else if ("$to_float_from_integer"sv == str) { // integer, floating point number -> floating point number(long double)
				std::string value = operandStack.pop().ToString();
				if (wiz::load_data::Utility::IsInteger(value)) {
					long double x = stoll(value);
					operandStack.push(wiz::_toString(x));
				}
				else {
					operandStack.push("it is not integer");
				}
			}
			else if ("$to_integer_from_float"sv == str) { // integer, floating point number -> floating point number(long double)
				std::string value = operandStack.pop().ToString();
				if (wiz::load_data::Utility::IsDouble(value)) {
					long long x = std::stold(value);
					operandStack.push(wiz::_toString(x));
				}
				else {
					operandStack.push("it is not floating number");
				}
			}
			//floor, ceiling, round,
			else if ("$floor"sv == str) {
				std::string value = operandStack.pop().ToString();
				if (wiz::load_data::Utility::IsDouble(value) ){
					long double x = std::floor(std::stold(value));
					operandStack.push(wiz::_toString(x));
				}
				else {
					operandStack.push("only double can");
				}
			}
			else if ("$ceiling"sv == str) {
				std::string value = operandStack.pop().ToString();
				if (wiz::load_data::Utility::IsDouble(value)) {
					long double x = std::ceil(std::stold(value));
					operandStack.push(wiz::_toString(x));
				}
				else {
					operandStack.push("only double can");
				}
			}
			else if ("$round"sv == str) {
				std::string value = operandStack.pop().ToString();
				if (wiz::load_data::Utility::IsDouble(value)) {
					long double x = std::round(std::stold(value));
					operandStack.push(wiz::_toString(x));
				}
				else {
					operandStack.push("only double can");
				}
			}
			//contains,
			else if ("$contains"sv == str) {
				std::string _str = operandStack.pop().ToString();
				std::string chk_str = operandStack.pop().ToString();

				operandStack.push(std::string::npos != _str.find(chk_str) ? "TRUE" : "FALSE");
			}
			//starts_with, ends_with,
			else if ("$starts_with"sv == str) {
				std::string _str = operandStack.pop().ToString();
				std::string chk_str = operandStack.pop().ToString();

				operandStack.push(wiz::String::startsWith(_str, chk_str) ? "TRUE" : "FALSE");
			}
			else if ("$ends_with"sv == str) {
				std::string _str = operandStack.pop().ToString();
				std::string chk_str = operandStack.pop().ToString();

				operandStack.push(wiz::String::endsWith(_str, chk_str) ? "TRUE" : "FALSE");
			}
			//WIZ_STRING_TYPE - length,
			else if ("$string_length"sv == str) {
				std::string _str = operandStack.pop().ToString();
	
				operandStack.push(wiz::_toString(_str.size()));
			}
			//substring ?
			else if ("$substring"sv == str) {
				std::string _str = operandStack.pop().ToString();
				long long begin = stoll(operandStack.pop().ToString());
				long long end = stoll(operandStack.pop().ToString());

				operandStack.push(wiz::String::substring(_str, begin, end - 1));
			}

			// todo - Is~ ?? others ??
			else if ("$is_integer_type"sv == str) {
				operandStack.push(wiz::load_data::Utility::IsInteger(operandStack.pop().ToString()) ? "TRUE" : "FALSE");
			}
			else if ("$is_float_type"sv == str) {
				operandStack.push(wiz::load_data::Utility::IsDouble(operandStack.pop().ToString()) ? "TRUE" : "FALSE");
			}
			else if ("$is_pure_string_type"sv == str) {
				operandStack.push("STRING" == wiz::load_data::Utility::GetType(operandStack.pop().ToString()) ? "TRUE" : "FALSE");
			}
			else if ("$get_type"sv == str) {
				operandStack.push(wiz::load_data::Utility::GetType(operandStack.pop().ToString()));
			}
			else if ("$is_itemtype_exist"sv == str) { // 2? - using ToBool4?
				operandStack.push(wiz::load_data::LoadData::Find(&global, operandStack.pop().ToString()).empty() ? "FALSE" : "TRUE");
			}
			else if ("$is_usertype_exist"sv == str) { // 2? - using ToBool4?
				operandStack.push(wiz::load_data::UserType::Find(&global, operandStack.pop().ToString()).first ? "TRUE" : "FALSE");
			}

			else {
				if (wiz::String::startsWith(str.ToString(), "$") && str.ToString().size() >= 2) {
					wiz::Out << "no exist in load-data " << str.ToString() << ENTER;
					return false;
				}
				return true;
			}
			return true;
			// cf)
			// remove "
		}

		WIZ_STRING_TYPE LoadData::ToBool4(wiz::load_data::UserType* now, wiz::load_data::UserType& global, const wiz::load_data::UserType& temp, const ExecuteData& excuteData)
		{
			WIZ_STRING_TYPE result, _temp;
			size_t user_count = 0;
			size_t item_count = 0;

			for (size_t i = 0; i < temp.GetIListSize(); ++i) {
				if (i > 0) {
					result += " ";
				}
				if (temp.IsItemList(i)) {
					_temp = ToBool4(now, global, temp.GetItemList(item_count), excuteData);
					result += _temp;
					item_count++;
				}
				else {
					_temp = _ToBool4(now, global, *temp.GetUserTypeList(user_count), excuteData);
					result += _temp;
					user_count++;
				}
			}

			return result;
		}
		WIZ_STRING_TYPE LoadData::_ToBool4(wiz::load_data::UserType* now, wiz::load_data::UserType& global, const wiz::load_data::UserType& temp, const ExecuteData& excuteData)
		{
			WIZ_STRING_TYPE result, _temp;
			size_t user_count = 0;
			size_t item_count = 0;
			ArrayStack<WIZ_STRING_TYPE> operandStack;

			for (size_t i = 0; i < temp.GetIListSize(); ++i) {
				if (i > 0) {
					result += " ";
				}
				if (temp.IsItemList(i)) {
					_temp = ToBool4(now, global, temp.GetItemList(item_count), excuteData);
					result += _temp;
					operandStack.push(_temp);
					item_count++;
				}
				else {
					_temp = _ToBool4(now, global, *temp.GetUserTypeList(user_count), excuteData);
					result += _temp;
					operandStack.push(_temp);
					user_count++;
				}
			}

			//std::cout << temp.GetName() << " ";
			//for (int i = 0; i < operandStack.size(); ++i) {
				//std::cout << operandStack[i] << " ";
			//}
			//std::cout << "\n";
			if (String::startsWith(temp.GetName().ToString(), "$") && temp.GetName().ToString().size() > 1) {
				wiz::ArrayStack<WIZ_STRING_TYPE> _stack;

				while (operandStack.empty() == false) {
					_stack.push(operandStack.pop());
				}

				if (!operation(now, global, temp.GetName(), _stack, excuteData)) {
					wiz::Out << "operation ERROR";
				}

				result = _stack.top();
			}
			//std::cout << "result is " << result << "\n";
			return result;
		}

		WIZ_STRING_TYPE LoadData::ToBool4(wiz::load_data::UserType* now, wiz::load_data::UserType& global, const wiz::load_data::ItemType<WIZ_STRING_TYPE>& temp, const ExecuteData& excuteData)
		{
			WIZ_STRING_TYPE result;
			if (temp.GetName().empty() == false) {
				result += temp.GetName() + " = ";
			}

			auto tokens = tokenize(temp.Get().ToString(), '/');

			if (tokens.size() <= 1) {
				return result + ToBool4(now, global, temp.Get().ToString(), excuteData);
			}
			
			WIZ_STRING_TYPE _result = "/";

			for (int i = 0; i < tokens.size(); ++i) {
				WIZ_STRING_TYPE _temp = ToBool4(now, global, tokens[i], excuteData);
				//std::cout << "chk " << _temp << " ";
				_result += _temp;

				if (i < tokens.size() - 1) {
					_result += "/";
				}
			}
			//std::cout << "\n";

			return result + ToBool4(now, global, _result.ToString(), excuteData);
		}

		WIZ_STRING_TYPE LoadData::ToBool4(wiz::load_data::UserType* now, wiz::load_data::UserType& global, const std::string& temp, const ExecuteData& excuteData) {
			std::string result;
			if (String::startsWith(temp, "$local.")) {
				if (!(result = FindLocals(excuteData.info.locals, temp)).empty()) {
					return result;
				}
			}
			else if (String::startsWith(temp, "$parameter.")) {
				if (!(result = FindParameters(excuteData.info.parameters, temp)).empty()) {
					return result;
				}
			}
			else if (String::startsWith(temp, "/") && temp.size() > 1) {
				if (!(result = Find(&global, temp)).empty()) {
					return result;
				}
			}
			
			{
				return temp;
			}
		}

	}
}