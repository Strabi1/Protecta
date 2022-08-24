
#include <iostream>
#include <fstream>
#include <string>

const uint16_t FRANGMENT_SIZE = 1024;

bool Compare(const std::string& p_A_filename, const std::string& p_B_filename);
uint32_t MyChkSum(const char* buff, size_t len, uint32_t prevchk);


int main(int argc, char *argv[])
{
	const std::string pathA = "./A.txt";
	const std::string pathB = "./B.txt";

	if(Compare(pathA, pathB))
		std::cout << "B starts with the same bytes as the whole A file" << std::endl;
	else
		std::cout << "B starts with the other bytes as the whole A file" << std::endl;

	return EXIT_SUCCESS;
}

bool Compare(const std::string& p_A_filename, const std::string& p_B_filename)
{
	char arrA[FRANGMENT_SIZE + 1], arrB[FRANGMENT_SIZE + 1];
	uint16_t readA, readB;
	uint32_t prevchkA = 0, prevchkB = 0;
	bool success = true;
	std::ifstream ifsA(p_A_filename);
	std::ifstream ifsB(p_B_filename);

	ifsA.seekg(0, std::fstream::beg);
	ifsB.seekg(0, std::fstream::beg);	

	do
	{
		ifsA.read(arrA, FRANGMENT_SIZE).eof();
		readA = ifsA.gcount();
		arrA[readA] = '\0';

		ifsB.read(arrB, readA);
		readB = ifsB.gcount();
		arrB[readB] = '\0';

		prevchkA = MyChkSum(arrA, readA, prevchkA);
		prevchkB = MyChkSum(arrB, readB, prevchkB);

		if(prevchkA != prevchkB)
			success = false;

	} while(readA && success);

	ifsA.close();
	ifsB.close();

	return success;
}

// It just needs to compile
uint32_t MyChkSum(const char* buff, size_t len, uint32_t prevchk)
{
	// Mock
	return 1;
}