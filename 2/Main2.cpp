
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <set>


class Point
{
private:
	float X, Y; 

public:
	Point(float x, float y, std::string id): X{x}, Y{y}, Id{id} {}
	~Point();

	std::string Id;

	float GetDistanceFromOrigo(void) const { return sqrtf(powf(X, 2) + powf(Y, 2)); }
};

Point::~Point(){}

float CalculateBiggestRadius(const std::vector<Point>& points);

int main(int argc, char *argv[])
{
	// Stored points, with test datas
	// Pythagorean triples
	std::vector<Point> Points = {
		{3.0f, 4.0f, "A"},
		{6.0f, 8.0f, "A"},
		{3.0f, 4.0f, "B"},
		{5.0f, 12.0f, "B"},
		{6.0f, 8.0f, "C"},
		{5.0f, 12.0f, "C"},
	};

	std::cout << "Max radius " << CalculateBiggestRadius(Points) << std::endl;
	
	return EXIT_SUCCESS;
}

float CalculateBiggestRadius(const std::vector<Point>& points)
{
	std::multimap<float, std::string> PointMap;
	std::set<std::string> existingIds;

	// Points in ascending order of distance
	for (auto &&i : points)
		PointMap.insert(std::pair<float, std::string>(i.GetDistanceFromOrigo(), i.Id));
	
	for (auto &&i : PointMap)
	{
		if(existingIds.count(i.second))
			return i.first; 
		else
			existingIds.insert(i.second);
	}

	// Each ID is unique
	return PointMap.rend()->first;
}
