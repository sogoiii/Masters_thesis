#include "Auction.h"

Auction::Auction(ArRobot *robot):
myReviewBidsCB(this,&Auction::ReviewBidList)
{
	myRobot = robot;
	myRobot->addUserTask("ReviewBids",65,&myReviewBidsCB); // my own reccuring function to chekc bids
	SendConfirmBool = 0; //This lets the sending pass the first time
	checkbidcount = 0;
	//Setting time for the robots
	patrolConfirmTime.setToNow();
	peopleConfirmTime.setToNow();
	p3atConfirmTime.setToNow();

	//create my ConfirmSendVector
	bool tempbool = 0;
	for(int i = 0; i < 2;i++){
		ConfirmSendVector.push_back(tempbool);
	}

}

//void Auction::task(){
//	ReviewBidList();
//}

void Auction::AddGridMap(GridMap *grid){
	myGrid = grid;
}

void Auction::AddClient(myClientInfo *aClient, int n){

	switch(n){
		case 0:
			PeopleClient = aClient;
			break;
		case 1:
			PatrolClient = aClient;
			break;
		case 2:
			P3atClient = aClient;
			break;
		default:
			cout << "Error with input n! \n";
			break;
	}

}

void Auction::updateBidList(tasks newBid){
//i have to add the bid to a specific location. with one timer.
	//cout << "ubl = " << newBid.robotbidder << endl;
	myGrid->AddToBidVector(newBid.x_index, newBid.y_index, newBid.bid);
	myGrid->AddToBidVectorBidder(newBid.x_index, newBid.y_index, newBid.robotbidder);
//	cout << "what is the bool? " << myGrid->CheckBidVectorSize(newBid.x_index, newBid.y_index) << endl;
	if(myGrid->CheckBidVectorSize(newBid.x_index, newBid.y_index) == true){
		BidList.push_back(newBid);
		//cout << "I pushed back a new bid!\n";
	}
	else if(myGrid->CheckBidVectorSize(newBid.x_index, newBid.y_index) == false){
		//do nothing
		//cout << "I did not pushback a new bid\n";
	}
	else{
		cout << "THERE IS A PROBLEM IN UPDATE BID LIST!!\n\n\n\n";
	}

}

void Auction::ReviewBidList(){ //This is a recurring function
//This function reviews the bids in BidList and awards other robots when the time expires.

	if(checkbidcount == 40){
		printf("ReviewingBids with BidList = %d bidcount = %d\n",BidList.size(),checkbidcount);
		for(int i = 0; i < BidList.size();i++){
			printf("ArTime %d\n", BidList[i].ReturnCreatedTimeMiliseconds());
		//	if(diff_in_hour > 0 || diff_in_min >= 0 && diff_in_sec > 5){
			if(BidList[i].ReturnCreatedTimeMiliseconds() > 5000){
			//printf("Going to Choose a Winner!!!\n");
				ChooseWinner(i);
			}// end of if
		} // end of for
		checkbidcount = 0;
	} // end of if
	checkbidcount++;

}

void Auction::ChooseWinner(int n){ //gets call by review bid list (recurring)
//First: Check the bidvector to see how many bids are made for a location n
//Second: Send confirm to winning robot

	//cout << "Size of bid vector = " << myGrid->ReturnBidVectorSize(BidList[n].x_index, BidList[n].y_index) << endl;
	
	int max = 0;
	int index;
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

	printf("confirmbool = %d\n", SendConfirmBool); // this is a test line, may remove

	//chekc winner name and send to robot. If sent confirm already, dont send anything
	if(robotwinner.compare("people") == 0){
		printf("time difference = %d\n",peopleConfirmTime.mSecSince()); // this is a test line, may remove
		if(peopleConfirmTime.mSecSince() > 15000 || SendConfirmBool == 0){
			printf("sending Task confirm to people for loc %d %d \n", BidList[n].x_index, BidList[n].y_index);
			ConfirmLocPeople.xloc = BidList[n].x_index;
			ConfirmLocPeople.yloc = BidList[n].y_index;
			//PeopleClient->SendTaskConfirmation(BidList[n]);
			peopleConfirmTime.setToNow();
		}
		else{
			printf("Blocked send to %d, %d\n", BidList[n].x_index, BidList[n].y_index);
			printf("Already sent Confirm to robot %s for loc %d %d\n", robotwinner.c_str(), ConfirmLocPeople.xloc,ConfirmLocPeople.yloc);
		}
	}
	else if(robotwinner.compare("patrol") == 0){
		printf("time difference = %d\n",patrolConfirmTime.mSecSince());// this is a test line, may remove
		if(patrolConfirmTime.mSecSince() > 15000 || SendConfirmBool == 0){
			printf("sending Task confirm to patrol for loc %d %d \n", BidList[n].x_index, BidList[n].y_index);
			ConfirmLocPatrol.xloc = BidList[n].x_index;
			ConfirmLocPatrol.yloc = BidList[n].y_index;
			//PatrolClient->SendTaskConfirmation(BidList[n]);
			patrolConfirmTime.setToNow();
		}
		else{
			printf("Blocked send to %d, %d\n", BidList[n].x_index, BidList[n].y_index);
			printf("Already sent Confirm to robot %s for loc %d %d\n", robotwinner.c_str(), ConfirmLocPatrol.xloc,ConfirmLocPatrol.yloc);
		}
	}
	SendConfirmBool = 1; //this bool lets us send a confirm on the first round. after that the time is a constraint

	//These must be the Last two lines!
	swap(BidList[n],BidList[BidList.size() - 1]);
	BidList.pop_back();//What if i add a task inbetween these lines? Ill lose that bid   
}


		