
#define _CRT_SECURE_NO_WARNINGS

#include "wiz/global.h"
#include "wiz/load_data_utility.h"

namespace wiz {
	inline std::string Remove(const std::string& str)
	{
		if (str.size() >= 2 && str.front() == str.back() && str.back() == '\"') {
			return str.substr(1, str.size() - 2);
		}
		return str;
	}

	long long checkDelimiter(const char* start, const char* last, const std::vector<std::string>& delimiter)
	{
		const int delim_size = delimiter.size();

		for (int delim_num = 0; delim_num < delim_size; ++delim_num) {
			bool pass = true;

			// size check
			if (start + delimiter[delim_num].size() - 1 > last) {
				continue;
			}

			for (const char* x = start; x <= start + delimiter[delim_num].size() - 1; ++x) {
				if (*x == delimiter[delim_num][x - start]) {

				}
				else {
					pass = false;
					break;
				}
			}

			if (pass) {
				return delim_num;
			}
		}

		return -1;
	}



	LineInfo GetLineInfo(long long x, long long* lines, long long lines_len, long long& start) {
		if (!lines || 0 == x) {
			return LineInfo();
		}
		long long idx = GetIdx(x);
		LineInfo result;

		long long i = start;
		for (; i < lines_len; ++i) {
			if (lines[i] >= idx) {
				break;
			}
		}

		result.line = i + 1;
		result.distance = i > 0 ? idx - lines[i - 1] : idx;
		start = i;

		return result;
	}



	int DataType::GetType()const {
		if (change) {
			if (wiz::load_data::Utility::IsInteger(str_value)) {
				type = 3; 
				int_value = ToInt();
			}
			else if (wiz::load_data::Utility::IsDouble(str_value)) {
				type = 5;
				float_value = ToFloat();
			}
			else {
				//throw "error in gettype in datatype";
			}
		}
		change = false;

		return type;
	}

	std::string DataType::GetTypeString()const
	{
		int value = GetType();

		if (3 == value) {
			return "INTEGER";
		}
		else if (5 == value) {
			return "FLOAT";
		}
		else {
			return "STRING";
		}
	}

	void DataType::SetInt(long long val)
	{
		this->type = 3;
		this->int_value = val;
		this->str_value = wiz::_toString(val);
		change = false;
	}

	void DataType::SetFloat(long double val)
	{
		this->type = 5;
		this->float_value = val;
		this->str_value = wiz::_toString(val);
		change = false;
	}


	DataType::DataType(const char* cstr, size_t len)
	{
		if (len <= 0) {
			std::cout << "chk\n";
		}
		this->str_value = std::string(cstr, len);

		if (USE_REMOVE_IN_DATATYPE) {
			this->str_value = Remove(str_value);
		}
		/* // #ifdef DataTypeDebug?
		this->change = true;

		if (wiz::load_data::Utility::IsInteger(this->str_value)) {
			this->type = 3;
			this->int_value = ToInt();
		}
		else if (wiz::load_data::Utility::IsDouble(this->str_value)) {
			this->type = 5;
			this->float_value = ToFloat();
		}
		else {
			this->type = 1;
		}
		*/

		this->change = true; // false;
	}

	DataType::DataType(const char* cstr, size_t len, const LineInfo& opt)
	{
		if (len <= 0) {
			std::cout << "chk\n";
		}
		this->str_value = std::string(cstr, len);

		if (USE_REMOVE_IN_DATATYPE) {
			this->str_value = Remove(str_value);
		}

		this->lineInfo = opt;
		/* // #ifdef DataTypeDebug?
		this->change = true;

		if (wiz::load_data::Utility::IsInteger(this->str_value)) {
			this->type = 3;
			this->int_value = ToInt();
		}
		else if (wiz::load_data::Utility::IsDouble(this->str_value)) {
			this->type = 5;
			this->float_value = ToFloat();
		}
		else {
			this->type = 1;
		}
		*/

		this->change = true; // false;
	}

	DataType::DataType(const char* cstr)
	{
		this->str_value = std::string(cstr);
		
		if (USE_REMOVE_IN_DATATYPE) {
			this->str_value = Remove(str_value);
		}
		/* // #ifdef DataTypeDebug?
		this->change = true;

		if (wiz::load_data::Utility::IsInteger(this->str_value)) {
			this->type = 3;
			this->int_value = ToInt();
		}
		else if (wiz::load_data::Utility::IsDouble(this->str_value)) {
			this->type = 5;
			this->float_value = ToFloat();
		}
		else {
			this->type = 1;
		}
		*/

		this->change = true; // false;
	}
	DataType::DataType(const std::string& str)
	{
		this->str_value = str;

		if (USE_REMOVE_IN_DATATYPE) {
			this->str_value = Remove(str_value);
		}

		/*
		this->change = true;

		if (wiz::load_data::Utility::IsInteger(this->str_value)) {
			this->type = 3;
			this->int_value = ToInt();
		}
		else if (wiz::load_data::Utility::IsDouble(this->str_value)) {
			this->type = 5;
			this->float_value = ToFloat();
		}
		else {
			this->type = 1;
		}
		*/
		this->change = true; // false;
	}	
	
