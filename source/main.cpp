

#define _CRT_SECURE_NO_WARNINGS

// memory leak test.
#ifdef _DEBUG 
//#include <vld.h>
#endif

// Log
#include <string>
#include <fstream>


// Array idx chk test.
//#define ARRAYS_DEBUG

#include <iostream>
#include <ctime>
#include <cstdlib>

//#define USE_FAST_LOAD_DATA // no use?
#include "wiz/ClauText.h"


int main(int argc, char* argv[])
{
	srand(0);
	//srand(time(nullptr)); // 

	std::string fileName;


	if (argc == 1) {
		std::cout << "FileName: ";
		std::getline(std::cin, fileName);
	}
	else
	{
		fileName = std::string(argv[1]);
	}

	wiz::load_data::UserType global;

	try {
		int a = clock();
		{
			//	wiz::load_data::LoadData::LoadDataFromFile(fileName, global);
		}
		int b = clock();
		//wiz::Out << "time " << b - a << "ms" << ENTER;

	//	global.Remove();

		for (int i = 1; i <= 1; ++i) { // i : pivot_num, thread num <= pivot_num + 1
		//	global.Remove();

			a = clock();
			{
			
				wiz::load_data::LoadData::LoadDataFromFile(fileName, global, 0, 0);
			
			}
			b = clock();
			wiz::Out << "time " << b - a << "ms" << ENTER;
		}
		//global.Remove();

		a = clock();
		{
			//wiz::load_data::LoadData::LoadDataFromFile4(fileName, global); // parallel lexing + parallel parsing
		}
		b = clock();
		//wiz::Out << "time " << b - a << "ms" << ENTER;

		wiz::Out << "fileName is " << fileName << ENTER;

		//wiz::load_data::LoadData::SaveWizDB(global, "test2.eu4", "3"); // 3 : JSON
		//std::cout << global.ToString() << "\n";

		wiz::Option opt;
		{
			int a = clock();
			wiz::Out.clear_file();
			std::string result = clauText.excute_module("", &global, wiz::ExecuteData(), opt, 0);
			int b = clock();
			wiz::Out << "excute result is " << result << "\n";
			wiz::Out << b - a << "ms" << "\n"; //
		}
	}
	catch (const char* str) {
		wiz::Out << str << ENTER;

		GETCH();
	}
	catch (const std::string& str) {
		wiz::Out << str << ENTER;
		GETCH();
	}
	//catch (const wiz::Error& e) {
	//	wiz::Out << e << ENTER;
	//	GETCH();
	//}
	catch (const std::exception&e) {
		wiz::Out << e.what() << ENTER;
		GETCH();
	}

#ifndef _DEBUG
	//catch (...) {
	//	wiz::Out << "UnKnown Error.." << ENTER;
	//	GETCH();
	//}
#endif

   	return 0;
}

