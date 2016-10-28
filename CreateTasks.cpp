#include "CreateTasks.h"


CreateTasks::CreateTasks(ArRobot *robot, GridMap *map, myDisplayImage *Display){

myRobot = robot;
CTGridMap = map;
DisplayImage = Display;
BoundRadius = 1212.2; // this is 4 feet in mm for robot localization
//resolution = 20; // res // this is grabbed from display image prase function 
}

CreateTasks::CreateTasks(ArRobot *robot){
//	BoundRadius = 914.4;
//BoundRadius = 1212.2;
BoundRadius = 1510; // in mm almost 5 feet
myRobot = robot;
}

void CreateTasks::AddGridMap(GridMap *map){
CTGridMap = map;
}

void CreateTasks::AddDisplayImage(myDisplayImage *Display){
DisplayImage = Display;
}


void CreateTasks::AddSelfRobotName(string name){
	selfrobot = name;
}

void CreateTasks::DEBUG_CreateSpecificTask(string name, int x, int y){
	vector<tasks> Debug_TaskVector;
	tasks temptask;
	Debug_TaskVector.push_back(temptask);

	Debug_TaskVector[0].robotcreator = name;
	Debug_TaskVector[0].x_index = x;
	Debug_TaskVector[0].y_index = y;

	Debug_TaskVector[0].updatecreationtime(Debug_TaskVector[0].createdtime_tm); // time.h object
	Debug_TaskVector[0].UpdateCreateTime(); //Artime Object
	Debug_TaskVector[0].cost = 12;
	
	DEBUG_mySelfTasks.push_back(Debug_TaskVector[0]);
}

vector<tasks> CreateTasks::DEBUG_getmySelfTasks(){
return DEBUG_mySelfTasks;
}

void CreateTasks::CreateSelfTasks(int x_loc, int y_loc){

	double temprad = BoundRadius/DisplayImage->resolution;
	//cvCircle(image,cvPoint(curPos.x,curPos.y), temprad ,CV_RGB(0,250,0),1);

	int cx,cy,degree;
	degree = 20;
	//vector<vector<int>> circle_list;
	vector<int> temp;
	for(int i = 0;i < 2;i++){
	temp.push_back(cx);
	}
	for(int j= 0; j < 18; j++){
		circle_list.push_back(temp);
	}

	if(circletask.size() > 1){
		circletask.clear();
	}
	
	tasks task1; //temp variable
	for(int j = 0; j < 18;j++){
		circletask.push_back(task1);
	}



	int j;
	j = 0;

	curPos.x = x_loc /DisplayImage->resolution - DisplayImage->offsetX;
	curPos.y = y_loc /DisplayImage->resolution - DisplayImage->offsetY;
	//cout << "CUR POSE =====    " << curPos.x << " " << curPos.y << "\n";
	srand((unsigned)time(0)); //testing
	double random_double; //testing
	for(int i = 0; i < 360/degree ; i++){
		random_double = ((rand()%200)+1)/DisplayImage->resolution; //300mm about 1 foot //testing
		temprad = temprad + random_double; //testing
	//	printf("TEMP RANDOM INTEGER!!! = %d and original = %f\n", random_integer, temprad);
		cx = curPos.x + temprad*cos((degree/180.0)*PI*i);
		cy = curPos.y + temprad*sin((degree/180.0)*PI*i);	
	   // cout << "Possible Location pixel " << cx << " " << cy << "\n";
		//cout << "size of cirlce " << circle_list.size()  << " " << circle_list[0].size() << "\n"; 
		circle_list[i][0] = cx; // pixel locations
		circle_list[i][1] = cy; // pixel locations

		circletask[i].x_pixel = cx;
		circletask[i].y_pixel = cy;
		j++;
		//cvCircle(DisplayImage->image, cvPoint(cx,cy),1,CV_RGB(0,250,0),1); //draws circle points around robot

	}
//	DisplayImage->print();
	//cout << "size of cirlce list = " << circle_list.size()  << " " << circle_list[0].size() << "\n"; 
//return circle_list;
	// cout << resolution << " " << temprad << "\n";

}



