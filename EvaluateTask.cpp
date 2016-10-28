#include "EvaluateTask.h"
/*
EvaluateTask::EvaluateTask(ArRobot *robot,CreateTasks *Task, GridMap *map){ // not used anymore
evalGridMap = map;
createTask = Task;
deleteflag = false;
max_distance = 11; // limit radius the robot will search
myRobot = robot;
	directory = "./BiddingValues.txt";
	myfile.open( directory.c_str() );
	if(myfile.fail())
	{
		cout << directory << " failed to open" << endl;
		//exit(-1);
	}
}*/

EvaluateTask::EvaluateTask(ArRobot *robot){
	myRobot = robot;
	deleteflag = false;
	max_distance = 10.0; // limit radius the robot will search
		directory = "./BiddingValues.txt";
	myfile.open( directory.c_str() );
	if(myfile.fail())
	{
		cout << directory << " failed to open" << endl;
		//exit(-1);
	}
}

void EvaluateTask::AddCreateTasks(CreateTasks *Task){
	createTask = Task;
}
void EvaluateTask::AddGridmap(GridMap *map){
	evalGridMap = map;
}

void EvaluateTask::AddPathTask(ArPathPlanningTask *newpathTask){
	pathTask = newpathTask;
}

void EvaluateTask::AddSelfRobotName(string name){
	robotname = name;
}

void EvaluateTask::updateTaskListVector(vector <tasks> newtasks){ //these are self created tasks // no longer used
	while(deleteflag == true){};
	
	for(unsigned int i = 0; i < newtasks.size();i++){
			TaskList.push_back(newtasks[i]);	
	}

}

void EvaluateTask::updateTaskList(tasks newtask){ //from server: these are new tasks

	while(deleteflag == true){};
	TaskList.push_back(newtask);
	//printf("Size of task list in update task list = %d\n", TaskList.size());

}


vector<tasks> EvaluateTask::DistanceFilter(vector<tasks> list){
	vector<tasks>::iterator it;
	for(it = list.begin(); it != list.end();++it){
		//printf("is %f biger than %f\n", it->dist, max_distance);
		if(it->dist > max_distance){
		//	printf("distance was too long = %f\n",it->dist);
			//printf("loc i am deleting %d %d\n", it->x_index, it->y_index);
			//printf("I DELETED SOMETHING!!! in Distance Filter Function \n");
			it = list.erase(it);
		}
	}// end of for
return list;
}

double EvaluateTask::ComputeDistance(vector<tasks> list, int i, vector<vector<mapvar>> map){
	double num;
	num = sqrt(pow(map[list[i].y_index][list[i].x_index].x_global - myRobot->getX(),2) + pow(map[list[i].y_index][list[i].x_index].y_global - myRobot->getY(),2));
	return num*(0.00328);  // 0.00328 changes mm to feet
} 

double EvaluateTask::ComputeDistance(double x1, double y1, double x2, double y2){
	double distance = sqrt(pow(x1 - x2,2) + pow(y1 - y2,2));
	return distance*(0.00328); // change to feet
}

vector<tasks> EvaluateTask::ComputeDistanceVector(vector<tasks> list, vector<vector<mapvar>> map){
	for(unsigned int i = 0; i < list.size(); i++){
		list[i].dist = sqrt(pow(map[list[i].y_index][list[i].x_index].x_global - myRobot->getX(),2) + pow(map[list[i].y_index][list[i].x_index].y_global - myRobot->getY(),2));
		list[i].dist = list[i].dist*(0.00328);
		//printf("distance = %f\n",list[i].dist);
	}
return list;
}

