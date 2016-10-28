#include "Auction.h"

Auction::Auction(ArRobot *robot):
myReviewBidsCB(this,&Auction::ReviewBidList)
{
	myRobot = robot;
	myRobot->addUserTask("ReviewBids",65,&myReviewBidsCB); // my own recurring function to check bids
	
	checkbidcount = 0;
	SendConfirmBool = 0; //This lets the sending pass the first time
	formatbool = false;
	//Setting time for the robots
	patrolConfirmTime.setToNow();
	peopleConfirmTime.setToNow();
	p3atConfirmTime.setToNow();

	//create my ConfirmSendVector
	bool tempbool = false;
	for(int i = 0; i <= 2;i++){
		ConfirmSendVector.push_back(tempbool);
	}

	directory = "./AuctionReviewBids.txt";
	myfile.open( directory.c_str() );
	if(myfile.fail()){
		cout << directory << " failed to open" << endl;
	}

	Peopleready = false;
	Patrolready = false;
	P3atready = false;
	TimeBetweenConfirms = 8000; //this is in milliseconds

}


void Auction::AddGridMap(GridMap *grid){
	myGrid = grid;
}

ConfirmLoc Auction::ReturnConfirmLoc(int robotID){

	switch(robotID){
		case 0:
			return ConfirmLocPeople;
			break;
		case 1:
			return ConfirmLocPatrol;
			break;
		case 2:
			return ConfirmLocP3at;
			break;
		case 3:
			printf("Error in ReturnConfirmLoc\n");
			ConfirmLoc TrashConfirmLoc;
			return TrashConfirmLoc;
			break;
	}
}
void Auction::SendConfirmConditionWait(int robotID){

	switch(robotID){
		case 0:
			PeopleCondition.wait();
			break;
		case 1:
			PatrolCondition.wait();
			break;
		case 2:
			P3atCondition.wait();
			break;
		case 3:
			printf("Error in Send Confirm Wait\n");
			break;
	}
}

void Auction::SendConfirmConditionSignal(int robotID){

	switch(robotID){
		case 0:
			PeopleCondition.signal();
			break;
		case 1:
			PatrolCondition.signal();
			break;
		case 2:
			P3atCondition.signal();
			break;
		case 3:
			printf("Error in Send Confirm Signal\n");
			break;
	}
}



bool Auction::ReturnReadyStatus(int robotID){
	switch(robotID){
		case 0:
			//printf("People status = %d\n",Peopleready);
			return Peopleready;
			break;
		case 1:
			//printf("Patrol status = %d\n",Patrolready);
			return Patrolready; 
			break;
		case 2:
			return P3atready;
			break;
		case 3:		
			printf("Error in ChangeReadyStatus!!!!\n");
			return NULL;
			break;
	}

}

void Auction::ChangeReadyStatus(int robotID){ // 0=people, 1=patrol, 2 = p3at
	switch(robotID){
		case 0:
			Peopleready = true;
			//printf("PEOPLE Status Changed to True\n");
			break;
		case 1:
			Patrolready = true;
			//printf("PATROL Status Changed to True\n");
			break;
		case 2:
			P3atready = true;
			//printf("P3AT Status Changed to True\n");
			break;
		case 3:
			printf("Error in ChangeReadyStatus in AUCTION CLASS!!!!\n");
			break;
	}
}

void Auction::StatusReadyGoWait(){
	printf("STATUSREADYGO = WAIT!!!!!\n");
	StatusReadyGoCondition.wait();
}

void Auction::StatusReadyGoSignal(){
	printf("STATUSREADYGO = SIGNAL!!!!!\n");
	StatusReadyGoCondition.signal();
}


int Auction::ReturnRobotIDFromString(string robotstring){

	if(robotstring.compare("people") == 0){return 0;}
	if(robotstring.compare("patrol") == 0){return 1;}
	if(robotstring.compare("p3at") == 0){return 2;}
	else{return NULL;}

}

