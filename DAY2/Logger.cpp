#define _CRT_SECURE_NO_WARNINGS
#include "Logger.h"

void LOGGER::consolePrint(std::string s)
{
	if (!TESTMODE)std::cerr << s << '\n';
}

// 입력받은 유저 로그 메시지를 사용해서 로그를 기록
void LOGGER::print(std::string s) 
{
	if (funcName == "")
	{
		consolePrint("함수명이 존재하지 않습니다. 함수를 실행시키세요");
		return;
	}

	msg = s;
	log_write();
	funcName = "";
}

std::string LOGGER::getLogs()
{
	std::ostringstream oss;
	// 날짜 형식 만들기
	oss << "[" << std::put_time(tm_now, "%y.%m.%d %H:%M") << "]";
	// 함수 형식 만들기
	oss << std::setw(30) << std::left << funcName;
	// 메시지 넣기
	oss<<" : " << msg;

	std::string log_data = oss.str();

	consolePrint(log_data);
	return log_data;
}

void LOGGER::log_write()
{
	// 실제로 작성하기 -> 테스트 모드가 아닐때만 작성한다
	// 로그 파일 열기
	std::fstream logFile("latest.log", std::ios::app);
	std::string log_data;
	// 크기 계산
	int new_log_size = sizeof(log_data);
	logFile.seekp(0, std::ios::end);
	int size = logFile.tellp();

	// 10kb 초과
	if (size + new_log_size > 10000)
	{
		logFile.close();
		changeFile();
		std::fstream logFile("latest.log", std::ios::app);
		update_time();
		log_data = getLogs();
		logFile << log_data;
		logFile.close();
		return;
	}
	update_time();
	log_data = getLogs();
	logFile << log_data;
	logFile.close();
}

void LOGGER::update_time()
{
	auto now = std::chrono::system_clock::now();
	time_t now_time = std::chrono::system_clock::to_time_t(now);
	tm_now = std::localtime(&now_time);
}

void LOGGER:: changeFile()
{
	// 현재 디렉토리 파일 확인
	char CurDir[1000];
	_getcwd(CurDir, 1000);
	std::string dir(CurDir);
	dir += "\\*.log";

	struct _finddata_t fd;
	intptr_t handle;

	if ((handle = _findfirst(dir.c_str(), &fd)) == -1L)
		consolePrint("No File in Directort *.log files !");

	_findnext(handle, &fd);
	_findnext(handle, &fd);
	_findclose(handle);

	std::string otherLog = std::string(fd.name);
	std::string newName;

	// 다른 로그 이름 변경(.log -> .zip)
	if (otherLog != "latest.log")
	{
		newName = otherLog.substr(0, otherLog.length() - 3) + "zip";
		if (std::rename(fd.name, &newName[0]) != 0) 
			consolePrint("이름 변경 실패\n");
	}

	// latest 로그 이름 변경 (latest -> util~ )
	std::ostringstream oss;
	oss << "util_" << std::put_time(tm_now, "%y%m%d_%Hh_%Mm_%Ss.log");
	newName = oss.str();

	if(std::rename("latest.log", &newName[0])!=0)
		consolePrint("이름 변경 실패");

}