void CreateTasks::ChoosePossibleLocationList(vector<vector<mapvar>> my_map, int x_size, int y_size, int n){
	
	//step 1: Transform bound list to value Map point 
	//step 2: make sure no repeating value locations exist. 	
	//Step 3: grab the value at those points and obstacle

	//cout << "Image width and hight = " << DisplayImage->image->width << " " << DisplayImage->image->height << endl;
	//cout << "xsize = " << x_size << " ysize = " << y_size << endl; // should be 16 19 
	for(unsigned int i = 0; i < circletask.size();i++){

		circletask[i].x_index =  circletask[i].x_pixel / 20;
		circletask[i].y_index =  circletask[i].y_pixel / 20; // this does not allow the function to pick a point outside of the map bound.
		

		// make sure the tasks created are within the map 
		if(circletask[i].x_index < 0){
			circletask[i].x_index = 0;
		}
		if(circletask[i].x_index > (DisplayImage->ReturnXLimit()-1)){
			circletask[i].x_index = (DisplayImage->ReturnXLimit()-1);
		}
		if(circletask[i].y_index < 0){
			circletask[i].y_index = 0;
		}
		if(circletask[i].y_index > (DisplayImage->ReturnYLimit()-1)){
			circletask[i].y_index = (DisplayImage->ReturnYLimit()-1);
		}

		assert(circletask[i].x_index <= (DisplayImage->ReturnXLimit()-1));
		assert(circletask[i].y_index <= (DisplayImage->ReturnYLimit()-1));


	//	printf("CIRCLE TASK INDEX VALUES = %d %d\n",circletask[i].x_index,circletask[i].y_index);
		//if this block is considered an obstacle dont compute anything, move to next interation 
		if(CTGridMap->ReturnIsObstacle(circletask[i].x_index,circletask[i].y_index) == true){
			continue;
		}	
		

		//cout << "x index " << circletask[i].x_index << " = " << circletask[i].x_pixel  << " / " << x_size <<endl;
		//cout << "y index " << circletask[i].y_index << " = " << circletask[i].y_pixel  << " / " << y_size <<endl;
		
		
		circletask[i].updatecreationtime(circletask[i].createdtime_tm);	// Update the creation time of this task (TIME.H OBJECT)
		circletask[i].UpdateCreateTime(); // set the time to now for this task (ARTIME OBJECT)
		//cout << "creation time = " << circletask[i].time_hour << " " << circletask[i].time_min << " " << circletask[i].time_sec << "\n";
		circletask[i].dist = sqrt(pow(my_map[circletask[i].y_index][circletask[i].x_index].x_global - myRobot->getX(),2)+ pow(my_map[circletask[i].y_index][circletask[i].x_index].y_global - myRobot->getY(),2));
		circletask[i].cost = 25 ; //temporary number // was initially 5
		circletask[i].utility = (circletask[i].dist*(0.00328) + circletask[i].cost)/my_map[circletask[i].y_index][circletask[i].x_index].value;
	} // end of for loop
	
	if(mySelfTasks.size() > 1){
		mySelfTasks.clear();
	}

	mySelfTasks = GrabMax(circletask, n);

	for(unsigned int i = 0; i < mySelfTasks.size() ; i++){ // fill in robot creator string
		mySelfTasks[i].robotcreator = selfrobot;
		cout << "task created = " << mySelfTasks[i].x_index << " " << mySelfTasks[i].y_index << endl;
//		DisplayImage->DrawThisLocationBLUE(CTGridMap->ReturnXPixel(mySelfTasks[i].x_index, mySelfTasks[i].y_index), CTGridMap->ReturnYPixel(mySelfTasks[i].x_index, mySelfTasks[i].y_index));
//		DisplayImage->DrawThisLocationRED(mySelfTasks[i].x_pixel,mySelfTasks[i].y_pixel);
		}


};


vector<tasks> CreateTasks::GrabMax(vector<tasks> list , int n){
	double max;
	int counter, max_index;
	max_index = 0;
	counter = 1;
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

vector<tasks> CreateTasks::getSelfCretedTasks(){
	//i can probably assert thatt it should have some size and not be zero
return mySelfTasks;
}

void CreateTasks::DEBUG_printSelfCreatedTasks(){
	for(unsigned int i = 0; i < mySelfTasks.size(); i++){
		//printf("task created at %d %d\n", mySelfTasks[i].x_index ,mySelfTasks[i].y_index);
	}

}