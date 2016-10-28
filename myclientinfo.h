#ifndef MyClientInfo_H
#define MyClientInfo_H


#include "Aria.h" 
#include "ArNetworking.h"
#include <iostream>
#include "Auction.h"
#include "mapvar.h"
#include "CreateTasks.h"
#include "EvaluateTask.h"
#include "tasks.h"
#include <string>
#include "mySync.h"


// oct 27 
class myClientInfo : public ArRecurrentTask 
{ 
public:
	void task(); // Arrecurrent task
	myClientInfo(ArClientBase *client, CreateTasks *TASK); 
	virtual ~myClientInfo(void); 
	myClientInfo(ArClientBase *client, string name);

	void AddCreateTask(CreateTasks *TASK);
	void AddAuction(Auction *auction, int robotID);
	void AddRobotName(string name);
	void AddEvaluateTask(EvaluateTask *eval);
	void AddmySync(mySync *aSync);
	void InitializeRecurringTasks();
	
	void SendCurrentTaskList(vector<tasks> currenttasks);
	void SendBid(tasks abid);
	void SendTaskConfirmation(tasks bidconfirm);
	void SendTaskConfirmation(ConfirmLoc a);
	void SecondPathResolution(vector<tasks> SimilarLocations);
	void ExchangeValueMaps(vector<vector<mapvar>> currentmap);

	void StatusReadyGo(int robotID);

	void SendConflictLocationInformation(vector<tasks> alist);
	
	void AddThisPhysicalRobotName(string name);

	bool flag;
	double data;
	float datafloat;
	int posx;
	int posy;
	double dataangle;

	int getMapInfo();
	
protected: 

	void AreYouReadyReturn(ArNetPacket *packet); //The sync function between robots
	void DoneSendingReturn(ArNetPacket *packet); // recieved when the server knows its done receving tasks from this client
	
	double serverInfo1, serverInfo2;
	int AuctionClientState;
	string ClientName;
	string ThisRobot;

	ArClientBase *myClient; 
	CreateTasks *myCreateTasks;
	EvaluateTask *myEval;
	Auction *myAuction;
	mySync *Sync;

	ArFunctor1C<myClientInfo, ArNetPacket *> HandleAreYouReady;
	ArFunctor1C<myClientInfo, ArNetPacket *> HandleDoneSending;


	bool PeopleReceivedTasks;
	bool PatrolReceivedTasks;
	bool P3atReceivedTasks;
	
};


#endif // MyClientInfo_H