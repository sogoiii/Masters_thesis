#ifndef _mystructs_
#define _mystructs_

#include <time.h>
#include <iostream>
#include <fstream>
#include <string>

using std::string;

struct Cartesian{
	int x;
	int y;
	double angle;
};

struct FovStruct{// used in mydisplay and grid map
	int x;
	int y;
	double percentage;
};

struct SecondPathStruct{
	int x1;
	int y1;
	int x2;
	int x3;
	string robotcreator;
	string robotSimilarLocation;

};



struct Task{ // i do not think i use this anymore....
	int x_global;
	int y_global;
	int x_index;
	int y_index;
	int x_pixel;
	int y_pixel;
	double bid;
	double cost;
	//char robotcreator;
	string robotcreator;
	time_t createdtime;
	struct tm createdtime_tm;
};

#endif