void Auction::AreWeReady(int numberofrobots, string robot1, string robot2, string robot3){ //run in its own thread
	//check the auction ready flags that are changed by the server
	assert(numberofrobots <=3); // will have max 3 robots
	assert(numberofrobots != 0); // cannot allow for 0 robots

	bool ready1, ready2, ready3;
	switch(numberofrobots){
		case 1:
				ready1 = ReturnReadyStatus(ReturnRobotIDFromString(robot1));
				while(ready1 == false){
					boost::this_thread::sleep(boost::posix_time::milliseconds(5));
					ready1 = ReturnReadyStatus(ReturnRobotIDFromString(robot1));
				//	printf("Are we ready = false\n");
				}			
				//printf("ONE robot is ready\n");
				StatusReadyGoSignal();
			break;
		case 2:
				ready1 = ReturnReadyStatus(ReturnRobotIDFromString(robot1));
				ready2 = ReturnReadyStatus(ReturnRobotIDFromString(robot2));
				while(ready1 == false || ready2 == false ){
					boost::this_thread::sleep(boost::posix_time::milliseconds(5));
					//printf("checking ready flags\n");
					ready1 = ReturnReadyStatus(ReturnRobotIDFromString(robot1));
					ready2 = ReturnReadyStatus(ReturnRobotIDFromString(robot2));
				}
				//printf("TWO robots are ready\n");
				StatusReadyGoSignal();
			break;
		case 3:
				ready1 = ReturnReadyStatus(ReturnRobotIDFromString(robot1));
				ready2 = ReturnReadyStatus(ReturnRobotIDFromString(robot2));
				ready3 = ReturnReadyStatus(ReturnRobotIDFromString(robot3));
				while(ready1 == false || ready2 == false || ready2 == false){
					boost::this_thread::sleep(boost::posix_time::milliseconds(5));
					ready1 = ReturnReadyStatus(ReturnRobotIDFromString(robot1));
					ready2 = ReturnReadyStatus(ReturnRobotIDFromString(robot2));
					ready3 = ReturnReadyStatus(ReturnRobotIDFromString(robot3));
				}
				StatusReadyGoSignal();
			break;

	}//end of switch
}

void Auction::updateBidList(tasks newBid){


	myGrid->AddToBidVector(newBid.x_index, newBid.y_index, newBid.bid);
	myGrid->AddToBidVectorBidder(newBid.x_index, newBid.y_index, newBid.robotbidder);
	BidList.push_back(newBid);


	/*//i have to add the bid to a specific location. with one timer.
	myGrid->AddToBidVector(newBid.x_index, newBid.y_index, newBid.bid);
	myGrid->AddToBidVectorBidder(newBid.x_index, newBid.y_index, newBid.robotbidder);

	//check to see if robot bidder has placed a bid for this location (maybe)
	//if i have already placed a bid, then do nothing
	//if i have not placed a bid, add to my bidlist

	if(myGrid->CheckBidVectorSize(newBid.x_index, newBid.y_index) == true){
		BidList.push_back(newBid);
	}
	else if(myGrid->CheckBidVectorSize(newBid.x_index, newBid.y_index) == false){
		//do nothing
		printf("I DIDN'T ADD THIS TO MY BID VECTOR-------------------\n");
	}
	else{
		cout << "THERE IS A PROBLEM IN UPDATE BID LIST!!\n\n\n\n";
	}*/

}

