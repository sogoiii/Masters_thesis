#include "GridMap.h"

//using namespace std;

GridMap::GridMap(myDisplayImage *DisplayImage){
	GridDisplayImage = DisplayImage;

	//cout << " GOING TO OPEN MY TESTINCLASS FILE!!!\n\n\n\n\n";
	//myfile.open("./GridInformation.txt");
	directory = "./MyGridInformation.txt";
	myfile.open( directory.c_str() );
	if(myfile.fail()){
		cout << directory << " failed to open" << endl;
		//exit(-1);
	}

	directory2 = "./TotalGridsSeen.txt";
	myfile2.open(directory2.c_str());
	if(myfile2.fail()){
		cout << directory2 << " failed to open" << endl;
	}




	X_limit = 0;// makes sure no objects above this are called
	Y_limit = 0;//makes sure no objecsts above this are called 

	totseenfirstbool = false;
	ContinueThread = true;

	ReduceValueParam = 10; //set how much value is removed from the grid

}
GridMap::~GridMap(){
myfile.close();
myfile2.close();
}

void GridMap::task(){ //draw the display image

	GridDisplayImage->FOVDipslayThread(); // changes display, and computes fov index seen
	UpdateSeenFOVIndex(GridDisplayImage->ReturnFovLocationsSeen()); // reduces the value of seen locations
//	UpdateTotalLocationsSeenList(GridDisplayImage->ReturnFovLocationsSeen());//tot grid areas seen


	if(ContinueThread == true){
		go();
	}
	else if(ContinueThread == false){
		//exit out of this task function
		//printf("I have stopped the Grid Map Thread!!!!!\n");
	}
	else{
		printf("ERROR IN GRIDMAP TASK CONTINUETHREAD CONDITION\n");
	}
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
}

void GridMap::StartRecurrentTask(){
	ContinueThread = true;
	go();
}

void GridMap::StopRecurrentTask(){
	ContinueThread = false;
	printf("Stopped display Image\n");
}


void GridMap::WrappedFovDisplay(){
		
	ContinueThread = true;
	printf("STARTED THE DISPLAY THREAD WILL CONTINUE FOR A WHILE!\n");
	while(ContinueThread == true){

		if(ContinueThread == true){
			GridDisplayImage->FOVDipslayThread(); // changes display, and computes fov index seen
			UpdateSeenFOVIndex(GridDisplayImage->ReturnFovLocationsSeen()); // reduces the value of seen locations
		//	UpdateTotalLocationsSeenList(GridDisplayImage->ReturnFovLocationsSeen());//tot grid areas seen
		}
		else if(ContinueThread == false){
			//exit out of this task function
			printf("I have stopped the Grid Map Thread!!!!!\n");
			break;
		}
		else{
			printf("ERROR IN GRIDMAP TASK CONTINUETHREAD CONDITION\n");
			break;
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(500));

	}//end of while loop

}



int GridMap::GetValue(int x, int y){
return MyGridMap[y][x].value;
}

void GridMap::ChooseMinValue(int x, int y, int value){
	if(MyGridMap[y][x].value <= value){
		//leave MyGridMap[y][x].value alone since it is smaller
	}
	else if(MyGridMap[y][x].value > value){
		MyGridMap[y][x].value = value;
	}
	else{printf("ERROR IN CHOOSMINVALUE GRID MAP !!!!!\n");}

}

void  GridMap::CreateGridMap(int x_length, int y_length){ // creates the empty map
	mapvar var;
	vector<mapvar> temp;
	for(int j=0;j < x_length; j++){ // j_index // the inputs should be 16 and 19 later on 
		temp.push_back(var);
	}
	for(int i=0;i< y_length;i++){ // i_index
		MyGridMap.push_back(temp);
	}
	ComputeGridObstacles();

	X_limit = x_length;
	Y_limit = y_length;
}

