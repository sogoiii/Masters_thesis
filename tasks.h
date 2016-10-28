#ifndef _tasks_
#define _tasks_

#include "Aria.h"
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using std::string;
using std::vector;


struct ConfirmLoc{
	int xloc;
	int yloc;
	double bid;
	string robot;
};

class tasks{
public:
	tasks(); //constructor

	int x_global;
	int y_global;
	int x_index;
	int y_index;
	int x_pixel;
	int y_pixel;
	double bid;
	double cost;
	double pathcost;
	double pathvalue;
	double dist;
	double utility;
	//char robotcreator;
	string robotcreator;
	string robotbidder;
	string robotSimilarLocation; // the robot who has a location similar to mine
	time_t createdtime;
	string robotOwner; // not used????
	string CenterLocationOwner;

	struct tm *createdtime_tm;
	int time_sec;
	int time_min;
	int time_hour;

	int taskConfirmFlag;
	int taskSentConfirm;
	//void update_time(time_t* tvar); //old function i believe 
	void updatecreationtime(struct tm *update_time_tm_variable);
	void UpdateCreateTime();
	int ReturnCreatedTimeMiliseconds();
	ArTime ReturnArTime();

	int x_index_Compete;
	int y_index_Compete;
	int x_global_Compete;
	int y_global_Compete;

	int x_global_Center;
	int y_global_Center;
	int x_index_Center;
	int y_index_Center;
	
	double myDistanceToCenterLocation;
	double otherDistanceToCenterLocation;
private:
	ArTime CreatedTime;


};

#endif