vector<tasks> EvaluateTask::ComputeUtility(vector<tasks> alist, vector<vector<mapvar>> map){
	vector<tasks>::iterator it;
	double numerator = 0;
	double denominator = 0;
	double tempcost = 5;
	for(it = alist.begin(); it != alist.end();++it){
	/*	if(it->pathcost ==0){
			continue;
		}
		else if(it->pathcost != 0){*/
			denominator =(it->pathcost + tempcost);
			numerator = map[it->y_index][it->x_index].value;
			myfile << "pathcost = " << it->pathcost << endl;
			myfile << "denominator = " << denominator << endl;
			printf("DENOMINATOR DENOMINATOR DENOMINATOR DENOMINATOR %f\n", denominator);
			it->utility = numerator/denominator;
			myfile << "utility = " << it->utility << endl << endl;
	//	}
	//	else{printf("ERROR ERROR !!!!!! COMPUTEUTILITY!!!!!\n");}
	}

	/*for(int i = 0; i < alist.size(); i++){
		printf("CHECKING %f\n", alist[i].utility);
	}*/


	return alist;
}


double EvaluateTask::ComputePathCost(int x_start, int y_start, int xend, int yend){
	list<ArPose> pathlist;
	pathlist = pathTask->getPathFromTo(ArPose(x_start,y_start),ArPose(xend,yend)); // selfnote take into account degrees?
	list<ArPose>::iterator it;
	
	double pathcost = 0;
	double oldx = 0;
	double oldy = 0;
	for(it = pathlist.begin(); it != pathlist.end();++it){
		pathcost = sqrt(pow(it->getX()- oldx,2) + pow(it->getY() - oldy,2)) + pathcost;
		oldx = it->getX();
		oldy = it->getY();
	}
/*	double durtime,durprog, durdist;
	durtime = pathTask->estimateTimeToGoal(ArPose(xend,yend));
	durdist = pathTask->estimateDistanceToGoal(ArPose(xend,yend));
	durprog = pathTask->computeProgress(ArPose(xend,yend));
	printf("Time = %f\n Dist = %f\n Prog = %f\n",durtime, durdist, durprog);*/
return pathcost*(0.00328); // 0.00328 changes mm to feet
}

vector<tasks> EvaluateTask::ComputePathCostVector(vector<tasks> alist, int xstart, int ystart){
	
	int debug_delete_count = 0;
	int debug_computecost_count = 0;
	//version 1
	vector<tasks>::iterator it;
	for(it = alist.begin(); it != alist.end();++it){
		it->pathcost = ComputePathCost(xstart,ystart, it->x_global, it->y_global);
		debug_computecost_count++;
		if(it->pathcost == 0){//means the robot canont path to this location 
		//	printf("COMPUTE PATH COSTVECTOR DELETED SOMETHING!!!\n");
		//	it = alist.erase(it);
			debug_delete_count++;
		}
	}// end of for
	
	//version 2
	/*vector<unsigned int> DeleteThese;
	for(unsigned i = 0; i < alist.size(); i++){
		alist[i].pathcost = ComputePathCost(xstart, ystart,alist[i].x_global, alist[i].y_global);
		if(alist[i].pathcost == 0){ //means robot cannot path to this location;
			printf("COMPUTEPATHVECTOR WANTS TO DELETE SOMETHING!!!\n");
			DeleteThese.push_back(i);
		}
	}//end of for*/

	/*printf("DEBUG LINE ---- DELETETHESE.size() == %d!!!!!!!!!!!!!!!\n",DeleteThese.size());
	for(unsigned j = 0; j < DeleteThese.size(); j++){
		if(DeleteThese[j] == 0){
			alist.erase(alist.begin());
			printf("DEBUG LINE ---- DELETED INDEX %d\n",DeleteThese[j]);
		}
		else if(DeleteThese[j] != 0){
			int deleteint = DeleteThese[j] - 1;
			alist.erase(alist.begin()+ deleteint);
			printf("DEBUG LINE ---- DELETED INDEX %d\n",DeleteThese[j]);
		}
		else{printf("ERROR ERROR ERROR IN COMPUTEPATHVECTOR!!!!!!\n");
	}*/



return alist;
}

vector<tasks> EvaluateTask::ComputeGlobalPositions(vector<tasks> alist, vector<vector<mapvar>> map){
	
	
	for(unsigned int i = 0; i < alist.size(); i++){ //was CurrentTaskList.size()...
		alist[i].x_global = map[alist[i].y_index][alist[i].x_index].x_global;
		alist[i].y_global = map[alist[i].y_index][alist[i].x_index].y_global;
	}
	return alist;
}