void GridMap::ComputeGridObstacles(){ // Collects and creates most of the map data
//	int xsize;
	int	w_start;
	int	w_end;
	int	h_start;
	int	h_end;
	int i_index;
	int j_index;
	myfile << "Size of image (width, height) " << GridDisplayImage->image->width << " " << GridDisplayImage->image->height << endl;
	for(int i=0; i<=GridDisplayImage->image->height - 15; i+=20){	//subtraction done to fit map::change with every map
		for(int j=0; j<=GridDisplayImage->image->width - 19; j+=20) {//subtraction done to fit map::change with every map
			h_start = i;
			h_end = i+20;
			w_start = j;
			w_end = j+20;
			i_index = i/20;//this 20 is the i+=20
			j_index = j/20;
			//printf("[w_start,w_end,h_start,h_end] = [%d, %d, %d, %d] \n" ,w_start,w_end,h_start,h_end);
			
			//myfile << "[w_start,w_end,h_start,h_end] = [" << w_start << ","<<w_end << "," << h_start << "," << h_end<< "]\n";
				
			CvScalar c;
			//MyGridMap[i_index][j_index].obstacle = true;
			MyGridMap[i_index][j_index].x_index = j_index;			// index location
			MyGridMap[i_index][j_index].y_index = i_index;			//index location
			MyGridMap[i_index][j_index].x_pixel = (w_start + w_end)/2; // these are pixel points
			MyGridMap[i_index][j_index].y_pixel = (h_start + h_end)/2; // these are pixel points 
			MyGridMap[i_index][j_index].x_global = GridDisplayImage->Blx + (MyGridMap[i_index][j_index].x_pixel)*(GridDisplayImage->resolution);
			MyGridMap[i_index][j_index].y_global = GridDisplayImage->Bly + (MyGridMap[i_index][j_index].y_pixel)*(GridDisplayImage->resolution);
		//	cout << "Index " << i_index << " " << j_index << " global = " << MyGridMap[i_index][j_index].x_global << " " << MyGridMap[i_index][j_index].y_global << endl;
			//myfile << " Index (" << j_index << "," << i_index << ") = pixel (" <<  MyGridMap[i_index][j_index].x_pixel << "," << MyGridMap[i_index][j_index].y_pixel << ") =";
			//myfile << " Position (" << MyGridMap[i_index][j_index].x_global << "," << MyGridMap[i_index][j_index].y_global << ")" << endl;
			
			//	cout << "Index point = " << MyGridMap[i_index][j_index].x_index << " " << MyGridMap[i_index][j_index].y_index;	
		//	cout << " Global points = " << MyGridMap[i_index][j_index].x_global << " " << MyGridMap[i_index][j_index].y_global << "\n";
		
			//MyGridMap[i_index][j_index].update_time(&MyGridMap[i_index][j_index].last_update_time);
		
			//cvCircle(GridDisplayImage->image, cvPoint(10,210),1,CV_RGB(0,250,0),1);

			//the line below draws the center point for every box 
		//	GridDisplayImage->DrawThisLocationGREEN(MyGridMap[i_index][j_index].x_pixel,MyGridMap[i_index][j_index].y_pixel);
	
			//Check how much of each grid has an obstacle inside
			for (int p = h_start; p < h_end; ++p){			
				for (int q = w_start; q < w_end; ++q){
					c = cvGetAt(GridDisplayImage->image,p,q);
					if (c.val[0] == 255 && c.val[1] == 255 && c.val[2] == 255){				
						MyGridMap[i_index][j_index].obstacle++;	
					} // end of if
				}	//end of q for loop
			}//end of p for loop
			MyGridMap[i_index][j_index].obstacle_percentage = (MyGridMap[i_index][j_index].obstacle)/400.0;
			if(MyGridMap[i_index][j_index].obstacle_percentage > .3){
				MyGridMap[i_index][j_index].isObstacle = true;
			}
		}
	}


	//GridDisplayImage->DEBUG_drawthesepoints(); //these are red points that are the center of each grid box
}

int GridMap::CompareValue(int x, int y , int cvalue){
	//returns the smallest value
	//cout << "Compare value locations = " << x << " " << y << "\n";
	//return cvalue;
	
	/*
if((*ref_my_map)[y][x].value > cvalue)
	return cvalue;
else if((*ref_my_map)[y][x].value < cvalue)
	return (*ref_my_map)[y][x].value;
else // if they are equal 
	return cvalue;*/
	
	return cvalue;
}

int GridMap::ReturnXglobal(int x, int y){

//the double if makes sure the robot searches for objects that exist

	assert(x <=(X_limit-1));
	assert(y <=(Y_limit-1));
/*	if(x >= X_limit){
		x = 29;// (X_limit -1);
	}
	if(y >= Y_limit){
		y = 23; //(Y_limit -1);
	}*/
	//printf("WILL I CRASH HERE RETURNXGLOBAL %d %d\n",x,y);
	return MyGridMap[y][x].x_global; // should be [y][x] //else 
}

