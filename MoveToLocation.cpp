#include "MoveToLocation.h"

MoveToLocation::MoveToLocation():
MyGoalDoneCB(this,&MoveToLocation::MyGoalDone),
MyGoalFailedCB(this,&MoveToLocation::MyGoalFailed),
MyGoalBlockedCB(this,&MoveToLocation::MyGoalBlocked),
//MyPlainGoalBlockedCB(this,&MoveToLocation::MyPlainGoalBlocked),
MyGoalInterruptedCB(this,&MoveToLocation::MyGoalInterrupted)
{
	MoveToConfirmNow = 0;
	PathState = 0;
	srand(time(NULL));
	ContinueThread = false;
}

void MoveToLocation::AddPathPlanning(ArPathPlanningTask *path){
aPath = path;
aPath->addGoalDoneCB(&MyGoalDoneCB);
aPath->addGoalFailedCB(&MyGoalFailedCB);
aPath->addGoalInterruptedCB(&MyGoalInterruptedCB);
//aPath->addPlainBlockedPathCB(&MyPlainGoalBlockedCB,100);
aPath->addBlockedPathCB(&MyGoalBlockedCB);

/*
aPath->addBlockedPathCBt
aPath->addGoalInterruptedCB
aPath->addPlainBlockedPathCB*/

}

void MoveToLocation::AddLocationToMoveTo(int x, int y){
currentMoveX = x;
currentMoveY = y;
}

void MoveToLocation::AddDisplayImage(myDisplayImage *newDisplayImage){
	DisplayImage = newDisplayImage;
}

void MoveToLocation::AddEvaluateTask(EvaluateTask * moveEvaluateTask){
	myEvaluateTask = moveEvaluateTask;
}

void MoveToLocation::NowMove(){

	if(aPath->getState() != ArPathPlanningTask::MOVING_TO_GOAL ){
	aPath->pathPlanToPose(ArPose(currentMoveX, currentMoveY,0),false,false);
	//ArUtil::sleep(2000);
	}
}
//Excess code 
		/*	if(pathTask.getState() == ArPathPlanningTask::FAILED_PLAN ||
				pathTask.getState() == ArPathPlanningTask::FAILED_MOVE ||
				pathTask.getState() == ArPathPlanningTask::ABORTED_PATHPLAN){
				cout << "INSIDE THE FAIL GETSTATE IF STATEMENT!\n";
			}
			*/
//Excess code 

void MoveToLocation::PlanningFailedNextMove(int x, int y){

	int secret_num;
	if(secret_num%2 == 0){
		currentMoveX = (rand()%100) + x + 800;
		currentMoveY = (rand()%100) + y + 800;
		DEBUG_DrawThisPoint(currentMoveX,currentMoveY);
	}
	else{
		currentMoveX = (rand()%100) + x - 800;
		currentMoveY = (rand()%100) + y - 800;
		DEBUG_DrawThisPoint(currentMoveX,currentMoveY);
	}

}



void MoveToLocation::PlanningFailedNextMove2(int x, int y){

}

void MoveToLocation::DEBUG_DrawThisPoint(int x, int y){
//the following equation will only work with the feb29BoundiresMap
//its the 171 which was computed as (mapminx)/20 = -171 hence i hadd 171

int xpixel = x/20 + 171;
int ypixel = y/20 + 374;

printf("I want to draw the pixel location (%d %d) \n", xpixel, ypixel);
DisplayImage->DrawThisLocationBLUE(xpixel, ypixel);
//DisplayImage->DEBUG_drawthesepoints(xpixel,ypixel);

}

void MoveToLocation::WaitMovePathCondition(){ // called by main
	printf("Move wait has been called\n");
	ContinueThread = true; // stop camera code thread in main
	MovePathCondition.wait();
}

void MoveToLocation::SignalMovePathCondition(){
	printf("Move Signal has been called\n");
	ContinueThread = false; // stop camera code thread in main
	MovePathCondition.signal();
}

void MoveToLocation::MyGoalDone(ArPose goal){ // PathState = 1;
	printf("MOVE TO LOCATION GOAL DONE\n");
	PathState = 1;
	SignalMovePathCondition();
}

void MoveToLocation::MyGoalFailed(ArPose goal){ // PathState = 2;
	printf("MOVE TO LOCATION GOAL FAILED\n");
	PathState = 2;
	SignalMovePathCondition();
}

void MoveToLocation::MyGoalBlocked(const std::list<ArPose>* mynewlist){  // PathState = 3
	//printf("MOVE TO LOCATION GOAL BLOCKED\n");
	printf("MyGoalBlocked !!!!!!!!!!!!!!!!!!!!!!!!!\n");
	PathState = 3;
	//SignalMovePathCondition();
}

void MoveToLocation::MyPlainGoalBlocked(){ 
	printf("MyPlainGoalBLocked----------------------\n");
	PathState = 3;
	//SignalMovePathCondition();
}



void MoveToLocation::MyGoalInterrupted(ArPose goal){ // Path State == ??? 
	printf("MOVE TO LOCATION GOAL INTERRUPTED\n\n");
	//myEvaluateTask->Debug_CostCompute();
	

//	SignalMovePathCondition(); // may not be neccesary since its done elsewhere but keeping for saftey
}


int MoveToLocation::GetPathState(){
	return PathState;
}