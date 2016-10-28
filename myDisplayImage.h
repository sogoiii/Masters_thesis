#ifndef _DisplayImgage_
#define _DisplayImgage_

#include "Aria.h"
#include <ArMutex.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <sstream>

//opencv stuff

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "mystructs.h"
#include <boost/thread.hpp> 
using namespace std;

#define INVALIDCELL 100
#define OBSTACLECELL  30
#define PI 3.14



class myDisplayImage  :public ArRecurrentTask
{


public:

	//void* runThread(void*) {return NULL;}
	void task();
	//functions
	myDisplayImage(); //constructor
	~myDisplayImage(); //Destructor
	void map_parse(string location);
	void setobstaclepoints(int minx, int miny, int maxx, int maxy, int tlx, int bry);// sets cell as obstacle
	//void addGridMap(GridMap *myGridMap);
	void print();
	void DrawFieldOfView(int robot_X,int robot_Y, double robot_theta);
	void RemoveFieldOfView(int robot_X,int robot_Y, double robot_theta);
	void FieldofViewPixels(int x_size, int y_size);
	void DrawThisLocationGREEN(int x, int y);
	void DrawThisLocationBLUE(int x, int y);
	void DrawThisLocationRED(int x, int y);
	void DEBUG_drawthesepoints(int x, int y);
	void UpdateRobotFOVonDisplay(int x, int y, double theta);
	void AddArRobot(ArRobot *robot);
	void FOVDipslayThread();

	//void FOV(int x, int y, double theta);
	void StartRecurrentTask();
	void StopRecurrentTask();

	deque<FovStruct> ReturnFovLocationsSeen();
	deque<deque<double>> ReturnLocationFovList();

	//member objects
	IplImage *image; //only using the blu channel atm; limits value form 0-255
	int resolution;
	int offsetX;
	int offsetY;
	bool DrawFovFirst;

	//these can be private
	int Blx;
	int Bly;
	int Trx;
	int Try;
	bool ContinueThread; 

	int XLimit;
	int YLimit;

	int ReturnXLimit();
	int ReturnYLimit();

	
	
private:
	//member functions
	void floodfill(CvScalar color, int x,int y, int& tcount);
    void removefloodfill(CvScalar color, int x,int y, vector<vector<int>>& fovlist, int& count);

	//member objects
	ArMutex datalock;
	ArRobot *myrobot;
	int scalarOut; //not sure what 5 feet is but need to figure it out
	int coneAngle; //degrees
	
	
	//IplImage *image; //only using the blu channel atm; limits value form 0-255
	

	int boundary;
	int myrow;
	int mycol;
	Cartesian leg1,leg2,curPos;
	vector<vector<int>> tempv;
	vector<vector<int>> PixelFovList; //list of pixels that are in the fov range
	deque<deque<double>> ValueFovList; // contains the location of each grid seen in fov
	deque<deque<double>> DoubleFovList; // used in mergesort as a copy
	vector<int> SingleFovList;
	deque<deque<double>>LocationFovList; // locations that are seen by the fov
	vector<int> IndexLocs;   //result of index locations in fov
	vector<double> IndexPercents; // percentage of each index in fov
	deque<FovStruct> FovLocationsSeen;
	deque<deque<double>> mymerge(deque<deque<double>> &list1, deque<deque<double>> &list2);
	deque<deque<double>> merge_split(deque<deque<double>> list);
	void merge_sort(deque<deque<double>> &list);

	double myLog(double number, int base);
	ofstream myfile;
	string directory;

	//ArFunctor3C<myDisplayImage,int, int,double> FOVCB;
	//ArFunctorC<myDisplayImage> FOVCB;
	

};

#endif