int GridMap::ReturnYglobal(int x, int y){
//the double if makes sure the robot searches for objects that exist

	assert(x <=(X_limit-1));
	assert(y <=(Y_limit-1));
/*	if(x >= X_limit){
		x = 29;//(X_limit -1);
	}
	if(y >= Y_limit){
		y = 23;//(Y_limit -1);
	}*/
	//printf("WILL I CRASH  HERE RETURNYGLOBAL  %d %d\n", x,y);
	return MyGridMap[y][x].y_global; // should be [y][x] //else 
}


bool GridMap::CheckBidVectorSize(int x, int y){
	if(MyGridMap[y][x].bidvector.size() > 1){
		return false;
	}
	else if(MyGridMap[y][x].bidvector.size() <= 1){
		return true;
	}
	else{
		cout << "ERROR IN CHECK BID VECTOR SIZE IN GRIDMAP!!\n\n\n\n\n";
	}
}

int GridMap::ReturnBidVectorSize(int x, int y){
	return MyGridMap[y][x].bidvector.size();	
	}

void GridMap::AddToBidVector(int x, int y, int bidvalue){
	MyGridMap[y][x].bidvector.push_back(bidvalue); 
}

int GridMap::ReturnBidVectorBid(int x, int y, int n){
	return MyGridMap[y][x].bidvector[n];
}

void GridMap::RemoveBidFromBidVector(int x, int y, int n){

	/*for(int i = 0; i < MyGridMap[y][x].bidvector.size();i++){
		printf("Bids before = %d\n",MyGridMap[y][x].bidvector[i]); // debugging
	}*/

	//printf("Want to delet bid = %d index %d\n",MyGridMap[y][x].bidvector[n], n);
	MyGridMap[y][x].bidvector.erase(MyGridMap[y][x].bidvector.begin()+n); // remove n'th item

   /*for(int i = 0; i < MyGridMap[y][x].bidvector.size();i++){
		printf("Bids after = %d\n",MyGridMap[y][x].bidvector[i]); // debugging
	}*/
}

void GridMap::AddToBidVectorBidder(int x, int y, string bidder){
	MyGridMap[y][x].bidvectorbidder.push_back(bidder);
}

string GridMap::ReturnBidVectorBidder(int x, int y, int n){
	return MyGridMap[y][x].bidvectorbidder[n];
}

void GridMap::RemoveBidVectorBidder(int x, int y, int n){

	/*for(int i = 0; i < MyGridMap[y][x].bidvectorbidder.size();i++){
		printf("Bidder before = %s\n",MyGridMap[y][x].bidvectorbidder[i].c_str()); // debugging
	}*/

	//printf("Want to delet %s index %d\n",MyGridMap[y][x].bidvectorbidder[n], n);
	MyGridMap[y][x].bidvectorbidder.erase(MyGridMap[y][x].bidvectorbidder.begin()+n); // remvo n'th item

	/*for(int i = 0; i < MyGridMap[y][x].bidvectorbidder.size();i++){
		printf("Bidder after = %s\n",MyGridMap[y][x].bidvectorbidder[i].c_str()); // debugging
	}*/
}

int GridMap::ReturnYPixel(int x, int y){
	return MyGridMap[y][x].y_pixel;
}

int GridMap::ReturnXPixel(int x, int y){
	return MyGridMap[y][x].x_pixel;
}

bool GridMap::ReturnIsObstacle(int x, int y){
	return MyGridMap[y][x].isObstacle;
}




