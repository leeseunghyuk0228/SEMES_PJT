#include "Logger.h"
#include <stdlib.h>
#include <regex>

#define WRITE 1
#define READ 2
#define FULLWRITE 3
#define FULLREAD 4
#define FULLWRITECOMPARE 5
#define FULLREAD10COMPARE 6
#define HELP 7
#define EXIT 8
#define ERASE 9
#define ERASE_RANGE 10

int Check_Cmd(std::string target);
bool Check_Index(std::string idx);
bool Check_Value(std::string val);
unsigned int str_to_unint(std::string s);
unsigned int SSD_READ(std::string lba);
void SSD_WRITE(std::string lba, std::string value);
void SSD_FULLWRITE(std::string val);
void SSD_FULLREAD();
void PRINT_HELP();
void SSD_ERASE(std::string lba,std::string size,int ver);
bool FullWriteReadCompare();
bool FullRead10AndComapre();
void myPrint(std::string s);
bool TESTMODE = false;


std::string BASE_CMD = "SSD.exe";
std::string cmd, slba, elba, val;
std::string res = BASE_CMD;

LOGGER logger;

int main(int argc,char* args[])
{
	if (argc > 1)
	{
		TESTMODE = true;
		freopen(std::string(args[1]).c_str(), "r", stdin);
	}
	logger.TESTMODE = TESTMODE;
	

	while (1)
	{
		myPrint("[INPUT COMMAND]:");
		std::cin >> cmd;
		int ret = Check_Cmd(cmd);
		if (TESTMODE)std::cerr << cmd << "  ___  ";
		if (ret == -1)
		{
			myPrint("INVALID COMMAND\n");
			if(!TESTMODE)continue;
			std::cerr << "fail\n";
			return 0;
		}

		switch (ret)
		{
		case WRITE:
			std::cin >> slba >> val;
			if (!Check_Index(slba)) continue;
			if (!Check_Value(val)) continue;
			SSD_WRITE(slba, val);
			break;
		case READ:
			std::cin >> slba;
			if (!Check_Index(slba)) continue;
			SSD_READ(slba);
			break;
		case FULLWRITE:
			std::cin >> val;
			if (!Check_Value(val)) continue;
			SSD_FULLWRITE(val);
			break;
		case FULLREAD:
			SSD_FULLREAD();
			break;
		case ERASE:
			std::cin >> slba >> elba;
			SSD_ERASE(slba, elba, 1);
			break;
		case ERASE_RANGE:
			std::cin >> slba >> elba;
			SSD_ERASE(slba, elba, 2);
			break;
		case FULLWRITECOMPARE:
			if(!FullWriteReadCompare())return 0;
			break;
		case FULLREAD10COMPARE:
			if(!FullRead10AndComapre())return 0;
			break;
		case HELP:
			PRINT_HELP();
			break;
		case EXIT:
			logger.funcName = "EXIT";
			logger.print("shell terminated");
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
	else if (target == "FullWriteReadCompare") return 5;
	else if (target == "FullRead10AndComapre") return 6;
	else if (target == "help") return 7;
	else if (target == "exit")return 8;
	else if (target == "erase") return 9;
	else if (target == "erase_range") return 10;
	return -1;
}
bool Check_Index(std::string idx)
{
	int temp = stoi(idx);
	if (temp >= 0 && temp <= 99) return true;
	myPrint("WRONG INDEX [INDEX : 0~99 ]\n");
	return false;
}
bool Check_Value(std::string val)
{
	// 형태 정규식
	std::regex pattern("^0x[0-9A-F]{8}$");

	if (std::regex_match(val, pattern))return true;
	myPrint("WRONG VALUE FORMAT [FORMAT 0x[0-9A-F]{8}\n");
	return false;

}
unsigned int str_to_unint(std::string s)
{
	unsigned int value = std::stoul(s, nullptr, 16);
	return value;
}
unsigned int SSD_READ(std::string lba)
{
	logger.funcName = "SSD_READ()";
	std::string res;
	res = BASE_CMD + " R " + lba;
	system(res.c_str());

	unsigned int readData;
	std::ifstream inFile("result.txt", std::ios::binary);
	inFile.read(reinterpret_cast<char*>(&readData), sizeof(readData));
	inFile.close();

	std::ostringstream oss;

	oss <<"0x" << std::hex << std::uppercase
		<< std::setw(8)
		<< std::setfill('0')
		<< readData;

	std::string value = oss.str();

	logger.print("ssd read success [" + lba + "]:" + value);
	return readData;
}
void SSD_WRITE(std::string lba, std::string value)
{
	logger.funcName = "SSD_WRITE()";
	std::string res;
	res = BASE_CMD + " W " + lba + " " + value;
	system(res.c_str());
	logger.print("ssd write success [" + lba + "]:" + value);
}
void SSD_FULLWRITE(std::string val)
{
	logger.funcName = "FULL_WRITE()";
	logger.print("fw start");
	for (int i = 0; i < 100; i++)
		SSD_WRITE(std::to_string(i), val);
	logger.funcName = "FULL_WRITE()";
	logger.print("fw finish");

}
void SSD_FULLREAD()
{
	logger.funcName = "FULLREAD()";
	logger.print("fr start");
	for (int i = 0; i < 100; i++)
		SSD_READ(std::to_string(i));
	logger.funcName = "FULLREAD()";
	logger.print("fr finish");
}
void PRINT_HELP()
{
	if (TESTMODE) return;
	logger.funcName = "PRINT_HELP()";
	myPrint("\n==============COMMAND LIST=============\n");
	myPrint("[write LBA VALUE]: you can write on SSD\n");
	myPrint("[read LBA]       : you can read the SSD\n");
	myPrint("[fullwrite VALUE]: you can write full range of the SSD with the value\n");
	myPrint("[fullread]       : you can read full range of the SSD\n\n");
	logger.print("print help func success");
}
bool FullWriteReadCompare()
{
	unsigned int cmp;
	logger.funcName = "FullWriteReadCompare()";
	std::string val;
	std::cin >> val;

	if (!Check_Value(val))
	{
		logger.print("FullWriteReadCompare fail [INVALID VALUE]");
		return false;
	}
	logger.print("FullWriteReadCompare start");

	SSD_FULLWRITE(val);
	cmp = str_to_unint(val);

	for (int i = 0; i < 100; i++)
	{
		if (cmp == SSD_READ(std::to_string(i))) continue;
		logger.print("FullWriteReadCompare fail!!");
		std::cerr << "fail\n";
		return 0;
		break;
	}
	std::cerr << "pass\n";
	logger.funcName = "FullWriteReadCompare()";
	logger.print("FullWriteReadCompare success!!");
	return 1;
}
bool FullRead10AndComapre()
{
	unsigned int cmp;
	logger.funcName = "TEST2()";
	cmp = str_to_unint("0x12345678");
	for (int i = 0; i < 30; i++) SSD_WRITE(std::to_string(i % 6), "0xAAAABBBB");
	for (int i = 0; i < 6; i++) SSD_WRITE(std::to_string(i), "0x12345678");
	for (int i = 0; i < 6; i++)
	{
		if (cmp == SSD_READ(std::to_string(i))) continue;
		std::cerr << " fail\n";
		logger.funcName = "TEST2()";
		logger.print("test2 fail!!");
		return 0;
	}
	std::cerr << "pass\n";
	logger.funcName = "TEST2()";
	logger.print("test2 success!!");
	return 1;
}
void SSD_ERASE(std::string slba, std::string size, int ver)
{
	int st, en;

	if (ver == 1)
	{
		st = std::stoi(slba);
		en = st + std::stoi(size)-1;
		for (int lba = st; lba <= en; lba++)
			SSD_WRITE(std::to_string(lba), "0x00000000");
		logger.funcName = "ERASE()";
		logger.print("ERASE from " + slba+" to " + std::to_string(en) + " success");
	}
	else if (ver == 2)
	{
		st = std::stoi(slba);
		en = std::stoi(size);
		for (int lba = st; lba < en; lba++)
			SSD_WRITE(std::to_string(lba), "0x00000000");
		logger.funcName = "ERASE_RANGE()";
		logger.print("ERASE from " + slba+" to " + std::to_string(en-1) + " success");

	}
}
void myPrint(std::string s){
if (!TESTMODE)
std::cerr << s ;
}
