

// change to char chk. (from addspace, chk #, chk " ") - 2016.02.17 

#ifndef LOAD_DATA_UTILITY_H
#define LOAD_DATA_UTILITY_H

#include <string>
#include <vector>
#include <string>
#include <fstream>
#include <thread> // maybe error with C++/CLI?
#include <algorithm>

#include "load_data_types.h"
#include "cpp_string.h"
#include "STRINGBUILDER.H"

namespace wiz {
	namespace load_data {
		class Utility
		{
		public:
			static bool IsInteger(std::string str) {
				//if (str.size() > 2 && str[0] == str.back() && (str[0] == '\"' || str[0] == '\'')) {
				//	str = str.substr(1, str.size() - 2);
				//}
				int state = 0;
				for (int i = 0; i < str.size(); ++i) {
					switch (state)
					{
					case 0:
						if ('+' == str[i] || '-' == str[i]) {
							state = 0;
						}
						else if (str[i] >= '0' && str[i] <= '9')
						{
							state = 1;
						}
						else return false;
						break;
					case 1:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 1;
						}
						else return false;
					}
				}
				return 1 == state; /// chk..
			}
			static bool IsNumberInJson(std::string str)
			{
				//if (str.size() > 2 && str[0] == str.back() && (str[0] == '\"' || str[0] == '\'')) {
				//	str = str.substr(1, str.size() - 2);
				//}
				int state = 0;
				for (int i = 0; i < str.size(); ++i) {
					switch (state)
					{
					case 0:
						if ( // '+' == str[i] || // why can`t +
							'-' == str[i]
							) {
							state = 0;
						}
						else if (str[i] >= '0' && str[i] <= '9')
						{
							state = 1;
						}
						else { return false; }
						break;
					case 1:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 1;
						}
						else if (str[i] == '.') {
							state = 2;
						}
						else { return false; }
						break;
					case 2:
						if (str[i] >= '0' && str[i] <= '9') { state = 3; }
						else { return false; }
						break;
					case 3:
						if (str[i] >= '0' && str[i] <= '9') { state = 3; }
						else if ('e' == str[i] || 'E' == str[i]) {
							state = 4;
						}
						else { return false; }
						break;
					case 4:
						if (str[i] == '+' || str[i] == '-') {
							state = 5;
						}
						else {
							state = 5;
						}
						break;
					case 5:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 6;
						}
						else {
							return false;
						}
						break;
					case 6:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 6;
						}
						else {
							return false;
						}
					}
				}
				return 3 == state || 6 == state;
			}
			static bool IsDouble(std::string str) {
				//if (str.size() > 2 && str[0] == str.back() && (str[0] == '\"' || str[0] == '\'')) {
				//	str = str.substr(1, str.size() - 2);
				//}
				int state = 0;
				for (int i = 0; i < str.size(); ++i) {
					switch (state)
					{
					case 0:
						if ('+' == str[i] || '-' == str[i]) {
							state = 0;
						}
						else if (str[i] >= '0' && str[i] <= '9')
						{
							state = 1;
						}
						else { return false; }
						break;
					case 1:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 1;
						}
						else if (str[i] == '.') {
							state = 2;
						}
						else { return false; }
						break;
					case 2:
						if (str[i] >= '0' && str[i] <= '9') { state = 3; }
						else { return false; }
						break;
					case 3:
						if (str[i] >= '0' && str[i] <= '9') { state = 3; }
						else if ('e' == str[i] || 'E' == str[i]) {
							state = 4;
						}
						else { return false; }
						break;
					case 4:
						if (str[i] == '+' || str[i] == '-') {
							state = 5;
						}
						else {
							state = 5;
						}
						break;
					case 5:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 6;
						}
						else {
							return false;
						}
						break;
					case 6:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 6;
						}
						else {
							return false;
						}
					}
				}
				return 3 == state || 6 == state;
			}
			static bool IsDate(std::string str) /// chk!!
			{
				//if (str.size() > 2 && str[0] == str.back() && (str[0] == '\"' || str[0] == '\'')) {
				//	str = str.substr(1, str.size() - 2);
				//}
				int state = 0;
				for (int i = 0; i < str.size(); ++i) {
					switch (state)
					{
					case 0:
						if (str[i] >= '0' && str[i] <= '9')
						{
							state = 1;
						}
						else return false;
						break;
					case 1:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 1;
						}
						else if (str[i] == '.') {
							state = 2;
						}
						else return false;
						break;
					case 2:
						if (str[i] >= '0' && str[i] <= '9') { state = 3; }
						else return false;
						break;
					case 3:
						if (str[i] >= '0' && str[i] <= '9') { state = 3; }
						else if (str[i] == '.') {
							state = 4;
						}
						else return false;
						break;
					case 4:
						if (str[i] >= '0' && str[i] <= '9') { state = 5; }
						else return false;
						break;
					case 5:
						if (str[i] >= '0' && str[i] <= '9') { state = 5; }
						else return false;
						break;
					}
				}
				return 5 == state;
			}
			static bool IsDateTimeA(std::string str) // yyyy.MM.dd.hh
			{
				//if (str.size() > 2 && str[0] == str.back() && (str[0] == '\"' || str[0] == '\'')) {
				//	str = str.substr(1, str.size() - 2);
			//	}
				int state = 0;
				for (int i = 0; i < str.size(); ++i) {
					switch (state)
					{
					case 0:
						if (str[i] >= '0' && str[i] <= '9')
						{
							state = 1;
						}
						else return false;
						break;
					case 1:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 1;
						}
						else if (str[i] == '.') {
							state = 2;
						}
						else return false;
						break;
					case 2:
						if (str[i] >= '0' && str[i] <= '9') { state = 3; }
						else return false;
						break;
					case 3:
						if (str[i] >= '0' && str[i] <= '9') { state = 3; }
						else if (str[i] == '.') {
							state = 4;
						}
						else return false;
						break;
					case 4:
						if (str[i] >= '0' && str[i] <= '9') { state = 5; }
						else return false;
						break;
					case 5:
						if (str[i] >= '0' && str[i] <= '9') { state = 5; }
						else if (str[i] == '.') { state = 6; }
						else return false;
						break;
					case 6:
						if (str[i] >= '0' && str[i] <= '9') { state = 7; }
						else return false;
						break;
					case 7:
						if (str[i] >= '0' && str[i] <= '9') { state = 7; }
						else return false;
						break;
					}
				}
				return 7 == state;
			}
			static bool IsDateTimeB(std::string str) // yyyy.MM.dd.hh.mm
			{
			//	if (str.size() > 2 && str[0] == str.back() && (str[0] == '\"' || str[0] == '\'')) {
			//		str = str.substr(1, str.size() - 2);
			//	}
				int state = 0;
				for (int i = 0; i < str.size(); ++i) {
					switch (state)
					{
					case 0:
						if (str[i] >= '0' && str[i] <= '9')
						{
							state = 1;
						}
						else return false;
						break;
					case 1:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 1;
						}
						else if (str[i] == '.') {
							state = 2;
						}
						else return false;
						break;
					case 2:
						if (str[i] >= '0' && str[i] <= '9') { state = 3; }
						else return false;
						break;
					case 3:
						if (str[i] >= '0' && str[i] <= '9') { state = 3; }
						else if (str[i] == '.') {
							state = 4;
						}
						else return false;
						break;
					case 4:
						if (str[i] >= '0' && str[i] <= '9') { state = 5; }
						else return false;
						break;
					case 5:
						if (str[i] >= '0' && str[i] <= '9') { state = 5; }
						else if (str[i] == '.') { state = 6; }
						else return false;
						break;
					case 6:
						if (str[i] >= '0' && str[i] <= '9') { state = 7; }
						else return false;
						break;
					case 7:
						if (str[i] >= '0' && str[i] <= '9') { state = 7; }
						else if (str[i] == '.') {
							state = 8;
						}
						else return false;
						break;
					case 8:
						if (str[i] >= '0' && str[i] <= '9') { state = 9; }
						else return false;
						break;
					case 9:
						if (str[i] >= '0' && str[i] <= '9') { state = 9; }
						else return false;
						break;
					}
				}
				return 9 == state;
			}
			static bool IsMinus(std::string str)
			{
			//	if (str.size() > 2 && str[0] == str.back() && (str[0] == '\"' || str[0] == '\'')) {
			//		str = str.substr(1, str.size() - 2);
			//	}
				return str.empty() == false && str[0] == '-';
			}

			static std::string reverse(std::string str) { /// to std::reverse ?
				std::reverse(str.begin(), str.end());
				return str;
			}
			static std::string GetType(const std::string& str) {
				//if (str.size() > 2 && str[0] == str.back() && (str[0] == '\"' || str[0] == '\'')) {
				//	str = str.substr(1, str.size() - 2);
				//}
				int state = 0;
				for (size_t i = 0; i < str.size(); ++i) {
					switch (state)
					{
					case 0:
						if ('+' == str[i] || '-' == str[i]) {
							state = 0;
						}
						else if (str[i] >= '0' && str[i] <= '9')
						{
							state = 1;
						}
						else { return "STRING"; }
						break;
					case 1: 
						if (str[i] >= '0' && str[i] <= '9') {
							state = 1; // INTEGER
						}
						else if (str[i] == '.') {
							state = 2;
						}
						else { return "STRING"; }
						break;
					case 2:
						if (str[i] >= '0' && str[i] <= '9') { state = 3; } // DOUBLE
						else { return "STRING"; }
						break;
					case 3:
						if (str[i] >= '0' && str[i] <= '9') { state = 3; } // DOUBLE
						else if ('e' == str[i] || 'E' == str[i]) {
							state = 4;
						}
						else if ('.' == str[i]) {
							state = 7;
						}
						else { return "STRING"; }
						break;
					case 4:
						if (str[i] == '+' || str[i] == '-') {
							state = 5;
						}
						else {
							state = 5;
						}
						break;
					case 5:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 6; // DOUBLE
						}
						else {
							return "STRING";
						}
						break;
					case 6:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 6; // DOUBLE
						}
						else {
							return "STRING";
						}
						break;
					case 7:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 7; // DATE
						}
						else if ('.' == str[i]) {
							state = 8;
						}
						else {
							return "STRING";
						}
						break;
					case 8:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 8; // DATETIMEA
						}
						else if ('.' == str[i]) {
							state = 9;
						}
						else {
							return "STRING";
						}
						break;
					case 9:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 9; // DATETIMEB
						}
						else {
							return "STRING";
						}
						break;
					}
				}

				if (1 == state) { return "INTEGER"; }
				else if (3 == state || 6 == state) { return "FLOAT"; }
				else if (9 == state) { return "DATETIMEB"; }
				else if (8 == state) { return "DATETIMEA"; }
				else if (7 == state) { return "DATE"; }
				else return "STRING";
			}
			static std::string Compare(std::string str1, std::string str2,
				std::string _type1="", std::string _type2="", const int type = 0)
			{
				//if (str1.size() > 2 && str1[0] == str1.back() && (str1[0] == '\"' || str1[0] == '\''))
				//{
				//	str1 = str1.substr(1, str1.size() - 2);
				//}
				//if (str2.size() > 2 && str2[0] == str2.back() && (str2[0] == '\"' || str2[0] == '\''))
				//{
				//	str2 = str2.substr(1, str2.size() - 2);
				//}


				std::string type1; // ""
				std::string type2; // ""
				if (_type1.empty()) {
					type1 = GetType(str1);
				}
				else {
					type1 = _type1;
				}
				if (_type2.empty()) {
					type2 = GetType(str2);
				}
				else {
					type2 = _type2;
				}

				if (type1 != type2) {
					return "ERROR";
				}

			
				if ("STRING" == type1 || type == 1)
				{
					if (str1 < str2) {
						return "< 0";
					}
					else if (str1 == str2) {
						return "== 0";
					}
					return "> 0";
				}
				else if ("INTEGER" == type1)
				{
					if (Utility::IsMinus(str1) && !Utility::IsMinus(str2)) { return "< 0"; }
					else if (!Utility::IsMinus(str1) && Utility::IsMinus(str2)) { return "> 0"; }

					const bool minusComp = Utility::IsMinus(str1) && Utility::IsMinus(str2);

					if (false == minusComp) {
						if (str1[0] == '+') { str1 = str1.substr(1); }
						if (str2[0] == '+') { str2 = str2.substr(1); }

						std::string x = reverse(str1);
						std::string y = reverse(str2);



						if (x.size() < y.size()) {
							while (x.size() < y.size()) {
								x.push_back('0');
							}
						}
						else {
							while (y.size() < x.size()) {
								y.push_back('0');
							}
						}
						return Compare(reverse(x), reverse(y), "INTEGER", "INTEGER", 1);
					}
					else {
						return Compare(str2.substr(1), str1.substr(1), "INTEGER", "INTEGER");
					}
				}
				else if ("DOUBLE" == type1)
				{
					size_t x_pos = str1.find('.');
					size_t y_pos = str2.find('.');

					std::string x = str1; x = x.substr(0, x_pos);
					std::string y = str2; y = y.substr(0, y_pos);

					std::string z = Compare(x, y, "INTEGER", "INTEGER");
					if ("== 0" == z)
					{
						x = str1; x.substr(x_pos + 1);
						y = str2; y.substr(y_pos + 1);

						if (x.size() < y.size()) {
							while (x.size() < y.size()) {
								x.push_back('0');
							}
						}
						else {
							while (y.size() < x.size()) {
								y.push_back('0');
							}
						}
						return Compare(x, y, "INTEGER", "INTEGER", 1);
					}
					else
					{
						return z;
					}
				}
				else if ("DATE" == type1)
				{
					size_t x_pos = str1.find('.');
					size_t y_pos = str2.find('.');

					std::string x = str1; x = x.substr(0, x_pos);
					std::string y = str2; y = y.substr(0, y_pos);

					for (int i = 0; i < 3; ++i) {
						const std::string comp = Compare(x, y);

						if (comp == "< 0") { return comp; }
						else if (comp == "> 0") { return comp; }
						x = str1; x = x.substr(x_pos + 1);
						y = str2; y = y.substr(y_pos + 1);
						x_pos = str1.find('.', x_pos + 1);
						y_pos = str2.find('.', y_pos + 1);
					}
					return "== 0";
				}
				else if ("DATETIMEA" == type1) {
					size_t x_pos = str1.find('.');
					size_t y_pos = str2.find('.');

					std::string x = str1; x = x.substr(0, x_pos);
					std::string y = str2; y = y.substr(0, y_pos);

					for (int i = 0; i < 4; ++i) {
						const std::string comp = Compare(x, y, "INTEGER", "INTEGER");

						if (comp == "< 0") { return comp; }
						else if (comp == "> 0") { return comp; }

						x = str1; x = x.substr(x_pos + 1);
						y = str2; y = y.substr(y_pos + 1);
						x_pos = str1.find('.', x_pos + 1);
						y_pos = str2.find('.', y_pos + 1);
					}
					return "== 0";
				}
				else if ("DATETIMEB" == type2) {
					size_t x_pos = str1.find('.');
					size_t y_pos = str2.find('.');

					std::string x = str1; x = x.substr(0, x_pos);
					std::string y = str2; y = y.substr(0, y_pos);

					for (int i = 0; i < 5; ++i) {
						const std::string comp = Compare(x, y, "INTEGER", "INTEGER");

						if (comp == "< 0") { return comp; }
						else if (comp == "> 0") { return comp; }

						x = str1; x = x.substr(x_pos + 1);
						y = str2; y = y.substr(y_pos + 1);
						x_pos = str1.find('.', x_pos + 1);
						y_pos = str2.find('.', y_pos + 1);
					}
					return "== 0";
				}
				return "ERROR";
			}

			static std::string BoolOperation(std::string op, std::string x, std::string y)
			{
				if (x == "ERROR" || y == "ERROR") { return "ERROR"; }
				if ("NOT" == op) { return x == "TRUE" ? "FALSE" : "TRUE"; }
				else if ("AND" == op) {
					if (x == "TRUE" && y == "TRUE") { return "TRUE"; }
					else {
						return "FALSE";
					}
				}
				else if ("OR" == op) {
					if (x == "TRUE" || y == "TRUE") { return "TRUE"; }
					else {
						return "FALSE";
					}
				}
				else {
					return "ERROR";
				}
			}

		public:
			class BomInfo
			{
			public:
				long long bom_size;
				char seq[5];
			};
			
			const static size_t BOM_COUNT = 1;

			enum BomType { UTF_8, UNDEFINED = -1 }; // ANSI - UNDEFINED
			
			inline static const BomInfo bomInfo[1] = {
				{ 3, { (char)0xEF, (char)0xBB, (char)0xBF } }
			};
			static BomType ReadBom(std::ifstream& file) {
				char btBom[5] = { 0, };
				file.read(btBom, 5);
				size_t readSize = file.gcount();

				if (0 == readSize) {
					file.clear();
					file.seekg(0, std::ios_base::beg);
					return UNDEFINED;
				}

				BomInfo stBom = { 0, };
				BomType type = ReadBom(btBom, readSize, stBom);
				
				if (type == UNDEFINED) { // ansi
					file.clear();
					file.seekg(0, std::ios_base::beg);
					return UNDEFINED;
				}
				
				file.clear();
				file.seekg(stBom.bom_size, std::ios_base::beg);
				return type;
			}

			static BomType ReadBom(const char* contents, size_t length, BomInfo& outInfo) {
				char btBom[5] = { 0, };
				size_t testLength = length < 5 ? length : 5;
				memcpy(btBom, contents, testLength);

				size_t i, j;
				for (i = 0; i < BOM_COUNT; ++i) {
					const BomInfo& bom = bomInfo[i];
					
					if (bom.bom_size > testLength) {
						continue;
					}

					bool matched = true;

					for (j = 0; j < bom.bom_size; ++j) {
						if (bom.seq[j] == btBom[j]) {
							continue;
						}

						matched = false;
						break;
					}

					if (!matched) {
						continue;
					}

					outInfo = bom;

					return (BomType)i;
				}

				return UNDEFINED;
			}

		};
	}
}

#endif
