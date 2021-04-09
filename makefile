ClauScript:
	g++   -std=c++1z ./source/main.cpp ./source/ClauText.cpp ./source/global.cpp ./source/load_data.cpp -I./include -lpthread -pthread     -Wno-narrowing  -O2 -o ClauScript