vector<tasks> EvaluateTask::ComputeTimeCost(vector<tasks> alist){ //incomplete july 12
	//double ExpectedTimeDuration;


	return alist;
}

void EvaluateTask::chooseMyBid(vector<vector<mapvar>> map){
	//This function clears what is in currentTasklist
	//then puts all tasks recieved and created into CurrentTaskList
	//Deletes Task list
	//Lastly fills in cost and information data in CurrentTaskList

	
	if(CurrentTaskList.size() >= 1){
		CurrentTaskList.clear();
	}
	CurrentTaskList = TaskList; // TaskList are all the self created tasks and recieved tasks
	deleteflag = true;
	TaskList.clear();
	deleteflag = false;

	/*CurrentTaskList = ComputeDistanceVector(CurrentTaskList, map); // initial cost
	//CurrentTaskList = DistanceFilter(CurrentTaskList);
	CurrentTaskList = ComputeGlobalPositions(CurrentTaskList, map); // fill in global positions
	CurrentTaskList = ComputePathCostVector(CurrentTaskList, myRobot->getX(), myRobot->getY()); // path cost
	CurrentTaskList = ComputeTimeCost(CurrentTaskList);
	CurrentTaskList = ComputeUtility(CurrentTaskList, map);*/

	CurrentTaskList = FillCostsAndInformation(CurrentTaskList);
	//printf("Size of Current Task list AFTER FillCostAndInformaiton function = %d\n", CurrentTaskList.size());


}

vector<tasks> EvaluateTask::FillCostsAndInformation(vector<tasks> ComputList){
	
	vector<vector<mapvar>> map = evalGridMap->MyGridMap;
	//printf("Size of Current Task list AFTER FillCostAndInformaiton function = %d\n", CurrentTaskList.size());
	ComputList = ComputeDistanceVector(ComputList, map); // initial cost
	//alist = DistanceFilter(alist);
	ComputList = ComputeGlobalPositions(ComputList, map); // fill in global positions
	ComputList = ComputePathCostVector(ComputList, myRobot->getX(), myRobot->getY()); // path cost //localization must be correct for this to work, else it crashes
	ComputList = ComputePathValue(ComputList, map);
	//ComputList = ComputeTimeCost(ComputList);
	ComputList = ComputeUtility(ComputList, map);
	ComputList = SumCosts(ComputList);

	/*for(int i = 0; i < alist.size(); i++){
		printf("COMPUTATIONAL VALUE::%d Utility Computed = %f\n", i, alist[i].utility);
	}*/
	return ComputList;
}


vector<tasks> EvaluateTask::ComputePathValue(vector<tasks> alist, vector<vector<mapvar>> amap){






return alist;
}


vector<tasks> EvaluateTask::SumCosts(vector<tasks> alist){
	for(unsigned int i = 0; i < alist.size(); i++){
		alist[i].cost = alist[i].pathcost;
	}
	return alist;
}




