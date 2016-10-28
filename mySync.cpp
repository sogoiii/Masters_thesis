#include "mySync.h"


mySync::mySync(){

	PeopleReceivedTasks = false;
	PatrolReceivedTasks = false;
	P3atReceivedTasks  = false;
}

mySync::~mySync(void){}

int mySync::ReturnRobotIDFromString(string robotstring){

	if(robotstring.compare("people") == 0){return 0;}
	if(robotstring.compare("patrol") == 0){return 1;}
	if(robotstring.compare("p3at") == 0){return 2;}
	else{return NULL;}

}


void mySync::ChangeReceivedTaskBool(int robotID){
	//assert(robotID <=2);
	//assert(robotID >=0);
	switch(robotID){
		case 0:
			PeopleReceivedTasks = true;
			printf("PEOPLE Received Tasks... Done .. in Sync\n");
			break;
		case 1:
			PatrolReceivedTasks = true;
			printf("PATROL Received Tasks... Done .. in Sync\n");
			break;
		case 2:
			P3atReceivedTasks = true;
			printf("P3AT Received Tasks... Done .. in Sync\n");
			break;
		case 3:
			printf("Error in ChangeReadyStatus in AUCTION CLASS!!!!\n");
			break;
	}
}

bool mySync::ReturnReceivedTaskBool(int robotID){
	//assert(robotID <=2);
	//assert(robotID >=0);
	switch(robotID){
		case 0:
			return PeopleReceivedTasks;
			break;
		case 1:
			return PatrolReceivedTasks;
			break;
		case 2:
			return P3atReceivedTasks;
			break;
		case 3:
			printf("Error in ChangeReadyStatus in AUCTION CLASS!!!!\n");
			break;
	}
}

void mySync::SignalReceivedTasksCondition(){
	ReceivedTasksCondition.signal();
	printf("ReceivedTasksCondition Signal\n");
}
void mySync::WaitReceivedTasksCondition(){
	printf("ReceivedTasksCondition Wait\n INSIDE OF SYNC FUNCTION WAITING FOR ALL ROBOTS TO CONFIRM RECEIVE\n");
	ReceivedTasksCondition.wait();
	}

void mySync::ReceivedTaskCheck(int numrobots, string robot1, string robot2, string robot3){
	bool ready1, ready2, ready3;
	switch(numrobots){
		case 1:
				ready1 = ReturnReceivedTaskBool(ReturnRobotIDFromString(robot1));
				while(ready1 == false){
					boost::this_thread::sleep(boost::posix_time::milliseconds(5));
					ready1 = ReturnReceivedTaskBool(ReturnRobotIDFromString(robot1));
					//printf("Are we ready = false\n");
				}			
				printf("ONE robots received Tasks\n");
				SignalReceivedTasksCondition();
			break;
		case 2:
				ready1 = ReturnReceivedTaskBool(ReturnRobotIDFromString(robot1));
				ready2 = ReturnReceivedTaskBool(ReturnRobotIDFromString(robot2));
				while(ready1 == false || ready2 == false ){
					boost::this_thread::sleep(boost::posix_time::milliseconds(5));
					//printf("checking ready flags\n");
					ready1 = ReturnReceivedTaskBool(ReturnRobotIDFromString(robot1));
					ready2 = ReturnReceivedTaskBool(ReturnRobotIDFromString(robot2));
				}
				printf("TWO robots received Tasks\n");
				SignalReceivedTasksCondition();
			break;
		case 3:
				ready1 = ReturnReceivedTaskBool(ReturnRobotIDFromString(robot1));
				ready2 = ReturnReceivedTaskBool(ReturnRobotIDFromString(robot2));
				ready3 = ReturnReceivedTaskBool(ReturnRobotIDFromString(robot3));
				while(ready1 == false || ready2 == false || ready2 == false){
					boost::this_thread::sleep(boost::posix_time::milliseconds(5));
					ready1 = ReturnReceivedTaskBool(ReturnRobotIDFromString(robot1));
					ready2 = ReturnReceivedTaskBool(ReturnRobotIDFromString(robot2));
					ready3 = ReturnReceivedTaskBool(ReturnRobotIDFromString(robot3));
				}
				SignalReceivedTasksCondition();
			break;
	}//end of switch
}