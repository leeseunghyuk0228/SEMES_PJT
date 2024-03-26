#include <iostream>
#include <fstream>
#include <string>
#include <iomanip> 
#include <regex>

void init();
void write(int idx, unsigned int hexValue);
void read(int idx);
bool Check_Index(std::string idx);
bool Check_Value(std::string val);

int main(int argc, char* args[])
{
    char command = *args[1];
    int index = *args[2] - '0';
    if (!Check_Index(std::to_string(index))) return 0;

    if (command == 'W') {
        std::string val = args[3];
        if (!Check_Value(val)) return 0;
        unsigned int value = static_cast<unsigned int>(std::stoul(val, nullptr, 16));
        write(index, value);
    }
    else if (command == 'R') {
        read(index);
    }
    else
    {
        std::cerr << "INVALID COMMAND\n";
        return 0;
    }

}

/*
* 데이터 전체 삭제,
* 0x00000000 으로 100크기만큼 초기화
*/
void init() {
    unsigned int initValue = 0x00000000;
    std::ofstream outFile("nand.txt", std::ios::binary);
    for (int i = 0; i < 100; i++) {
        outFile.write(reinterpret_cast<const char*>(&initValue), sizeof(initValue));
    }
    outFile.close();
}

void write(int idx, unsigned int hexValue) {

    std::fstream file("nand.txt", std::ios::binary | std::ios::in | std::ios::out);
    if (!file) init();
    file = std::fstream("nand.txt", std::ios::binary | std::ios::in | std::ios::out);

    // 파일 포인터를 idx 위치로 이동
    file.seekp(idx * sizeof(unsigned int), std::ios::beg);
    file.write(reinterpret_cast<const char*>(&hexValue), sizeof(hexValue));
    if (!file) std::cerr << "파일 쓰기 오류." << std::endl;
    file.close();
}

void read(int idx) {

    std::ifstream inFile("nand.txt", std::ios::binary);
    inFile.seekg(idx * sizeof(unsigned int), std::ios::beg);

    unsigned int readData;
    inFile.read(reinterpret_cast<char*>(&readData), sizeof(readData));
    inFile.close();

    std::ofstream file("result.txt", std::ios::binary | std::ios::trunc);
    file.write(reinterpret_cast<const char*>(&readData), sizeof(readData));
    file.close();
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
    std::regex pattern("^0x[0-9A-F]{8}$");
    if (std::regex_match(val, pattern))return true;
    std::cerr << "WRONG VALUE FORMAT [FORMAT 0x[0-9A-F]{8}\n";
    return false;

}
