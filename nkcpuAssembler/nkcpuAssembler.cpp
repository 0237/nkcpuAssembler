#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <map>

using namespace std;

class Code {
public:
	bool mem[32];
};

class Data {
public:
	bool mem[16];
};

class Address {
public:
	bool mem[16];
};

//变量名、段名与地址的映射关系
map<string, Address> umap;
map<string, int> rmap;
string register_name[32] = { "zero","at","v0","v1","a0" ,"a1" ,"a2" ,"a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","s8","ra" };

bool* num2bin5(int num) {
	bool* bin = new bool[5];
	for (int i = 4; i >= 0; i--)
	{
		if (num & (1 << i))
			bin[4-i] = 1;
		else
			bin[4-i] = 0;
	}
	return bin;
}

bool* num2bin(int num) {
	bool* bin = new bool[16];
	for (int i = 15; i >= 0; i--)
	{
		if (num & (1 << i))
			bin[15-i] = 1;
		else
			bin[15-i] = 0;
	}
	return bin;
}

bool* op2bin(char* op) {
	char* pNext;
	char* opcode = strtok_s(op, " $,()+", &pNext);	string sop = opcode;
	char* operand0 = strtok_s(NULL, " $,()+", &pNext);
	char* operand1 = strtok_s(NULL, " $,()+", &pNext);
	char* operand2 = strtok_s(NULL, " $,()+", &pNext);
	bool* bin=new bool[32];
	memset(bin, 0, 32);
	if (sop == "add") {//add $s7,$zero,$zero
		bin[26] = 1;
		int d = rmap[operand0];
		bool* binum = num2bin5(d);
		for (int i = 16;i < 21;i++) {
			bin[i] = binum[i - 16];
		}
		int s = rmap[operand1];
		binum = num2bin5(s);
		for (int i = 6;i < 11;i++) {
			bin[i] = binum[i - 6];
		}
		int t = rmap[operand2];
		binum = num2bin5(t);
		for (int i = 11;i < 16;i++) {
			bin[i] = binum[i - 11];
		}
	}
	else if (sop == "addi") {//addi $s6,$s7,1
		bin[2] = 1;
		int t = rmap[operand0];
		bool* binum = num2bin5(t);
		for (int i = 11;i < 16;i++) {
			bin[i] = binum[i - 11];
		}
		int s = rmap[operand1];
		binum = num2bin5(s);
		for (int i = 6;i < 11;i++) {
			bin[i] = binum[i - 6];
		}
		int imm = atoi(operand2);
		binum = num2bin(imm);
		for (int i = 16;i < 32;i++) {
			bin[i] = binum[i - 16];
		}
	}
	else if (sop == "la") {//la $v2,value
		bin[0] = 1;
		int t = rmap[operand0];
		bool* binum = num2bin5(t);
		for (int i = 11;i < 16;i++) {
			bin[i] = binum[i - 11];
		}
		for (int i = 16;i < 32;i++) {
			bin[i] = umap[operand1].mem[i - 16];
		}
	}
	else if (sop == "lw") {//lw $a0,($v2+$s7)类似add
		bin[0] = bin[4] = bin[5] = 1;
		int d = rmap[operand0];
		bool* binum = num2bin5(d);
		for (int i = 16;i < 21;i++) {
			bin[i] = binum[i - 16];
		}
		int s = rmap[operand1];
		binum = num2bin5(s);
		for (int i = 6;i < 11;i++) {
			bin[i] = binum[i - 6];
		}
		int t = rmap[operand2];
		binum = num2bin5(t);
		for (int i = 11;i < 16;i++) {
			bin[i] = binum[i - 11];
		}
	}
	else if (sop == "sw") {//sw $v0,($v2+$s6)类似lw
		bin[0] = bin[2] = bin[4] = bin[5] = 1;
		int d = rmap[operand0];
		bool* binum = num2bin5(d);
		for (int i = 16;i < 21;i++) {
			bin[i] = binum[i - 16];
		}
		int s = rmap[operand1];
		binum = num2bin5(s);
		for (int i = 6;i < 11;i++) {
			bin[i] = binum[i - 6];
		}
		int t = rmap[operand2];
		binum = num2bin5(t);
		for (int i = 11;i < 16;i++) {
			bin[i] = binum[i - 11];
		}
	}
	else if (sop == "blez") {//blez $v1,p6
		bin[3] = bin[4] = 1;
		int s = rmap[operand0];
		bool* binum = num2bin5(s);
		for (int i = 6;i < 11;i++) {
			bin[i] = binum[i - 6];
		}
		for (int i = 16;i < 32;i++) {
			bin[i] = umap[operand1].mem[i - 16];
		}
	}
	else if (sop == "slt") {//slt $t0,$v0,$v1类似add
		bin[26] = bin[28] = bin[30] = 1;
		int d = rmap[operand0];
		bool* binum = num2bin5(d);
		for (int i = 16;i < 21;i++) {
			bin[i] = binum[i - 16];
		}
		int s = rmap[operand1];
		binum = num2bin5(s);
		for (int i = 6;i < 11;i++) {
			bin[i] = binum[i - 6];
		}
		int t = rmap[operand2];
		binum = num2bin5(t);
		for (int i = 11;i < 16;i++) {
			bin[i] = binum[i - 11];
		}
	}
	else if (sop == "j") {//j p2
		bin[4] = 1;
		for (int i = 16;i < 32;i++) {
			bin[i] = umap[operand0].mem[i - 16];
		}
	}
	else if (sop == "li") {//li $v0,1
		bin[0] = bin[1] = bin[2] = bin[3] = 1;
		int t = rmap[operand0];
		bool* binum = num2bin5(t);
		for (int i = 11;i < 16;i++) {
			bin[i] = binum[i - 11];
		}
		int imm = atoi(operand1);
		binum = num2bin(imm);
		for (int i = 16;i < 32;i++) {
			bin[i] = binum[i - 16];
		}
	}
	else if (sop == "syscall") {//syscall
		bin[29] = bin[28] = 1;
	}
	else {
		for (int i = 0;i < 32;i++) {
			bin[i] = 1;
		}
	}
	return bin;
}