bool EvaluateTask::SecondPathPlanCheck(){
	//split the tasks in to two vectors //need to add one with p3at later
	bool conflict = false;
	vector<tasks> peopletasks, patroltasks, p3attasks;
	printf("size of currentasklist = %d \n",CurrentTaskList.size());
	for(unsigned int i = 0; i < CurrentTaskList.size();i++){
		if(CurrentTaskList[i].robotcreator.compare("people") == 0){
			peopletasks.push_back(CurrentTaskList[i]);
			//printf("Pushed onto peopletasks\n");
		}
		else if (CurrentTaskList[i].robotcreator.compare("patrol") == 0){
			patroltasks.push_back(CurrentTaskList[i]);
			//printf("Pushed onto patroltasks\n");
		}
		else if (CurrentTaskList[i].robotcreator.compare("p3at") == 0){
			p3attasks.push_back(CurrentTaskList[i]);
			//printf("Pushed onto p3attasks\n"); 
		}
		else{printf("ERROR IN SECONDPATHCHECK ROBOTCREATOR NAME IS WRONG\n");}
	}//end of for loop
 
	//create a vector of the locations that are similar
	//ArUtil::sleep(1000); // stops the error from server when a distance is received
	//SimilarLocations.clear(); // added debugclearsimilarlocations to get rid of error
	if(peopletasks.size() != 0 && patroltasks.size() != 0){
		if(robotname.compare("people") == 0 || robotname.compare("patrol") == 0){ // may not be needed
			//printf("CompareVectorDistance between patrol and people\n");
			SimilarLocations = CompareVectorDistances(peopletasks, patroltasks,SimilarLocations);
		}
	}
	if(peopletasks.size() != 0 && p3attasks.size() != 0){
		if(robotname.compare("people") == 0 || robotname.compare("p3at") == 0){ // may not be needed
			//printf("CompareVectorDistance between p3at and people\n");
			SimilarLocations = CompareVectorDistances(peopletasks, p3attasks,SimilarLocations);
		}
	}
	if(patroltasks.size() != 0 && p3attasks.size() != 0){
		if(robotname.compare("patrol") == 0 || robotname.compare("p3at") == 0){ // may not be needed
			//printf("CompareVectorDistance between patrol and p3at\n");
			SimilarLocations = CompareVectorDistances(patroltasks, p3attasks, SimilarLocations);
		}
	}

	//printf("Size of SIMILAR Locations = %d \n", SimilarLocations.size());
	if(SimilarLocations.size() >= 1){
		conflict = true;
		int index = GrabMaxLocationsInVector(SimilarLocations);
	//	printf("The robot = %s has a conflict with robot = %s\n",SimilarLocations[index].robotcreator.c_str(), SimilarLocations[index].robotSimilarLocation.c_str());
	//	printf("For location (%d,%d) \n", SimilarLocations[index].x_index, SimilarLocations[index].y_index);
	//	printf("and (%d,%d)\n", SimilarLocations[index].x_index_Compete, SimilarLocations[index].y_index_Compete);
	}
	else if(SimilarLocations.size() == 0){
		conflict = false;
		printf("This robot = %s has NO conflict\n",robotname.c_str());
	}
	else{printf("ERROR SIMILAR LOCATIONS CONFLICT BOOL!!!!!!\n");}

	//printf("DEBUG LINE --- Second path plan check = %d\n", conflict);
	return conflict;
}

double  EvaluateTask::CenterLocationComputation(){

	SimilarLocations[0].x_global_Compete = evalGridMap->ReturnXglobal(SimilarLocations[0].x_index_Compete, SimilarLocations[0].y_index_Compete);
	SimilarLocations[0].y_global_Compete = evalGridMap->ReturnYglobal(SimilarLocations[0].x_index_Compete, SimilarLocations[0].y_index_Compete);
	

	SimilarLocations[0].x_global_Center = (SimilarLocations[0].x_global_Compete + SimilarLocations[0].x_global)/2;
	SimilarLocations[0].y_global_Center = (SimilarLocations[0].y_global_Compete + SimilarLocations[0].y_global)/2;

	//printf("NewContentionLocation = (%d,%d)\n", SimilarLocations[0].x_global_Center , SimilarLocations[0].y_global_Center );
	//printf("NEWCONTENTIONLOCATION index = (%d, %d)\n", SimilarLocations[0].x_index_Compete, SimilarLocations[0].y_index_Compete);

	SimilarLocations[0].x_global = SimilarLocations[0].x_global_Center;
	SimilarLocations[0].y_global = SimilarLocations[0].y_global_Center;

	
	
	//whoever has shorter distance owns task
	SimilarLocations[0].myDistanceToCenterLocation = ComputeDistance(myRobot->getX(), myRobot->getY(), SimilarLocations[0].x_global_Center , SimilarLocations[0].y_global_Center );
	
	return SimilarLocations[0].myDistanceToCenterLocation;
}

vector<tasks> EvaluateTask::GrabSimilarLocations(){
	return SimilarLocations;
}

void EvaluateTask::SignalExchangeValuesCondition(){
	ExchangeValuesCondition.signal();
}
void EvaluateTask::WaitExchangeValuesCondition(){
	ExchangeValuesCondition.wait();
}

