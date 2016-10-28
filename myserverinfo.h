#ifndef MyServerInfo_H
#define MyServerInfo_H

//Aria requirments
#include "Aria.h" 
#include "ArNetworking.h"
#include "Arnl.h"
//System requirments
#include <iostream>
#include <vector>
//My Classes
#include "mapvar.h"
#include "tasks.h"
#include "Auction.h"
#include "EvaluateTask.h"
#include "MoveToLocation.h"
#include "GridMap.h"
#include "mySync.h"
//



class myServerInfo 
{ 
public:
	/// Constructor 
	myServerInfo(ArServerBase *server, EvaluateTask *newtasks, ArPathPlanningTask *path);
	myServerInfo(ArServerBase *server);
	/// Destructor 
	//~myServerInfo(); 
	void AddEvaluateTask(EvaluateTask *newtasks);
	void AddPathPlanning(ArPathPlanningTask *path);
	void AddAuctionClass(Auction *auction);
	void AddMoveToLocationClass(MoveToLocation *move);
	void AddGridMap(GridMap *amap);
	
	//client access my data
	void ReceiveTasks(ArServerClient *client, ArNetPacket *packet);
	void ReceiveBid(ArServerClient *client, ArNetPacket *packet);
	void ReceiveTaskConfirmation(ArServerClient *client, ArNetPacket *packet);
	void ExchangeValueMaps(ArServerClient *client, ArNetPacket *packet);
	void SecondPathResolution(ArServerClient *client, ArNetPacket *packet);
	int returnMovingNow();	
	void resetMovingNow();
	tasks CurrentTaskExport; 
	
	//member getters and setters
	void WaitServerMoveNowCondition();
	void SignalServerMoveNowCondition(); 

	void StatusReadyGoCheck(ArServerClient *client, ArNetPacket *packet);
	void ChangeStatusReady(int robotID); // not used
	void AddSelfRobotName(string name);


	void ReceiveConflictLocationInformation(ArServerClient *client, ArNetPacket *packet);
	void DoneReceivingTasks(ArServerClient *client, ArNetPacket *packet);


	bool ReturnConfirmTaskBool();

protected:
	ArServerBase *myServer; 	
	Auction *serverAuction;
	EvaluateTask *serverEvaluateTask;
	ArPathPlanningTask *myPath;
	MoveToLocation *moveto;
	GridMap *serverGridMap;

	string Servername;
	bool Peopleready;
	bool Patrolready;
	bool P3atready;

	ArCondition ServerMoveNowCondition;

	bool ConfirmTaskBool;

		int MovingNow; // temporary , remove when done!!!
	//client access my data
	ArFunctor2C<myServerInfo, ArServerClient *, ArNetPacket *> myReceiveTasks;
	ArFunctor2C<myServerInfo, ArServerClient *, ArNetPacket *> myReceiveBid;
	ArFunctor2C<myServerInfo, ArServerClient *, ArNetPacket *> myReceiveTaskConfirmation;
	ArFunctor2C<myServerInfo, ArServerClient *, ArNetPacket *> myExchangeValue;
	ArFunctor2C<myServerInfo, ArServerClient *, ArNetPacket *> mySecondPathResolution;
	ArFunctor2C<myServerInfo, ArServerClient *, ArNetPacket *> myStatusReadyCheck;
	ArFunctor2C<myServerInfo, ArServerClient *, ArNetPacket *> mySendConflictDistance;
	ArFunctor2C<myServerInfo, ArServerClient *, ArNetPacket *> myDoneSendingTask;


};

#endif // MyServerInfo_H