vector<vector<int>> GridMap::Get8neighbors(int x, int y){

	int first;
	vector<int> hold;
	vector<vector<int>> neighbors;
	for(int j=0; j <= 1;j++){ //this a vector of size 2
		hold.push_back(first);
	}
	neighbors.push_back(hold); // vector of vector size 1 by 2
	int count = 0;

	if(x == (X_limit-1)){x = 33;}//change with every map
	if(y == (Y_limit-1)){y = 34;}//change with every map

	if(MyGridMap[y][x+1].obstacle == false){ //right
		neighbors[count][0] = x+1;
		neighbors[count][1] = y;
		count++;
		neighbors.push_back(hold);
	}
	if(MyGridMap[y+1][x+1].obstacle == false){ //top right
		neighbors[count][0] = x+1;
		neighbors[count][1] = y+1;
		count++;
		neighbors.push_back(hold);
	}
	if(MyGridMap[y+1][x].obstacle == false){ // top
		neighbors[count][0] = x;
		neighbors[count][1] = y+1;
		count++;
		neighbors.push_back(hold);
	}
	if(MyGridMap[y+1][x-1].obstacle == false){ // top left
		neighbors[count][0] = x-1;
		neighbors[count][1] = y+1;
		count++;
		neighbors.push_back(hold);
	}
	if(MyGridMap[y][x-1].obstacle == false){ // left
		neighbors[count][0] = x-1;
		neighbors[count][1] = y;
		count++;
		neighbors.push_back(hold);
	}
	if(MyGridMap[y-1][x-1].obstacle == false){ // bottom left
		neighbors[count][0] = x-1;
		neighbors[count][1] = y-1;
		count++;
		neighbors.push_back(hold);
	}
	if(MyGridMap[y-1][x].obstacle == false){ // bottom
		neighbors[count][0] = x;
		neighbors[count][1] = y-1;
		count++;
		neighbors.push_back(hold);
	}	
	if(MyGridMap[y-1][x+1].obstacle == false){ // bottom right
		neighbors[count][0] = x+1;
		neighbors[count][1] = y-1;
		count++;
		neighbors.push_back(hold);
	}


	int xt = 0;
	int yt = 0;
	for(int i = 0; i < count;i++){
		xt = ReturnXglobal(neighbors[i][0], neighbors[i][1]);
		yt = ReturnYglobal(neighbors[i][0], neighbors[i][1]);
		neighbors[i][0] = xt;
		neighbors[i][1] = yt;
	//	printf("A neighbor = %d %d\n", neighbors[i][0], neighbors[i][1]);
	}


return neighbors;
}


void GridMap::BeginUpdateMapValues(){ // this must be called in main
boost::thread* UpdateGridMapValuesThread = new boost::thread(boost::bind(&GridMap::UpdateGridMapValues, this));
}

void GridMap::UpdateGridMapValues(){
	//this function updates the value of the whole map
	while(1){
		boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
		for(unsigned int i = 0; i < MyGridMap.size();i++){ // i == y axis
			for(unsigned int j = 0; j < MyGridMap[0].size();j++){ /// j == x axis
				MyGridMap[i][j].value = MyGridMap[i][j].value + 3; // changed 
			}//end of j loop
		}//end of i loop
	}//end of while


}

void GridMap::DEBUG_PrintDisplayMap(){
	for(unsigned int i = 0; i < MyGridMap.size();i++){ // i == y axis
		for(unsigned int j = 0; j < MyGridMap[0].size();j++){ /// j == x axis
			myfile << "loc " << MyGridMap[i][j].x_index << " " << MyGridMap[i][j].y_index;
			myfile << " value = " << MyGridMap[i][j].value << endl;
		}//end of j loop
	}//end of i loop
}

void GridMap::UpdateSeenFOVIndex(deque<FovStruct> fovlocs){

	for(unsigned int i = 0; i < fovlocs.size(); i++){
		int x = fovlocs[i].x;
		int y = fovlocs[i].y;
		double percent = fovlocs[i].percentage;
		//printf("Changing the value of %d %d\n",x,y);
		MyGridMap[y][x].value = MyGridMap[y][x].value - ReduceValueParam*percent;
	}
}

void GridMap::UpdateTotalLocationsSeenList(deque<FovStruct> seen){

//	deque<FovStruct> seen;
	//seen = GridDisplayImage->ReturnFovLocationsSeen();

	bool exists;
	int startsize;
	if(totseenfirstbool == false){ //should only happen once
		totseenfirstbool = true;
		TotSeen = seen;
		startsize = TotSeen.size();
	}
	else{
		startsize = TotSeen.size();
		for(unsigned i = 0; i < seen.size(); i++){
			for(unsigned j = 0; j < TotSeen.size(); j++){
				if((seen[i].x == TotSeen[j].x) && (seen[i].y == TotSeen[j].y)){
					//do nothing
					exists = true;
					break;
				}
				else{
					exists = false;
				}
			}//end of j loop

			if(exists == false){
				TotSeen.push_back(seen[i]);
			}
			else if (exists == true){
				//measns seen[i] is in TotSeen
			}
		}//end of i loop
	}// end of testseenfirstbool else

	if(startsize != TotSeen.size()){
		myfile2 << "Total Grid Locs Seen = " << TotSeen.size() << endl;
	}

}