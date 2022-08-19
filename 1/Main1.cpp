
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


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
	std::ifstream ifs(p_A_filename);
	std::ostringstream ossA;
	std::string strA;

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

	ifs.seekg(0, std::fstream::beg);

	arrB = new char[sizeA + 1];

	if(ifs.read(arrB, sizeA).eof())
	{
		// A file is bigger
		ifs.close();
		delete[] arrB;
		return false;
	}

	ifs.close();

	while(sizeA > 0 && success)
	{
		prevchkA = MyChkSum(strA.substr(fragmentCnt * 1024, std::min(1024, sizeA)).c_str(), std::min(1024, sizeA), prevchkA);
		prevchkB = MyChkSum(&arrB[fragmentCnt * 1024], std::min(1024, sizeA), prevchkB);

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