	DataType::DataType(const std::string& str, const LineInfo& opt)
	{
		this->str_value = str;

		if (USE_REMOVE_IN_DATATYPE) {
			this->str_value = Remove(str_value);
		}

		this->lineInfo = opt;
		/*
		this->change = true;

		if (wiz::load_data::Utility::IsInteger(this->str_value)) {
			this->type = 3;
			this->int_value = ToInt();
		}
		else if (wiz::load_data::Utility::IsDouble(this->str_value)) {
			this->type = 5;
			this->float_value = ToFloat();
		}
		else {
			this->type = 1;
		}
		*/
		this->change = true; // false;
	}

	DataType::DataType(std::string&& str)
	{
		this->str_value = std::move(str);

		if (USE_REMOVE_IN_DATATYPE) {
			this->str_value = Remove(str_value);
		}

		/*
		this->change = true;

		if (wiz::load_data::Utility::IsInteger(this->str_value)) {
			this->type = 3;
			this->int_value = ToInt();
		}
		else if (wiz::load_data::Utility::IsDouble(this->str_value)) {
			this->type = 5;
			this->float_value = ToFloat();
		}
		else {
			this->type = 1;
		}
		*/
		this->change = true; // false;
	}

	bool DataType::operator==(const DataType& type) const
	{
		return type.str_value == this->str_value;
	}
	bool DataType::operator==(const char* cstr) const
	{
		return cstr == this->str_value;
	}
	bool DataType::operator==(std::string_view str) const
	{
		return this->str_value == str;
	}
	bool DataType::operator==(const std::string& str) const
	{
		return str == this->str_value;
	}

	bool DataType::operator!=(const DataType& type) const
	{
		return type.str_value != this->str_value;
	}
	bool DataType::operator!=(const char* cstr) const
	{
		return cstr != this->str_value;
	}
	bool DataType::operator!=(const std::string& str) const
	{
		return str != this->str_value;
	}

	DataType DataType::operator+(const DataType& type)const
	{
		return DataType(this->str_value + type.str_value);
	}
	DataType DataType::operator+(const char* cstr) const
	{
		return DataType(this->str_value + std::string(cstr));
	}
	DataType DataType::operator+(const std::string& str) const
	{
		return DataType(this->str_value + str);
	}

	DataType& DataType::operator+=(const DataType& type) 
	{
		this->str_value += type.str_value;

		this->change = true;
		return *this;
	}
	DataType& DataType::operator+=(const char* cstr) 
	{
		(*this) += DataType(cstr);
		return *this;
	}
	DataType& DataType::operator+=(const std::string& str)
	{
		(*this) += DataType(str);
		return *this;
	}

	bool operator==(const char* cstr, const DataType& type)
	{
		return type == cstr;
	}
	bool operator==(std::string_view str, const DataType& type)
	{
		return type == str;
	}
	bool operator==(const std::string& str, const DataType& type)
	{
		return type == str;
	}

	bool operator!=(const char* cstr, const DataType& type)
	{
		return type != cstr;
	}
	bool operator!=(const std::string& str, const DataType& type)
	{
		return type != str;
	}

	DataType operator+(const char* cstr, const DataType& type)
	{
		return DataType(cstr) + type;
	}
	DataType operator+(const std::string& str, const DataType& type)
	{
		return DataType(str) + type;
	}


	std::vector<std::string> tokenize(std::string sv, char ch) {
		std::vector<std::string> result;
		size_t x;
		if ((x = sv.find(ch)) == std::string::npos) {
			if (!sv.empty()) {
				result.push_back(sv);
			}
			return result;
		}

		if (x > 0) {
			result.push_back(sv.substr(0, x));
		}

		size_t y;
		while (x != std::string::npos) {
			y = sv.find(ch, x + 1);

			if (y == std::string::npos) {
				if (x + 1 < sv.size()) {
					result.push_back(sv.substr(x + 1));
				}
				break;
			}
			else {
				if (y - 1 - x + 1 > 0) {
					result.push_back(sv.substr(x + 1, y - 1 - (x + 1) + 1));
				}
			}

			x = y;
		}

		return result;
	}


}

