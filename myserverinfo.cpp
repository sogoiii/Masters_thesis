#include "myserverinfo.h"
//#include "ValueMapInterrupt.h"
/*
myServerInfo::myServerInfo(ArServerBase *server, EvaluateTask *newtasks, ArPathPlanningTask *path) : //old
	myReceiveTasks(this,&myServerInfo::ReceiveTasks),
	myReceiveBid(this,&myServerInfo::ReceiveBid),
	myReceiveTaskConfirmation(this,&myServerInfo::ReceiveTaskConfirmation)
{
	myServer = server;
	//serverAuction = servAuction;
	serverEvaluateTask = newtasks;
	myPath = path;
	MovingNow = 0;
	//sends to the client updates about data on server

	//cout << "ADDING HANDLERS\n";
	myServer->addData("ReceiveTasks","Receiveing new tasks", &myReceiveTasks, "3 ints and 2 doubles", "nothing");	
	myServer->addData("ReceiveBid", "Receiveing A Bid", &myReceiveBid, " 1 double", "nothing returned");
	myServer->addData("ReceiveTaskConfirmation", "Receving task location to move to", &myReceiveTaskConfirmation, "location of place to move to", "returns nothing");
}*/


myServerInfo::myServerInfo(ArServerBase *server): //this one is in use
	myReceiveTasks(this,&myServerInfo::ReceiveTasks),
	myReceiveBid(this,&myServerInfo::ReceiveBid),
	myReceiveTaskConfirmation(this,&myServerInfo::ReceiveTaskConfirmation),
	myExchangeValue(this,&myServerInfo::ExchangeValueMaps),
	mySecondPathResolution(this, &myServerInfo::SecondPathResolution),
	myStatusReadyCheck(this, &myServerInfo::StatusReadyGoCheck),
	mySendConflictDistance(this, &myServerInfo::ReceiveConflictLocationInformation),
	myDoneSendingTask(this,&myServerInfo::DoneReceivingTasks)
{
	myServer = server;
	MovingNow = 0;
	myServer->addData("ReceiveTasks","Receiveing new tasks", &myReceiveTasks, "3 ints and 2 doubles", "nothing");	
	myServer->addData("ReceiveBid", "Receiveing A Bid", &myReceiveBid, " 1 double", "nothing returned");
	myServer->addData("ReceiveTaskConfirmation", "Receving task location to move to", &myReceiveTaskConfirmation, "location of place to move to", "returns nothing");
	myServer->addData("ExchangeValueMaps", "Compare recived value at (x,y) current value", &myExchangeValue,"x,y,value", "returns nothign");
	myServer->addData("mySecondPathResolution", "Deliberate with other robot", &mySecondPathResolution," ", "none"); // incomplete
	myServer->addData("AreYouReady", "Is the robot ready to start?", &myStatusReadyCheck, " " , "nothing");
	myServer->addData("SendConflictDistance", "Distance to conflict location", &mySendConflictDistance, "distance robotname " , "nothing");
	myServer->addData("DoneSending" , "Done Receiving tasks", &myDoneSendingTask, "the client sending data", "Server string that received tasks");


	 ConfirmTaskBool = false;

}

//myServerInfo::~myServerInfo() {}


void myServerInfo::AddEvaluateTask(EvaluateTask *newtasks){
	serverEvaluateTask = newtasks;
}

void myServerInfo::AddPathPlanning(ArPathPlanningTask *path){
	myPath = path;
}

void myServerInfo::AddAuctionClass(Auction *auction){
serverAuction = auction;
}

void myServerInfo::AddMoveToLocationClass(MoveToLocation *move){
moveto = move;
}

void myServerInfo::AddGridMap(GridMap *amap){
	serverGridMap = amap;
}

void myServerInfo::AddSelfRobotName(string name){ 
	Servername = name;
}



void myServerInfo::ExchangeValueMaps(ArServerClient *client, ArNetPacket *packet){
	int x, y, value;
	x = packet->bufToByte4();	
	y = packet->bufToByte4();
	value = packet->bufToByte4();	
	serverGridMap->ChooseMinValue(x,y,value);
	//serverGridMap->MyGridMap[y][x].value = value; // old unused

}

