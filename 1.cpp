
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

bool Compare(const std::string& p_A_filename, const std::string& p_B_filename);
uint32_t MyChkSum(const char* buff, size_t len, uint32_t prevchk);


int main(int argc, char *argv[])
{
	
}

bool Compare(const std::string& p_A_filename, const std::string& p_B_filename)
{
	const char pathA[] = "./A.txt";
	const char pathB[] = "./B.txt";
	const uint16_t fragmentSize = 1024;

	ostringstream ossA;
	string strA;
	ifstream ifs(pathA);
	// int sizeB;
	int32_t sizeA;
	char *arrB;
	uint32_t prevchkA = 0, prevchkB = 0;
	uint16_t fragmentCnt = 0;
	bool success = true;

	// Read A file
	ossA << ifs.rdbuf();
	ifs.close();

	strA = ossA.str();
	sizeA = strA.size();

	printf("A: %s size:%d\n", strA.c_str(), sizeA);

	// Read B file
	ifs.open(pathB);

	// ifs.seekg(0, fstream::end);
	// sizeB = ifs.tellg();

	ifs.seekg(0, fstream::beg);

	arrB = new char[sizeA + 1];

	if(ifs.read(arrB, sizeA).eof())
	{
		printf("hiba\n");

		// A file is bigger
		ifs.close();
		return false;
	}
	else
	{
		printf("ok\n");
	}

	ifs.close();

	while(sizeA > 0 && success)
	{
		prevchkA = MyChkSum(strA.substr(fragmentCnt * 1024, min(1024, sizeA)).c_str(), min(1024, sizeA), prevchkA);
		prevchkB = MyChkSum(&arrB[fragmentCnt * 1024], min(1024, sizeA), prevchkB);

		if(prevchkA != prevchkB)
		{
			success = false;
		}

		++fragmentCnt;
		sizeA -= fragmentSize;
	}
	
	delete[] arrB;

	return success;
}

uint32_t MyChkSum(const char* buff, size_t len, uint32_t prevchk)
{
	// Mock
	return 1;
}