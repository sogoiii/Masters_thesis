#ifndef _Auction_
#define _Auction_

#include "Aria.h"
#include "tasks.h"
#include "myclientinfo.h"
#include "GridMap.h"
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


struct ConfirmLoc{
	int xloc;
	int yloc;
};

class Auction : public ArRecurrentTask //ArASyncTask//  ArRecurrentTask
{
	ArMutex myAuctionMutex;
public:
	void task(){ 
	//printf("ArRecurrentTask\n");
	ReviewBidList();
	go();
	};//ArRecurrentTask
	/*void* runThread(void*){
		myMutex.lock();
		printf("runThread\n");
		ReviewBidList();
		myMutex.unlock();
		return NULL;};*/
	Auction(ArRobot *robot);
	void updateBidList(tasks newbid); //add bid to our list
	void ReviewBidList(); // move to private 
	void AddClient(myClientInfo *aClient, int n);
	void AddGridMap(GridMap *grid);
	vector<tasks> BidList;  // vector of all the bids to be recieved. 

protected:
	//Member functions
	void ChooseWinner(int n);
	

	//Member Objects
	ArFunctorC<Auction> myReviewBidsCB;
	ArRobot *myRobot;
	myClientInfo *PeopleClient;
	myClientInfo *PatrolClient;
	myClientInfo *P3atClient;
	GridMap *myGrid;
	int checkbidcount;
	bool SendConfirmBool;
	ArTime patrolConfirmTime;
	ArTime peopleConfirmTime;
	ArTime p3atConfirmTime;
	vector<bool> ConfirmSendVector; //vector
	ConfirmLoc ConfirmLocPeople;
	ConfirmLoc ConfirmLocPatrol;
	ConfirmLoc ConfirmLocp3at;


};

#endif