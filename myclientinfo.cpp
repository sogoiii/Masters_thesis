#include "myclientinfo.h"


//USE THIS CONSTRUCTOR!!!
myClientInfo::myClientInfo(ArClientBase *client, string name):	
	HandleAreYouReady(this,&myClientInfo::AreYouReadyReturn),
	HandleDoneSending(this,&myClientInfo::DoneSendingReturn)
{
	myClient = client;
	ClientName = name;
	myClient->addHandler("AreYouReady", &HandleAreYouReady); // sync start function
	myClient->addHandler("DoneSending", &HandleDoneSending); // Done sending confirm
}

myClientInfo::~myClientInfo(void) {
}

void myClientInfo::AddCreateTask(CreateTasks *TASK){
	myCreateTasks = TASK;
}

void myClientInfo::AddThisPhysicalRobotName(string name){
	ThisRobot = name;
}

void myClientInfo::AddAuction(Auction *auction, int robotID){
	myAuction = auction;
	if(robotID >= 3){ // nothing should be 3 or greater
		AuctionClientState = NULL;
	}
	else{
		AuctionClientState = robotID; //if 0 people, if 1 patrol, if 2 p3at
		printf("Client with ID = %d now has auction\n",robotID);
	}
	go(); // start Client ArRecurrentTask task();
}

void myClientInfo::AddEvaluateTask(EvaluateTask *eval){
	myEval = eval;
}

void myClientInfo::AddmySync(mySync *aSync){
	Sync = aSync;
}

void myClientInfo::task(){ // add auction starts this recurrent task.
	//printf("%sClient has client state = %d\n",robotname.c_str(), AuctionClientState);
	//step 1, call wait
	//step 2, on signal, send data for robot
	//step 3, restart recurring task
	myAuction->SendConfirmConditionWait(AuctionClientState); // pauses this thread untill auction tells it to send something
	SendTaskConfirmation(myAuction->ReturnConfirmLoc(AuctionClientState));
	
	go(); //Must be the last line, this restarts the recurring task 
}

void myClientInfo::InitializeRecurringTasks(){
	go();
}

void myClientInfo::AreYouReadyReturn(ArNetPacket *packet){
	//if i come here the other robot is ready -- this is not true

	char buffer[512];
	packet->bufToStr(buffer, sizeof(buffer));
	printf("The robot %s now knows this robot is ready\n",buffer);
	//printf("TESTING TESTING TESTING\n");
	myClient->requestStop("AreYouReady"); // stop requesting this packet
	flag = true;
}

void myClientInfo::StatusReadyGo(int robotID){ // ask other robot if you are ready

	printf("Telling %srobot that robotID = %d is ready\n", ClientName.c_str(), robotID);
	if (myClient->isConnected())
	{
		flag = false;
		ArNetPacket *sending = new ArNetPacket();
		sending->byte4ToBuf(robotID); // tell the server that robotID is ready
		myClient->request("AreYouReady",1000,sending); // request every 1 second
		//myClient->requestOnce("AreYouReady", sending);
	/*	while(flag == false && myClient->isConnected()){
				//sleep for 10 microseconds whie we wait for data
				cout << "Waiting for the flag to change\n";
				ArUtil::sleep(2000);
				//cout << "Waiting on server\n";
		}*/
	}
/*	while(myAuction->ReturnReadyStatus(robotID) == false){
		printf("In status ready go while loop\n");	
		ArUtil::sleep(100); // sleep this thread
	}*/
	//printf("I am leaving the status ready go function\n");
}


void myClientInfo::ExchangeValueMaps(vector<vector<mapvar>> currentmap){
	
	if(myClient->isConnected()){
		printf("exchanging maps now\n");
		for(unsigned int i = 0; i < currentmap.size(); i++){
			for(unsigned int j = 0; j < currentmap[0].size();j++){
				ArNetPacket *sending = new ArNetPacket();
				sending->byte4ToBuf(currentmap[i][j].x_index);
				sending->byte4ToBuf(currentmap[i][j].y_index);
				sending->byte4ToBuf(currentmap[i][j].value);
				myClient->requestOnce("ExchangeValueMaps",sending);
			}// end of for looop
		}//end of second for loop
	}// end of if connected
	myEval->SignalExchangeValuesCondition(); // signal the main thread to continue
	printf("DONE EXCHANGING MAP INFORMATION\n");
}


