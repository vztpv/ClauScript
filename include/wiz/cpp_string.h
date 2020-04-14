#ifndef CPP_STRING_H_INCLUDED
#define CPP_STRING_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS

#include "global.h"
#include "STRINGBUILDER.H"
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

namespace wiz {
	class String {
	public:
		static std::string upper(std::string str) {
			if (str.empty()) { return str; }

			for (long long i = str.size() - 1; i >= 0; --i) {
				str[i] = toupper(str[i]);
			}
			return str;
		}
		static int find(std::string str, std::string dilim, const int start_index)
		{
			if (str.empty() || dilim.empty()) { return -1; }

			int k = 0;

			for (int i = start_index; i < str.size(); i++) {
				if (str[i] == dilim[k]) { k++; if (dilim.size() == k) { return i - dilim.size() + 1; } }
				else {
					k = 0;
					if (str[i] == dilim[k]) { k++; if (dilim.size() == k) { return i - dilim.size() + 1; } }
				}
			}

			return -1;
		}
		static bool Comp(const char* cstr1, const char* cstr2, const int n) /// isSameData
		{
			for (int i = 0; i < n; i++)
			{
				if (cstr1[i] != cstr2[i]) { return false; }
			}
			return true;
		}
		static bool Comp(std::string str1, std::string str2, const int n) /// isSameData
		{
			for (int i = 0; i < n; i++)
			{
				if (str1[i] != str2[i]) { return false; }
			}
			return true;
		}
		// fixed more fast!
		static std::string replace(const std::string& str, std::string str1, std::string str2)
		{
			if (str1.empty()) { return ""; }

			std::string temp = str;

			if (temp.size() < str1.size()) {
				throw "Error in replace";
			}

			for (long long i = temp.size() - str1.size(); i >= 0;) {
				i = temp.rfind(str1.data(), i);

				if (i == std::string::npos) {
					break;
				}
				else {
					temp.replace(temp.begin() + i, temp.begin() + i + str1.size(), str2.data());
					--i;
				}
			}

			return temp;

			/*
			// if str1 is empty, then infinite loop. // USE more speed pointer?
			const char* pStr = str.c_str();
			std::string result;

			for( int i=0; i < str.size(); i++ ) {
				if( strlen( pStr+i ) >= str1.size()
				   && Comp( pStr+i, str1.c_str(), str1.size() ) ) // check!
				{
					result += str2; //
					i = i + str1.size() - 1;
				}
				else
				{
					result.push_back( str[i] );
				}
			}

			return result;
			*/
		}
		static std::string remove(const std::string& str, std::string str1)
		{
			return replace(str, str1, std::string(""));
		}
		//start index
		static int indexOf(std::string str, const char ch)
		{
			for (int i = 0; i < str.size(); i++)
			{
				if (str[i] == ch) { return i; }
			}
			return -1;
		}
		// chk..
		static std::pair< bool, size_t > indexOf(std::string str, std::string str1, const int fromIndex)
		{
			if (str1.empty()) { return{ false, 0 }; }
			auto idx = str.find(str1, fromIndex);
			if (idx == std::string::npos) { return{ false, 0 }; }
			return{ true, idx };
		}
		static auto indexOf(std::string str, std::string str1)
		{
			return indexOf(str, str1, 0);
		}
		
		static std::string substring(const std::string& str, const int start, const int last)
		{
			return str.substr(start, last - start + 1);
		}
		static std::string substring(const std::string& str, const int start)
		{
			return substring(str, start, str.size() - 1);
		}
		static bool startsWith(std::string_view str, std::string_view start)
		{
			if (str.size() < start.size()) { return false; }
			return Comp(str.data(), start.data(), start.size());
		}

		static bool endsWith(std::string_view str, std::string_view last)
		{
			if (str.size() < last.size()) { return false; }
			if (last.empty()) { return true; } /// chk... return false; 
			return Comp(str.data() + (str.size() - last.size()), last.data(), last.size());
		}
	};
}

#endif // CPP_STRING_H_INCLUDED