void Auction::ReviewBidList(){ //This is a recurring function
//This function reviews the bids in BidList and awards other robots when the time expires.

	if(checkbidcount == 40){
		printf("BidList size in AUCTION = %d\n",BidList.size());
		myfile << "BidList size = " << BidList.size() << endl;;
			for(unsigned int j = 0; j < BidList.size();j++){
			myfile << "Loc [" << BidList[j].x_index << " " << BidList[j].y_index;	
			myfile << "] time = " << BidList[j].time_hour << " " << BidList[j].time_min << " " << BidList[j].time_sec ;
			myfile << " from " << BidList[j].robotbidder << endl;
			//myfile << "] ArTime = " << checktime.secSince() << "old from " <<    \n";
			}

		for(unsigned int i = 0; i < BidList.size();i++){
		//	printf("ArTime %d\n", BidList[i].ReturnCreatedTimeMiliseconds());
			
			formatbool = true;
			ArTime checktime = BidList[i].ReturnArTime();

			
			if(BidList[i].ReturnCreatedTimeMiliseconds() > 5000){
				ChooseWinner(i); // Function chooses who to send a task confirm to.
			}// end of if
		} // end of for
		checkbidcount = 0;
		if(formatbool == true){myfile << endl;formatbool = false;}
	} // end of if
	checkbidcount++;

}