void myClientInfo::SecondPathResolution(vector<tasks> SimilarLocations){ // incomplete
	if(myClient->isConnected()){
		printf("size of SimilarLocations vector = %d \n", SimilarLocations.size());
		for(unsigned int i = 0; i < SimilarLocations.size();i++){
			ArNetPacket *sending = new ArNetPacket();
			sending->byte4ToBuf(SimilarLocations[i].x_index);
			sending->byte4ToBuf(SimilarLocations[i].y_index);
			sending->strToBuf(SimilarLocations[i].robotcreator.c_str());
			myClient->requestOnce("SecondPathResolution", sending);
		}
	}// end of if
}

void myClientInfo::SendCurrentTaskList(vector<tasks> list){
//list = [xloc, yloc, value, obstacle percentage, dist, cost, utility, owner]`
//send only [xloc, yloc, value, obstacle percentage, utility, owner]

	if(myClient->isConnected()){
		printf("size of client vector = %d \n", list.size());
		for(unsigned int i = 0; i < list.size();i++){
			ArNetPacket *sending = new ArNetPacket();
			sending->byte4ToBuf(list[i].x_index);
			sending->byte4ToBuf(list[i].y_index);
			//sending->byte4ToBuf(list[i].value); // send this for a different revision 
			sending->byte4ToBuf(list[i].time_sec);
			sending->byte4ToBuf(list[i].time_min);
			sending->byte4ToBuf(list[i].time_hour);
			sending->strToBuf(list[i].robotcreator.c_str());
			myClient->requestOnce("ReceiveTasks", sending);
		}
	
	//Now send the I am done sending bits;
	ArNetPacket *Donesending = new ArNetPacket();
	Donesending->strToBuf(ClientName.c_str());
	myClient->requestOnce("DoneSending",Donesending);
	
	}
}

void myClientInfo::DoneSendingReturn(ArNetPacket *packet){
	char buffer[512];
	packet->bufToStr(buffer, sizeof(buffer));
	//printf("The robot %s knows it will not recive more tasks from this robot\n",buffer);
	Sync->ChangeReceivedTaskBool(Sync->ReturnRobotIDFromString(buffer));
	int robotID = myAuction->ReturnRobotIDFromString(buffer);

}


void myClientInfo::SendBid(tasks abid){
	//This function sends its batch of requests to robotname computer.
	//This robot will then wait for a confirm on a specific task 
	//need to send robot id
	
	if(myClient->isConnected()){
		ArNetPacket *sending = new ArNetPacket();
		//cout << "Sending Bid!! for loc "<< abid.x_index << " " << abid.y_index << "\n";
		sending->byte4ToBuf(abid.x_index);
		sending->byte4ToBuf(abid.y_index);
		sending->doubleToBuf(abid.bid);
		sending->byte4ToBuf(abid.time_sec);
		sending->byte4ToBuf(abid.time_min);
		sending->byte4ToBuf(abid.time_hour);
		sending->strToBuf(abid.robotbidder.c_str());
		myClient->requestOnce("ReceiveBid",sending);
	}

}

void myClientInfo::SendTaskConfirmation(tasks bid){ // no longer used
	//cout << "Sending Task confirmation\n";
	ArNetPacket *sending = new ArNetPacket();
	sending->byte4ToBuf(bid.x_index);
	sending->byte4ToBuf(bid.y_index);
	sending->doubleToBuf(bid.bid);
	bid.taskConfirmFlag = 1;
	sending->byte4ToBuf(bid.taskConfirmFlag);
	printf("STC Client\n");
	myClient->requestOnce("ReceiveTaskConfirmation",sending);

}

void myClientInfo::SendTaskConfirmation(ConfirmLoc a){ // is being used
	//printf("In Sending Task Confirmation\n");
	ArNetPacket *sending = new ArNetPacket();
	sending->byte4ToBuf(a.xloc);
	sending->byte4ToBuf(a.yloc);
	sending->doubleToBuf(a.bid);
	sending->byte4ToBuf(1); //This is the task confirm flag that i send
	myClient->requestOnce("ReceiveTaskConfirmation",sending);
}



void myClientInfo::SendConflictLocationInformation(vector<tasks> alist){ // tells other robot how far this client is from the newly created center point
	ArNetPacket *sending = new ArNetPacket();
	sending->doubleToBuf(alist[0].myDistanceToCenterLocation);
	sending->strToBuf(ThisRobot.c_str()); 
	printf("Sending distance data to %s\n", ClientName.c_str()); 
	myClient->requestOnce("SendConflictDistance",sending);
}










