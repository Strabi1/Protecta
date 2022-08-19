
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>

using std::string;
using std::cout;
using std::endl;
using std::min;
using std::ifstream;
using std::ostringstream;
using std::fstream;

//================================================
// Feladat 1
//================================================
bool Compare(const std::string& p_A_filename, const std::string& p_B_filename);
uint32_t MyChkSum(const char* buff, size_t len, uint32_t prevchk);


int main(int argc, char *argv[])
{
	const string pathA = "./A.txt";
	const string pathB = "./B.txt";

	if(Compare(pathA, pathB))
		cout << "B starts with the same bytes as the whole A file" << endl;
	else
		cout << "B starts with the other bytes as the whole A file" << endl;
}

bool Compare(const std::string& p_A_filename, const std::string& p_B_filename)
{
	
	const uint16_t FRANGMENT_SIZE = 1024;

	ostringstream ossA;
	string strA;
	ifstream ifs(p_A_filename);
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

	// Read B file
	ifs.open(p_B_filename);

	ifs.seekg(0, fstream::beg);

	arrB = new char[sizeA + 1];

	if(ifs.read(arrB, sizeA).eof())
	{
		// A file is bigger
		ifs.close();
		return false;
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
		sizeA -= FRANGMENT_SIZE;
	}
	
	delete[] arrB;

	return success;
}

// It just needs to compile
uint32_t MyChkSum(const char* buff, size_t len, uint32_t prevchk)
{
	// Mock
	return 1;
}