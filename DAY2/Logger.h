#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <io.h>
#include <direct.h>
#include <fstream>

class LOGGER
{

public:
	bool TESTMODE;
	std::string funcName;
	void print(std::string s);
	void log_write();
	void update_time();
	void changeFile();
	void consolePrint(std::string s);
private:
	std::string msg;
	std::tm* tm_now;
	std::string getLogs();
};