void EvaluateTask::SignalDistanceValueCondition(){
	printf("SIGNAL - Distance value condition\n");
	ReceivedDistanceValueCondition.signal();
}

void EvaluateTask::WaitDistanceValueCondition(){
	printf("WAIT - Distance value condition\n");
	ReceivedDistanceValueCondition.wait();
}

int EvaluateTask::GrabMaxLocationsInVector(vector<tasks> alist){
	
	int max, current, index;
	max = evalGridMap->GetValue(alist[0].x_index, alist[0].y_index);
	index = 0;
	for(unsigned int i = 0; i < alist.size(); i++){
		if(alist[i].robotcreator.compare(robotname.c_str()) == 0){ // look at tasks that i created
			current = evalGridMap->GetValue(alist[i].x_index, alist[i].y_index);
			if(current > max){
				max = current;
				index = i;
			}
		}
	}//end of for loop

return index;

}


vector<tasks> EvaluateTask::CompareVectorDistances(vector<tasks> plist, vector<tasks> qlist, vector<tasks> result){

	double icompareX, icompareY, jcompareX, jcompareY, distance;
	//now compare all locations in patroltask with peopletasks
	for(unsigned int i = 0; i < plist.size();i++){	
		icompareX = plist[i].x_global;
		icompareY = plist[i].y_global;
		for(unsigned int j = 0; j < qlist.size();j++){
			jcompareX = qlist[j].x_global;
			jcompareY = qlist[j].y_global;
			distance = ComputeDistance(icompareX, icompareY, jcompareX, jcompareY);
			//printf("COMPAREVECTORDISTANCE DISTANCE = %f\n", distance);
			//printf("%s and %s distance = %f", plist[i].robotcreator.c_str(), qlist[j].robotcreator.c_str(), distance);
			//printf("  (%d,%d) (%d,%d)\n", plist[i].x_index, plist[i].y_index, qlist[j].x_index, qlist[j].y_index);
			
			if(distance < 2){ // 10 was a test parameter to make this work
				//push i and j into one vector
				//printf("Distance is less than 10 robot name = %s\n", robotname.c_str());
				if(plist[i].robotcreator.compare(robotname.c_str()) == 0){
					plist[i].robotSimilarLocation = qlist[j].robotcreator;
					plist[i].x_index_Compete = qlist[j].x_index;
					plist[i].y_index_Compete = qlist[j].y_index;
					result.push_back(plist[i]);
					//printf("pushing plist onto results\n");
				}
				else if(qlist[j].robotcreator.compare(robotname.c_str()) == 0){
					qlist[j].robotSimilarLocation = plist[i].robotcreator;
					qlist[j].x_index_Compete = plist[i].x_index;
					qlist[j].y_index_Compete = plist[i].y_index;
					result.push_back(qlist[j]);			
					//printf("pushing qlist onto results\n");
				}
				else{printf("ERROR IN COMPAREVECTORDISTANCES!!!!!!!!\n");}
			}
		}//q loop
	}//p loop


return result;
}

double EvaluateTask::PathCost(){ // old
	double t1;
	t1 = 2.9;
	return t1;
}

void EvaluateTask::PushCurrentTaskList(){//only takes care of single case, not multiple
	CurrentTaskList.push_back(SimilarLocations[0]);
	/*printf("Size of CurrentTaskList after Adding SimilarLocations = %d\n", CurrentTaskList.size());
	for(int i = 0; i < CurrentTaskList.size(); i++){
	//printf("Locations = %d %d with index %d %d\n",CurrentTaskList[i].x_global, CurrentTaskList[i].y_global,
			CurrentTaskList[i].x_index, CurrentTaskList[i].y_index);
	}*/
}
		
