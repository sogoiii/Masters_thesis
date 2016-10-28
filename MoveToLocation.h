#ifndef _MoveToLocation_
#define _MoveToLocation_

#include "Aria.h"
#include "Arnl.h"
#include "myDisplayImage.h"
#include "EvaluateTask.h"
#include <iostream>
#include <time.h>
#include <list>

class MoveToLocation{

public:
	MoveToLocation();
	void AddPathPlanning(ArPathPlanningTask *path);
	void AddEvaluateTask(EvaluateTask *moveEvaluateTask);
	int MoveToConfirmNow;
	void AddLocationToMoveTo(int x, int y);
	void AddDisplayImage(myDisplayImage *newDisplayImage);
	void NowMove();
	void PlanningFailedNextMove(int x, int y);
	void PlanningFailedNextMove2(int x, int y);
	void DEBUG_DrawThisPoint(int x, int y);
	void WaitMovePathCondition();
	void SignalMovePathCondition();

	int GetPathState();

	bool ContinueThread;
protected: 
	void MyGoalDone(ArPose goal);
	void MyGoalFailed(ArPose goal);
	void MyGoalInterrupted(ArPose goal);
	void MyGoalBlocked(const std::list<ArPose>* mynewlist);
	void MyPlainGoalBlocked();

	ArPathPlanningTask *aPath;
	myDisplayImage *DisplayImage;
	EvaluateTask *myEvaluateTask;
	int currentMoveX;
	int currentMoveY;
	int PathState;
	ArCondition MovePathCondition;
	ArFunctor1C<MoveToLocation, ArPose> MyGoalDoneCB; 
	ArFunctor1C<MoveToLocation, ArPose> MyGoalFailedCB; 
	ArFunctor1C<MoveToLocation, ArPose> MyGoalInterruptedCB;

	ArFunctor1C<MoveToLocation,const std::list<ArPose>*> MyGoalBlockedCB; 
	ArFunctorC<MoveToLocation > MyPlainGoalBlockedCB;

	//ArFunctor1C<MoveToLocation,list<ArPose> > MyGoalBlockedCB;
	//ArFunctorC<MoveToLocation> MyGoalBlockedCB;
};
#endif 