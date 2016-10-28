#ifndef _Auction2_
#define _Auction2_

#include "Aria.h"
#include "tasks.h"
#include "GridMap.h"
#include "mySync.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <boost/thread.hpp>

class Auction {
	ArMutex myAuctionMutex;
public:

	Auction(ArRobot *robot);
	void updateBidList(tasks newbid); //add bid to our list
	void ReviewBidList(); // move to private ;
	void AddGridMap(GridMap *grid);
	vector<tasks> BidList;  // vector of all the bids recieved. 
	void SendConfirmConditionWait(int robotID); 
	void SendConfirmConditionSignal(int robotID);
	ConfirmLoc ReturnConfirmLoc(int robotID);

	bool ReturnReadyStatus(int robotID);//individual flags 
	void ChangeReadyStatus(int robotID);//individual flags

	void StatusReadyGoWait(); // this is the arcondition that haults the thread that calls this function
	void StatusReadyGoSignal(); // this function will signal the above thread to continue

	void AreWeReady(int numberofrobots, string robot1, string robot2, string robot3); //checkds if all robots are ready

	int ReturnRobotIDFromString(string robotstring);
protected:
	//Member functions
	void ChooseWinner(int n);
	void RemoveOldBids(ConfirmLoc theLoc, int index);
	
	unsigned int TimeBetweenConfirms;

	//Member Objects
	ArFunctorC<Auction> myReviewBidsCB;
	ArRobot *myRobot;
	GridMap *myGrid;
	int checkbidcount;
	bool SendConfirmBool;
	ArTime patrolConfirmTime;
	ArTime peopleConfirmTime;
	ArTime p3atConfirmTime;
	vector<bool> ConfirmSendVector; //vector
	ArCondition SendConfirmCondition;
	ArCondition PeopleCondition;
	ArCondition PatrolCondition;
	ArCondition P3atCondition;
	ConfirmLoc ConfirmLocPeople;
	ConfirmLoc ConfirmLocPatrol;
	ConfirmLoc ConfirmLocP3at;

	ofstream myfile;
	string directory;
	bool formatbool;

	bool Peopleready;
	bool Patrolready;
	bool P3atready;

	ArCondition StatusReadyGoCondition;


};

#endif