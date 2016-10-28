#ifndef _EvaluateTask_
#define _EvaluateTask_

#include "Aria.h"
#include "Arnl.h"
#include "tasks.h"
#include "CreateTasks.h"
#include "GridMap.h"

//#include "ArSonarLocalizationTask.h" why is this needed?
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>


class EvaluateTask{

public:
	EvaluateTask(ArRobot *robot, CreateTasks *Task, GridMap *map);
	EvaluateTask(ArRobot *robot);
	void AddCreateTasks(CreateTasks *Task);
	void AddGridmap(GridMap *map);
	void AddPathTask(ArPathPlanningTask *newpathTask);
	void updateTaskListVector(vector <tasks> newtasks);
	void updateTaskList(tasks newtask);
	void chooseMyBid(vector<vector<mapvar>> map);
	double PathCost();
	vector<tasks> computeMyBid();
	vector<tasks> getTaskList();
	bool SecondPathPlanCheck();
	void AddSelfRobotName(string name);
	vector<tasks> GrabSimilarLocations();
	

	vector<double> Debug_CostCompute();
	void SignalExchangeValuesCondition();
	void WaitExchangeValuesCondition();
	void SignalDistanceValueCondition();
	void WaitDistanceValueCondition();
	void PushCurrentTaskList();

	double CenterLocationComputation();
	void PutOtherRobotsDistance(double distance);
	void WhoOwnConflictLocation();
	vector<tasks> FillCostsAndInformation(vector<tasks> alist);
	
	void DEBUG_ClearSimilarLocations();

	vector<tasks> ReturnMyOwnBids();
	vector<tasks> GrabMaxBid(vector<tasks> list, int n);

private:
	//Member Functions
	vector<tasks> GrabMax(vector<tasks> list , int n);
	vector<tasks> DistanceFilter(vector<tasks> list);
	double ComputeDistance(vector<tasks> list, int i,vector<vector<mapvar>> map);
	double ComputeDistance(double x1, double y1, double x2, double y2);
	vector<tasks> ComputeDistanceVector(vector<tasks> list, vector<vector<mapvar>> map);
	double ComputePathCost(int x_start, int y_start, int xend, int yend);
	vector<tasks> ComputePathCostVector(vector<tasks> alist, int xstart, int ystart);
	vector<tasks> ComputeUtility(vector<tasks> alist, vector<vector<mapvar>> map);
	vector<tasks> ComputeGlobalPositions(vector<tasks> alist, vector<vector<mapvar>> map);
	vector<tasks> ComputeTimeCost(vector<tasks> alist);
	vector<tasks> CompareVectorDistances(vector<tasks> plist, vector<tasks> qlist, vector<tasks> result);
	int GrabMaxLocationsInVector(vector<tasks> alist);
	vector<tasks> SumCosts(vector<tasks> alist);

	vector<tasks> ComputePathValue(vector<tasks> alist, vector<vector<mapvar>> amap);
	

	//Member objects
	ofstream myfile;
	string directory;
	double max_distance;
	bool deleteflag;
	string robotname;
	vector<tasks> TaskList;

	vector<tasks> myOwnBids;
	vector<tasks> CurrentTaskList;
	vector<tasks> SimilarLocations;

	GridMap *evalGridMap;
	ArRobot *myRobot;
	CreateTasks *createTask;
	ArPathPlanningTask *pathTask;

	ArCondition ExchangeValuesCondition; 
	ArCondition ReceivedDistanceValueCondition;

};

#endif