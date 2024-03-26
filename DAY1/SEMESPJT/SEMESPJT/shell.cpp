#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <regex>
#include <iomanip> 

#define WRITE 1
#define READ 2
#define FULLWRITE 3
#define FULLREAD 4
#define TEST1 5
#define TEST2 6
#define HELP 7
#define EXIT 8

std::string BASE_CMD = "SSD.exe";

int Check_Cmd(std::string target);
bool Check_Index(std::string idx);
bool Check_Value(std::string val);
unsigned int str_to_unint(std::string s);
unsigned int SSD_READ(std::string lba);
void SSD_WRITE(std::string lba, std::string value);
void SSD_FULLWRITE(std::string val);
void PRINT_HELP();


int main()
{
	while (1)
	{
		unsigned int cmp;
		std::string cmd, lba, val;
		std::string res = BASE_CMD;
		std::cerr << "[INPUT COMMAND]:";
		std::cin >> cmd;

		int ret = Check_Cmd(cmd);

		if (ret == -1)
		{
			std::cerr << "INVALID COMMAND\n";
			continue;
		}

		switch (ret)
		{
		case WRITE:
			std::cin >> lba >> val;
			if (!Check_Index(lba)) continue;
			if (!Check_Value(val)) continue;
			SSD_WRITE(lba, val);
			break;
		case READ:
			std::cin >> lba;
			if (!Check_Index(lba)) continue;
			SSD_READ(lba);
			break;
		case FULLWRITE:
			std::cin >> val;
			if (!Check_Value(val)) continue;
			SSD_FULLWRITE(val);
			break;
		case FULLREAD:
			for (int i = 0; i < 100; i++) SSD_READ(std::to_string(i));
			break;
		case TEST1:
			std::cin >> val;
			if (!Check_Value(val)) continue;
			SSD_FULLWRITE(val);
			cmp = str_to_unint(val);
			for (int i = 0; i < 100; i++)
			{
				if (cmp == SSD_READ(std::to_string(i))) continue;
				std::cerr << "FAIL\n";
				break;
			}
			std::cerr << "TEST1 SUCCESS\n";
			break;
		case TEST2:
			cmp = str_to_unint("0x12345678");
			for (int i = 0; i < 30; i++)
				SSD_WRITE(std::to_string(i % 6), "0xAAAABBBB");
			for (int i = 0; i < 6; i++)
				SSD_WRITE(std::to_string(i), "0x12345678");
			for (int i = 0; i < 6; i++)
			{
				if (cmp == SSD_READ(std::to_string(i))) continue;
				std::cerr << "FAIL\n";
				break;
			}
			std::cerr << "TEST2 SUCCESS\n";
			break;
		case HELP:
			PRINT_HELP();
			break;
		case EXIT:
			std::cerr << "@@@@@@@@@@@ SHELL TERMINATE @@@@@@@@@@@\n";
			return 0;
		}
	}
	return 0;
}


int Check_Cmd(std::string target)
{
	if (target == "write") return 1;
	else if (target == "read") return 2;
	else if (target == "fullwrite") return 3;
	else if (target == "fullread") return 4;
	else if (target == "testapp1") return 5;
	else if (target == "testapp2") return 6;
	else if (target == "help") return 7;
	else if (target == "exit")return 8;

	return -1;
}
bool Check_Index(std::string idx)
{
	int temp = stoi(idx);
	if (temp >= 0 && temp <= 99) return true;
	std::cerr << "WRONG INDEX [INDEX : 0~99 ]\n";
	return false;
}
bool Check_Value(std::string val)
{
	// 형태 정규식
	std::regex pattern("^0x[0-9A-F]{8}$");

	if (std::regex_match(val, pattern))return true;
	std::cerr << "WRONG VALUE FORMAT [FORMAT 0x[0-9A-F]{8}\n";
	return false;

}
unsigned int str_to_unint(std::string s)
{
	unsigned int value = std::stoul(s, nullptr, 16);
	return value;
}
void SSD_WRITE(std::string lba, std::string value)
{
	std::string res;
	res = BASE_CMD + " W " + lba + " " + value;
	system(res.c_str());
}
unsigned int SSD_READ(std::string lba)
{
	std::string res;
	res = BASE_CMD + " R " + lba;
	system(res.c_str());

	unsigned int readData;
	std::ifstream inFile("result.txt", std::ios::binary);
	inFile.read(reinterpret_cast<char*>(&readData), sizeof(readData));
	inFile.close();

	std::cerr << lba << ": 0x"
		<< std::hex << std::uppercase
		<< std::setw(8)
		<< std::setfill('0')
		<< readData << "\n";
	return readData;
}
void SSD_FULLWRITE(std::string val)
{
	for (int i = 0; i < 100; i++)
		SSD_WRITE(std::to_string(i), val);
}
void PRINT_HELP()
{
	std::cerr << "\n==============COMMAND LIST=============\n";
	std::cerr << "[write LBA VALUE]: you can write on SSD\n";
	std::cerr << "[read LBA]       : you can read the SSD\n";
	std::cerr << "[fullwrite VALUE]: you can write full range of the SSD with the value\n";
	std::cerr << "[fullread]       : you can read full range of the SSD\n\n";
}