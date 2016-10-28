
#ifndef _CreateTasks_
#define _CreateTasks_

#include "Aria.h"
#include "GridMap.h"
#include "myDisplayImage.h"
#include "mapvar.h"
#include <vector>
#include <cmath>
#include <time.h>
#include "mystructs.h"
#include "tasks.h"
//opencv stuff
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#define PI 3.14


class CreateTasks{

public:
	CreateTasks(ArRobot *robot, GridMap *map, myDisplayImage *Display);
	CreateTasks(ArRobot *robot);
	void AddGridMap(GridMap *map);
	void AddDisplayImage(myDisplayImage *Display);
	void CreateSelfTasks(int x_loc, int y_loc);
	void ChoosePossibleLocationList(vector<vector<mapvar>>, int x_size, int y_size, int n);
	void AddSelfRobotName(string name);
	void DEBUG_CreateSpecificTask(string name, int x, int y);

	string selfrobot;
	vector<tasks> getSelfCretedTasks();
	vector<tasks> mySelfTasks; // if i make protect, need to create function

	vector<tasks> DEBUG_getmySelfTasks();
	vector<tasks> DEBUG_mySelfTasks;
	void DEBUG_printSelfCreatedTasks();
	vector<tasks> GrabMax(vector<tasks>, int); // grab max n values in input vector

protected:
	//member functions
	

	//member objects
	
	ArRobot *myRobot;
	GridMap *CTGridMap;
	myDisplayImage *DisplayImage;
	double BoundRadius;
	double resolution;
	Cartesian curPos;
	vector<vector<int>> circle_list; //temporary untill replaced with CircleList
	vector<tasks> circletask;

};
#endif 