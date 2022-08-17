
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <set>

#include <iostream>

using std::string;
using std::vector;
using std::multimap;
using std::set;

class Point
{
private:
	float x, y; 

public:
	Point(float x, float y): x{x}, y{y} {}
	~Point();

	string Id;

	float GetDistanceFromOrigo(void) const { return sqrtf(powf(x, 2) + powf(y, 2)); }
};

Point::~Point(){}
/*
class PointType
{
private:
	string Id;
	vector<float> MinDistances {0.0f};

public:
	PointType();
	~PointType();

	void AddNewPoint(Point point);
};

PointType::PointType() {}
PointType::~PointType() {}

void PointType::AddNewPoint(Point point)
{
	float dist = point.GetDistanceFromOrigo();

	if(MinDistances.empty())
		MinDistances.push_back(dist);

	else if(MinDistances.size() < 2)
	{
		if(dist < MinDistances[0])
			MinDistances.
	}


	if(MinDistances[0] == 0.0f)
		MinDistances[0] = dist;

	else if(MinDistances[1] == 0.0f)
		MinDistances[1] = dist;
		
	else if(dist > MinDistances[0] && dist < MinDistances[1])
		MinDistances[1] = dist;
	
	else if(dist <= MinDistances[0])
	{	
		MinDistances[1] = MinDistances[0];
		MinDistances[0] = dist;
	}
}
*/



int main(int argc, char *argv[])
{
	// Stored points
	vector<Point> Points;
	multimap<float, string> PointMap;
	set<string> existingIds;

	// Points in ascending order of distance
	for (auto &&i : Points)
	{
		PointMap.insert( std::pair<float, string>(i.GetDistanceFromOrigo(), i.Id));
	}
	
	for (auto &&i : PointMap)
	{
		if(existingIds.count(i.second))
		{
			std::cout << "Max radius " << i.first << std::endl;
			return; 
		}
		else
		{
			existingIds.insert(i.second);
		}
	}
	
}