void Auction::ChooseWinner(int n){ //gets called by review bid list (recurring)
//First: Check the bidvector to see how many bids are made for a location n
//Second: Send confirm to winning robot

	//cout << "Size of bid vector = " << myGrid->ReturnBidVectorSize(BidList[n].x_index, BidList[n].y_index) << endl;
	
	int max = 0;
	int index = 0;
	int newval = 0;
	int numofbids = myGrid->ReturnBidVectorSize(BidList[n].x_index, BidList[n].y_index);
	//check max
	for(int i=0; i < numofbids ;i++){
		newval = myGrid->ReturnBidVectorBid(BidList[n].x_index, BidList[n].y_index, i);
		if(newval > max){
			max = newval;
			index = i;
		}
	}
	//grab winner name	
	string robotwinner = myGrid->ReturnBidVectorBidder(BidList[n].x_index, BidList[n].y_index, index);

	//printf("confirmbool = %d\n", SendConfirmBool); // this is a test line, may remove

	//check winner name and send to robot. If sent confirm already, dont send anything
	if(robotwinner.compare("people") == 0){// if people wins location
		//printf("time difference = %d\n",peopleConfirmTime.mSecSince()); // this is a test line, may remove
		if(peopleConfirmTime.mSecSince() > TimeBetweenConfirms || ConfirmSendVector[0] == false){
			printf("Sending Task confirm to people for loc %d %d \n", BidList[n].x_index, BidList[n].y_index);
			ConfirmSendVector[0] = true;
			ConfirmLocPeople.xloc = BidList[n].x_index;
			ConfirmLocPeople.yloc = BidList[n].y_index;
			ConfirmLocPeople.bid = BidList[n].bid;
			ConfirmLocPeople.robot = "people";
			RemoveOldBids(ConfirmLocPeople, index);
			//boost::thread* PeopleRemoveThread = new boost::thread(boost::bind(&Auction::RemoveOldBids, this,ConfirmLocPeople,index));
			//PeopleClient->SendTaskConfirmation(BidList[n]); // old method
			PeopleCondition.signal(); //the wait is in myclientinfo class in ArRecurrentTask task()
			peopleConfirmTime.setToNow();
			myfile << "Sent confirm to people = " << BidList[n].x_index << " " << BidList[n].y_index << endl;
		}
		else{
		//	printf("Blocked send to %d, %d\n", BidList[n].x_index, BidList[n].y_index);
		//	printf("Already sent Confirm to robot %s for loc %d %d\n", robotwinner.c_str(), ConfirmLocPeople.xloc,ConfirmLocPeople.yloc);
		}
	}
	else if(robotwinner.compare("patrol") == 0){// if patrol wins location
	//	printf("time difference = %d\n",patrolConfirmTime.mSecSince());// this is a test line, may remove
		if(patrolConfirmTime.mSecSince() > TimeBetweenConfirms || ConfirmSendVector[1] == false){
			printf("TESTING - Sending Task confirm to patrol for loc %d %d \n", BidList[n].x_index, BidList[n].y_index);
			ConfirmSendVector[1] = true;
			ConfirmLocPatrol.xloc = BidList[n].x_index;
			ConfirmLocPatrol.yloc = BidList[n].y_index;
			ConfirmLocPatrol.bid = BidList[n].bid;
			ConfirmLocPatrol.robot = "patrol";
			RemoveOldBids(ConfirmLocPatrol, index);
			//boost::thread* PatrolRemoveThread = new boost::thread(boost::bind(&Auction::RemoveOldBids, this,ConfirmLocPatrol,index));
			//PatrolClient->SendTaskConfirmation(BidList[n]); //old method
			PatrolCondition.signal(); //the wait is in myclientinfo class in ArRecurrentTask task()
			patrolConfirmTime.setToNow();
			myfile << "Sent confirm to patrol = " << BidList[n].x_index << " " << BidList[n].y_index << endl;
		}
		else{
		//	printf("Blocked send to %d, %d\n", BidList[n].x_index, BidList[n].y_index);
		//	printf("Already sent Confirm to robot %s for loc %d %d\n", robotwinner.c_str(), ConfirmLocPatrol.xloc,ConfirmLocPatrol.yloc);
		}
	}
		else if(robotwinner.compare("p3at") == 0){// if p3at wins location
	//	printf("time difference = %d\n",p3atConfirmTime.mSecSince());// this is a test line, may remove
		if(p3atConfirmTime.mSecSince() > TimeBetweenConfirms || ConfirmSendVector[2] == false){
			printf("Sending Task confirm to patrol for loc %d %d \n", BidList[n].x_index, BidList[n].y_index);
			ConfirmSendVector[2] = true;
			ConfirmLocP3at.xloc = BidList[n].x_index;
			ConfirmLocP3at.yloc = BidList[n].y_index;
			ConfirmLocP3at.bid = BidList[n].bid;
			ConfirmLocP3at.robot = "p3at";
			RemoveOldBids(ConfirmLocP3at, index);
			//boost::thread* P3atRemoveThread = new boost::thread(boost::bind(&Auction::RemoveOldBids, this,ConfirmLocPatrol,index));
			//P3atClient->SendTaskConfirmation(BidList[n]); //old method
			P3atCondition.signal(); //the wait is in myclientinfo class in ArRecurrentTask task()
			p3atConfirmTime.setToNow();
			myfile << "Sent confirm to p3at = " << BidList[n].x_index << " " << BidList[n].y_index << endl;
		}
		else{
		//	printf("Blocked send to %d, %d\n", BidList[n].x_index, BidList[n].y_index);
		//	printf("Already sent Confirm to robot %s for loc %d %d\n", robotwinner.c_str(), ConfirmLocPatrol.xloc,ConfirmLocPatrol.yloc);
		}
	}
	else{
		printf("ERROR IN AUCTION CHOOSE WINNER!!!!!\n");
	}

	SendConfirmBool = 1; //this bool lets us send a confirm on the first round. after that the time is a constraint

	//These must be the Last two lines! //I reduce my bids list since i sent something to a robot
	swap(BidList[n],BidList[BidList.size() - 1]);
	BidList.pop_back();//What if i add a task inbetween these lines? Ill lose that bid   
}


		
void Auction::RemoveOldBids(ConfirmLoc theloc, int index){ //remove information on gridmap //i can pass in index 
	//boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
	
	//remove confirm loc robot id bid 
	myGrid->RemoveBidFromBidVector(theloc.xloc, theloc.yloc,index);
	myGrid->RemoveBidVectorBidder(theloc.xloc, theloc.yloc,index);

	if(theloc.robot.compare("people") == 0){
		//ConfirmSendVector[0] = false;
	}
	else if(theloc.robot.compare("patrol") == 0){
		//ConfirmSendVector[1] = false;
	}
	else if (theloc.robot.compare("p3at") == 0){
		//ConfirmSendVector[2] = false;
	}
	else{printf("ERROR IN REMOVEOLDBIS!!!!!!!!!!!\n");}

}