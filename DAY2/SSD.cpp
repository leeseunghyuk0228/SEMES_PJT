#include <iostream>
#include <fstream>
#include <string>
#include <iomanip> 
#include <regex>
#include <vector>
#include <set>

#define WRITE       1
#define READ        2
#define ERASE       3
#define ERASE_RANGE 4
#define FLUSH       5
using namespace std;

void init();
void write(int idx, unsigned int hexValue);
void read(int idx);

int Check_Cmd(string ch);
bool Check_Index(string idx);
bool Check_Value(string val);

void buff_init();
void buff_insert(unsigned int cmd[]);
void buff_flush();
void buff_fastWrite();
void buff_delWrite();
void buff_delErase();

bool buff_canRead(int target);
unsigned int buff_fastRead(int target);

vector<vector<unsigned int>> buf;

int main(int argc, char* args[])
{
    cerr << args[1] << " " << args[2] << " " << args[3] << "\n";
    int COM = Check_Cmd(args[1]);
    int index = stoi(args[2]);
    if (!Check_Index(to_string(index))) return 0;

    unsigned int com[3] = { COM, index, static_cast<unsigned int>(std::stoul(args[3], nullptr, 16)) };

    switch ((int)COM)
    {
    case WRITE:
        if (!Check_Value(args[3])) return 0;
        buff_insert(com);
        break;
    case READ:
        if (buff_canRead(index)) buff_fastRead(index);
        else read(index);
        break;
    case ERASE:
        com[2] = stoi(args[3]);
        buff_insert(com);
        break;
    case ERASE_RANGE:
        com[2] = stoi(args[3]);
        buff_insert(com);
        break;
    case FLUSH:
        buff_flush();
        break;
    default :
        cerr << "INVALID COMMAND\n";
        return 0;
    }
}

void init() {
    unsigned int initValue = 0x00000000;
    ofstream outFile("nand.txt", ios::binary);
    for (int i = 0; i < 100; i++) 
        outFile.write(reinterpret_cast<const char*>(&initValue), sizeof(initValue));
    outFile.close();
}

// txt파일 내용 vector에 넣기, 없으면 파일만들기
void buff_init() {
    fstream file("buffer.txt");

    buf.clear();
    int seq = 0;
    int COM, index;
    while (file >> COM >> index) {
        vector<unsigned int> tempVec;
        tempVec.push_back(COM);
        tempVec.push_back(index);
        switch (COM) 
        {
        case WRITE:
            unsigned int hexValue;
            file >> std::hex >> hexValue >> std::dec;
            tempVec.push_back(hexValue);
        break;
        case ERASE:
            int size; file >> size;
            tempVec.push_back(size);
        break;
        case ERASE_RANGE:
            int endIndex;
            file >> endIndex;
            tempVec.push_back(endIndex);
        break;
        }
        buf.push_back(tempVec); // buf 벡터에 tempVec 추가
    }
    file.close();
    cerr << "버프 사이즈 : " << buf.size() << "\n";
}

void buff_insert(unsigned int com[]) {
    ofstream file("buffer.txt", ios::app); 

    int COM = com[0];
    int index = com[1]; 
    switch (COM) {
    case WRITE:
        file << WRITE << " " << index << " " << std::hex << com[2] << std::dec << std::endl;
        break;
    case ERASE:
    case ERASE_RANGE:
        file << COM << " " << index << " " << com[2] << std::endl;
        break;
    }
    
    buff_init();
    if (buf.size() >= 10) {
        buff_flush();
        file.open("buffer.txt", std::ios::out | std::ios::trunc);
        buf.clear();
        //buff_fastWrite();
        /*if (buf.size() >= 10) {
            file.open("buffer.txt", std::ios::out | std::ios::trunc);
            buff_flush();
            buf.clear();
        }*/
    }
    file.close();
}

void buff_flush() {
    int len = buf.size();
    for (int i = 0; i < len; i++)
    {
        int COM = buf[i][0];
        int index = buf[i][1];

        switch (COM)
        {
        case WRITE:
            write(index, buf[i][2]);
            break;
        case ERASE:
            for (int i = index; i < index + (int)buf[i][2]; i++)
            {
                if (!Check_Index(to_string(i))) break;
                write(i, 0);
            }
            break;
        case ERASE_RANGE:
            for (int i = index; i < (int)buf[i][2]; i++)
            {
                if (!Check_Index(to_string(i))) break;
                write(i, 0);
            }
            break;
        }
    }

}