vector<tasks> EvaluateTask::computeMyBid(){
	//looking at CurrentTaskList, i grab the max N of locations and compute a bid for them. 
	vector<tasks> Bidding_tasks;
	Bidding_tasks = GrabMax(CurrentTaskList, CurrentTaskList.size());
	for(unsigned int i = 0 ; i < Bidding_tasks.size(); i++){
		Bidding_tasks[i].bid = Bidding_tasks[i].utility - Bidding_tasks[i].cost;
		Bidding_tasks[i].bid = abs(Bidding_tasks[i].bid);
		printf("Bid:: %f = %f - %f\n",Bidding_tasks[i].bid, Bidding_tasks[i].utility, Bidding_tasks[i].cost);
		myfile << "Bidding on loc " << Bidding_tasks[i].x_global << " " << Bidding_tasks[i].y_global;
		myfile << " with bid = " << Bidding_tasks[i].bid << " to " << Bidding_tasks[i].robotcreator <<endl;
	}
	myfile << endl << endl;

	myOwnBids = Bidding_tasks;
return Bidding_tasks;
}


vector<tasks> EvaluateTask::ReturnMyOwnBids(){
	return myOwnBids;
}


vector<tasks> EvaluateTask::GrabMax(vector<tasks> list , int n){ //this functionis not complete 
	double max;
	int counter, max_index;
	max_index = 0;
	counter = 1;
//	vector<mapvar> max_vector;
	vector<tasks> max_vector;

	for(unsigned int j = 0; j < list.size() && counter <=n;j++){
		max = list[j].utility;	
		max_index = j;
		for(unsigned int i = 0; i < list.size();i++){
			if(max < list[i].utility){
				max = list[i].utility;
				max_index = i;
			}
		}
		max_vector.push_back(list[max_index]);
		list[max_index].utility = 0;
		counter++;
	}

return max_vector;
}

vector<tasks> EvaluateTask::GrabMaxBid(vector<tasks> list, int n){
	double max;
	int counter, max_index;
	max_index = 0;
	counter = 1;
	vector<tasks> max_vector;

	for(unsigned int j = 0; j < list.size() && counter <=n;j++){
		max = list[j].bid;	
		max_index = j;
		for(unsigned int i = 0; i < list.size();i++){
			if(max < list[i].bid){
				max = list[i].bid;
				max_index = i;
			}
		}
		max_vector.push_back(list[max_index]);
		list[max_index].bid = 0;
		counter++;
	}

return max_vector;
}





vector<tasks> EvaluateTask::getTaskList(){
	return TaskList;
}

vector<double> EvaluateTask::Debug_CostCompute(){

//printf("I AM IN EVALUATE TASK DEBUG COST COMPUTE\n\n\n");

vector<double> temp;
return temp;
}

void EvaluateTask::PutOtherRobotsDistance(double distance){
	//printf("I SAW A DISTANCE from another robot %f\n", distance);
	
	checksimilar:
	if(SimilarLocations.size() != 0){
		SimilarLocations[0].otherDistanceToCenterLocation = distance;
	}
	else{
		ArUtil::sleep(1000);
		goto checksimilar;
	}

}

void EvaluateTask::WhoOwnConflictLocation(){//compare otherDistanceToCenterLocation with myDistanceToCenterLocation
//The closer robot owns the location.

	printf("Distance (%f,%f)\n", SimilarLocations[0].otherDistanceToCenterLocation,SimilarLocations[0].myDistanceToCenterLocation);
	
	if(SimilarLocations[0].otherDistanceToCenterLocation >= SimilarLocations[0].myDistanceToCenterLocation){
		SimilarLocations[0].CenterLocationOwner = robotname;	
		SimilarLocations[0].robotcreator = robotname;
		printf("The owner of the Task Location = %s\n", SimilarLocations[0].CenterLocationOwner.c_str());
	}
	else if(SimilarLocations[0].otherDistanceToCenterLocation < SimilarLocations[0].myDistanceToCenterLocation){
		SimilarLocations[0].CenterLocationOwner = SimilarLocations[0].robotSimilarLocation;
		SimilarLocations[0].robotcreator = SimilarLocations[0].robotSimilarLocation;
		printf("The owner of the Task Location = %s\n", SimilarLocations[0].CenterLocationOwner.c_str());
	}
	else{printf("ERROR IN WHOOWNSCONFLICTLOCATION EVALUATETASK CLASS !!!!! \n");}

}


void EvaluateTask::DEBUG_ClearSimilarLocations(){
	SimilarLocations.clear();
}