int main() {
	Code cbinary;
	Data dbinary;
	Address abinary;
	queue<Code> textq;
	queue<Data> dataq;

	for (int i = 0; i < 32; i++) {
		rmap[register_name[i]] = i;
	}
	ifstream fin;
	fin.open("sort1.asm", ios_base::in);
	if (!fin) {
		cerr << "Open Error!" << endl;
		exit(1);
	}
	char buffer[256];
	char* op;
	char* tokenPtr;
	char* pNext;
	int add = 0;
	int seg = 0;
	//先遍历一遍确定变量和代码块位置
	while (!fin.eof()) {
		fin.getline(buffer, 256);
		if (buffer[0] != 0) {
			tokenPtr = strtok_s(buffer, "#", &pNext);
			if (tokenPtr[0] != 0) {
				char* pchd = strstr(tokenPtr, ".data");
				char* pcht = strstr(tokenPtr, ".text");
				if (pchd != NULL) {
					seg = 1;
					add = 0;
					continue;
				}
				if (pcht != NULL) {
					seg = 2;
					add = 0;
					continue;
				}
				if (seg == 1) {
					pchd = strstr(tokenPtr, ":");
					if (pchd != NULL) {
						tokenPtr = strtok_s(tokenPtr, ":", &pNext);
						bool* binum = num2bin(add);
						memcpy(abinary.mem, binum, 16 * sizeof(bool));
						umap[string(tokenPtr)] = abinary;
						tokenPtr = strtok_s(NULL, " .word,", &pNext);
						while (tokenPtr != NULL) {
							binum = num2bin(atoi(tokenPtr));
							memcpy(dbinary.mem, binum, 16 * sizeof(bool));
							dataq.push(dbinary);
							add+=2;//数据为两个八位二进制组合成的16位字
							tokenPtr = strtok_s(NULL, " .word,", &pNext);
						}
					}
				}
				else if (seg == 2) {
					pcht = strstr(tokenPtr, ":");
					if (pcht != NULL) {
						tokenPtr = strtok_s(tokenPtr, ":", &pNext);
						bool* binum = num2bin(add);
						memcpy(abinary.mem, binum, 16 * sizeof(bool));
						umap[string(tokenPtr)] = abinary;
					}
					add+=4;//指令为四个八位二进制字组成的32位字
				}
			}
		}
	}
	fin.seekg(0, ios::beg);
	seg = 0;
	//再遍历一遍翻译.text为汇编码
	while (!fin.eof()) {
		fin.getline(buffer, 256);
		if (buffer[0] != 0) {
			op = strtok_s(buffer, "#", &pNext);
			if (op[0] != 0) {
				char* pchd = strstr(op, ".data");
				char* pcht = strstr(op, ".text");
				if (pchd != NULL) {
					seg = 1;
					continue;
				}
				if (pcht != NULL) {
					seg = 2;
					continue;
				}
				if (seg == 2) {
					pcht = strstr(op, ":");
					if (pcht != NULL) {
						op = strtok_s(op, ":", &pNext);
						op = strtok_s(NULL, ":", &pNext);
						if (op != NULL)pcht = strstr(op, ":");
					}
					if (pcht == NULL) {
						bool* newline = op2bin(op);
						memcpy(cbinary.mem, newline, 32 * sizeof(bool));
						textq.push(cbinary);
					}
				}
			}
		}
	}
	ofstream tout,dout;
	//输出代码段
	tout.open("mcode1.txt", ios::out | ios::trunc);
	if (tout.is_open()) {
		while (!textq.empty()) {
			for (int i = 0; i < 32; i++) {
				tout << textq.front().mem[i];
				if ((i + 1) % 8 == 0 && i != 31) {
					tout << " ";
				}
			}
			tout << endl;
			textq.pop();
		}
		cout << ".text compile success!" << endl;
		tout.close();
	}
	//输出数据段
	dout.open("mdata.txt", ios::out | ios::trunc);
	if (dout.is_open()) {
		while (!dataq.empty()) {
			for (int i = 0; i < 16; i++) {
				dout << dataq.front().mem[i];
				if ((i + 1) % 8 == 0 && i != 15) {
					dout << " ";
				}
			}
			dout << endl;
			dataq.pop();
		}
		cout << ".data compile success!" << endl;
		dout.close();
	}
	return 0;
}