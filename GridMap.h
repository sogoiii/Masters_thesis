#ifndef _GridMap_
#define _GridMap_

#include "myDisplayImage.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "mapvar.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <boost/thread.hpp> 

using namespace std;


class GridMap: public ArRecurrentTask
{
public:
	void task();
	GridMap(myDisplayImage *DisplayImage);
	~GridMap();
	void CreateGridMap(int x_length, int y_length);
	int CompareValue(int x, int y , int cvalue);
	int ReturnYPixel(int x, int y);
	int ReturnXPixel(int x, int y);
	int ReturnXglobal(int x, int y);
	int ReturnYglobal(int x, int y);

	int GetValue(int x, int y);
	
	bool CheckBidVectorSize(int x, int y);
	int ReturnBidVectorSize(int x, int y);

	void AddToBidVector(int x, int y, int bidvalue);
	int ReturnBidVectorBid(int x, int y, int n);
	void RemoveBidFromBidVector(int x, int y, int n);

	void AddToBidVectorBidder(int x, int y, string bidder);
	string ReturnBidVectorBidder(int x, int y, int n);
	void RemoveBidVectorBidder(int x, int y, int n);
	void BeginUpdateMapValues(); // simply starts my updateGridMapValues thread


	bool ReturnIsObstacle(int x, int y);
	vector<vector<int>> Get8neighbors(int x, int y);

	//member objects
	vector<vector<mapvar>> MyGridMap;

	void DEBUG_PrintDisplayMap();
	void UpdateSeenFOVIndex(deque<FovStruct> fovlocs);

	void StartRecurrentTask();
	void StopRecurrentTask();

	void ChooseMinValue(int x, int y, int value);

	void UpdateTotalLocationsSeenList(deque<FovStruct> seen);
	deque<FovStruct> TotSeen;
	bool totseenfirstbool;

	int ReduceValueParam;

	void WrappedFovDisplay();

private:
	//functions
	void ComputeGridObstacles();
	void UpdateGridMapValues();

	bool ContinueThread;
	//member objects
	ofstream myfile;
	string directory;

	ofstream myfile2;
	string directory2;

	myDisplayImage *GridDisplayImage;
	int X_limit;
	int Y_limit;


};
#endif