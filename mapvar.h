
#ifndef __MAPVAR_H__
#define __MAPVAR_H__

//OCt 22 2010
#include <time.h>
#include <iostream>
#include <fstream>
#include <cv.h>
#include <vector>
#include <cxcore.h>
#include <highgui.h>
#include <string>

#include <boost/thread.hpp>

using std::string;
using std::vector;
class mapvar{
public:
	//functions
	mapvar();
	~mapvar();
	void update_time(time_t *update_time_variable);
	void update_time(struct tm *update_time_tm_variable);
	int grab_value();
	
	//variables
	vector < int > received_bids;

	string task_creation_string;
	int x_global; // center of the grid in global position
	int y_global; // center of the grid in global position
	int x_index;
	int y_index;
	int x_pixel; // center of the grid pixel 
	int y_pixel; // center of the grid pixel
	double bid;
	vector<int> bidvector; // vector of all the bid for a location
	vector<string> bidvectorbidder; // vector of all the robots that placed a bid
	string robotorigin;
	time_t task_creation_time; // printc error if i print something after this line.
	struct tm * task_creation_time_tm;
	int time_sec;
	int time_min;
	int time_hour;


	int value;
	double distance;
	double cost;
	double utility;
	time_t last_update_time;
	//string Agent_update;
	//time current_time;
	int visibility_count;
	double visibility;
	//int last_time;
	int obstacle;
	double obstacle_percentage;
	
	bool isObstacle;
private:
	void UpdateValue();


};




#endif