void myServerInfo::StatusReadyGoCheck(ArServerClient *client, ArNetPacket *packet){ //are you ready from client

	int robotID = packet->bufToByte4();
	printf("robot ID Received is = %d\n",robotID);
	serverAuction->ChangeReadyStatus(robotID); //the robotID is ready

	//Return with string of this server to reqesting client
	ArNetPacket sending;
	sending.strToBuf(Servername.c_str());
	client->sendPacketTcp(&sending);
	printf("SENT DATA BACK TO THE CLIENT!!!!!\n");
}



void myServerInfo::SecondPathResolution(ArServerClient *client, ArNetPacket *packet){

}



void myServerInfo::ReceiveTasks(ArServerClient *client, ArNetPacket *packet){
//list = [xloc, yloc, value, obstacle percentage, dist, cost, utility, owner]
//received only [xloc, yloc, value, obstacle percentage, utility, owner]
	tasks receivedTask;
	char buffer[512]; 

	receivedTask.x_index = packet->bufToByte4();	
	receivedTask.y_index = packet->bufToByte4();
	receivedTask.time_sec = packet->bufToByte4();	
	receivedTask.time_min = packet->bufToByte4();	
	receivedTask.time_hour = packet->bufToByte4();	
	packet->bufToStr(buffer, sizeof(buffer));
	receivedTask.robotcreator = buffer;

	//send receivedtask to auction class 
	serverEvaluateTask->updateTaskList(receivedTask); //i want to make this a functor
	printf("Received Task from %s\n", buffer);

	//testing for now
	//ArNetPacket sending;
//	sending.strToBuf(Servername.c_str());
	//client->sendPacketTcp(&sending);

}

void myServerInfo::ReceiveBid(ArServerClient *client, ArNetPacket *packet){

	tasks newbid;
	char buffer[512]; 

	newbid.x_index = packet->bufToByte4();
	newbid.y_index = packet->bufToByte4();
	newbid.bid = packet->bufToDouble();
	newbid.time_sec = packet->bufToByte4(); // time in seconds
	newbid.time_min  = packet->bufToByte4(); // time in minutes
	newbid.time_hour  = packet->bufToByte4(); // time in hours 
	packet->bufToStr(buffer, sizeof(buffer));
	newbid.robotbidder = buffer;
	printf("RB %d %d from %s\n",newbid.x_index ,newbid.y_index , buffer);
	//cout << "RB " << newbid.x_index << " " << newbid.y_index << " from " << buffer << endl;
	serverAuction->updateBidList(newbid);

}

void myServerInfo::ReceiveTaskConfirmation(ArServerClient *client, ArNetPacket *packet){
//I receive the location to move to, i should know the bid i placed..maybe not though
//printf("RTC Server\n");
CurrentTaskExport.x_index = packet->bufToByte4();
CurrentTaskExport.y_index = packet->bufToByte4();
CurrentTaskExport.bid = packet->bufToDouble();
CurrentTaskExport.taskConfirmFlag = packet->bufToByte4();

printf("Recieved Confirm on = %d %d with bid = %f\n", CurrentTaskExport.x_index, CurrentTaskExport.y_index,CurrentTaskExport.bid);

MovingNow = 1;
SignalServerMoveNowCondition(); // tells main thread to continue
}


int myServerInfo::returnMovingNow(){
	return MovingNow;
}

void myServerInfo::resetMovingNow(){
	MovingNow = 0;
}

void myServerInfo::WaitServerMoveNowCondition(){
	ServerMoveNowCondition.wait();
}

void myServerInfo::SignalServerMoveNowCondition(){
	ServerMoveNowCondition.signal(); // Server calls this when it receives a confirm from a robot
}

bool myServerInfo::ReturnConfirmTaskBool(){
	return ConfirmTaskBool;
}


void myServerInfo::ReceiveConflictLocationInformation(ArServerClient *client, ArNetPacket *packet){
	char buffer[512]; 
	double distance = packet->bufToDouble(); // the distance
	packet->bufToStr(buffer, sizeof(buffer)); // the robot with that distance
	printf("I GOT A distance %f from %sclient\n",distance,buffer);
	serverEvaluateTask->PutOtherRobotsDistance(distance); // this line is giving me trouble
	
	serverEvaluateTask->SignalDistanceValueCondition(); // tell main thread to continue
}


void myServerInfo::DoneReceivingTasks(ArServerClient *client, ArNetPacket *packet){
	//testing for now
	ArNetPacket sending;
	sending.strToBuf(Servername.c_str());
	client->sendPacketTcp(&sending);

}