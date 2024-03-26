#define _CRT_SECURE_NO_WARNINGS
#include "Logger.h"

void LOGGER::consolePrint(std::string s)
{
	if (!TESTMODE)std::cerr << s << '\n';
}

// �Է¹��� ���� �α� �޽����� ����ؼ� �α׸� ���
void LOGGER::print(std::string s) 
{
	if (funcName == "")
	{
		consolePrint("�Լ����� �������� �ʽ��ϴ�. �Լ��� �����Ű����");
		return;
	}

	msg = s;
	log_write();
	funcName = "";
}

std::string LOGGER::getLogs()
{
	std::ostringstream oss;
	// ��¥ ���� �����
	oss << "[" << std::put_time(tm_now, "%y.%m.%d %H:%M") << "]";
	// �Լ� ���� �����
	oss << std::setw(30) << std::left << funcName;
	// �޽��� �ֱ�
	oss<<" : " << msg;

	std::string log_data = oss.str();

	consolePrint(log_data);
	return log_data;
}

void LOGGER::log_write()
{
	// ������ �ۼ��ϱ� -> �׽�Ʈ ��尡 �ƴҶ��� �ۼ��Ѵ�
	// �α� ���� ����
	std::fstream logFile("latest.log", std::ios::app);
	std::string log_data;
	// ũ�� ���
	int new_log_size = sizeof(log_data);
	logFile.seekp(0, std::ios::end);
	int size = logFile.tellp();

	// 10kb �ʰ�
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
	// ���� ���丮 ���� Ȯ��
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

	// �ٸ� �α� �̸� ����(.log -> .zip)
	if (otherLog != "latest.log")
	{
		newName = otherLog.substr(0, otherLog.length() - 3) + "zip";
		if (std::rename(fd.name, &newName[0]) != 0) 
			consolePrint("�̸� ���� ����\n");
	}

	// latest �α� �̸� ���� (latest -> util~ )
	std::ostringstream oss;
	oss << "util_" << std::put_time(tm_now, "%y%m%d_%Hh_%Mm_%Ss.log");
	newName = oss.str();

	if(std::rename("latest.log", &newName[0])!=0)
		consolePrint("�̸� ���� ����");

}