void buff_fastWrite() {
    ofstream file("buffer.txt", ios::app);
    file.open("buffer.txt", std::ios::out | std::ios::trunc);

    // 1. 같은 LBA에 write 이후 erase 들어오면 write명령어 삭제
    //buff_delWrite();
    // 2. 버퍼 내 연속적인 LBA에 erase오면 erase_range로 교체 
    //buff_delErase();

    /*for (int i = 0; i < buf.size(); i++)
    {
        if(buf[i][0]==WRITE) file << buf[i][0] << " " << buf[i][1] << " " << std::hex << buf[i][2] << std::dec << std::endl;
        else file << buf[i][0] << " " << buf[i][1]<< " " << buf[i][2] << std::endl;
    }*/
    
    file.close();
}

void buff_delWrite() {
    for (auto it = buf.begin(); it != buf.end(); )
    {
        bool erase = false;
        if ((*it)[0] == WRITE) {
            auto nxt = it;
            int lba = (*it)[1];
            while (++nxt != buf.end()) {
                if ((*nxt)[0] == ERASE || (*nxt)[0] == ERASE_RANGE) 
                {
                    int st = (*nxt)[1];
                    int en = st + (*nxt)[2];
                    if (lba < st || lba >= en) continue;
                    it = buf.erase(it);
                    erase = true;
                    break;
                }
            }
        }
        if(!erase) it++;
    }
}

void buff_delErase() {
    
    vector<int> arr(100, -1);
    for (int i = 0 ; i < buf.size(); i++)
    {
        vector<unsigned int>& now = buf[i];
        int st = now[1], en;
        if (now[0] == ERASE)      en = st + now[2];
        else if (now[0] == ERASE) en = now[2];
        else continue;

        for (int j = st; j < en; j++) arr[j] = i;
    }

    set<int, greater<int>> toDel;
    int size = 0;
    for (int i = 0; i <= 100; i++)
    {
        if(i == 100 || arr[i] == -1) {
            if (toDel.size() > 1) {
                for (int el : toDel) buf.erase(buf.begin() + el);
                //erase명령어 넣기
                buf.push_back({ERASE, (unsigned int)i- size, (unsigned int)size });
            }
            toDel.clear();
            size = 0;
        }
        else if (arr[i] != -1) {
            size++;
            if (toDel.find(arr[i]) == toDel.end()) 
                toDel.insert(arr[i]);
        }
    }

}

bool buff_canRead(int target) {
    buff_fastWrite();

    for (int i = 0; i < buf.size(); i++)
    {
        if (buf[i][0] == WRITE && buf[i][1]==target) return true;
        int st = buf[i][1], en = buf[i][2];
        if (buf[i][1] == ERASE) en += buf[i][1];
        if (target >= st && target < en) return true;
    }
    return false;
}

unsigned int buff_fastRead(int target) {
    for (int i = 0; i < buf.size(); i++)
    {
        int lba = buf[i][1];
        if (lba == target) {
            if (buf[i][0] == WRITE) return buf[i][2];
        }
    }
    return 0;
}

void write(int idx, unsigned int hexValue) {
    if (!fstream("nand.txt", ios::binary | ios::in | ios::out)) init();
    fstream file("nand.txt", ios::binary | ios::in | ios::out);
    file.seekp(idx * sizeof(unsigned int), ios::beg);
    file.write(reinterpret_cast<const char*>(&hexValue), sizeof(hexValue));
    file.close();
}

void read(int idx) {
    ifstream inFile("nand.txt", ios::binary);
    inFile.seekg(idx * sizeof(unsigned int), ios::beg);

    unsigned int readData;
    inFile.read(reinterpret_cast<char*>(&readData), sizeof(readData));
    inFile.close();

    ofstream file("result.txt", ios::binary | ios::trunc);
    file.write(reinterpret_cast<const char*>(&readData), sizeof(readData));
    file.close();
}

int Check_Cmd(string str) 
{
    if (str == "W") return WRITE;
    if (str == "R") return READ;
    if (str == "E" || str == "erase") return ERASE;
    if (str == "erase_range") return ERASE_RANGE;
    if (str == "F") return FLUSH;
    return -1; 
}

bool Check_Index(string idx)
{
    int temp = stoi(idx);
    if (temp >= 0 && temp <= 99) return true;
    cerr << "WRONG INDEX [INDEX : 0~99 ]\n";
    return false;
}

bool Check_Value(string val)
{
    regex pattern("^0x[0-9A-F]{8}$");
    if (regex_match(val, pattern))return true;
    cerr << "WRONG VALUE FORMAT [FORMAT 0x[0-9A-F]{8}\n";
    return false;

}
