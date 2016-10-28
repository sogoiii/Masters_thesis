#include "myDisplayImage.h"
#include <vector>
using namespace std;

myDisplayImage::myDisplayImage()
{
	scalarOut = 60;//not sure what 5 feet is but need to figure it out
	coneAngle = 40;//degrees
	resolution = 0;
	offsetX = offsetY = 0;
	boundary = 100;
	myrow = 0;
	mycol = 0;
	DrawFovFirst = false;
	ContinueThread = true;
	XLimit = 0;
	YLimit = 0;

	directory = "./FOVIndexLocations.txt";
	myfile.open( directory.c_str() );
	if(myfile.fail()){
		cout << directory << " failed to open" << endl;
	}

}

myDisplayImage::~myDisplayImage(){
	//fout.close();
	cvReleaseImage(&image);
}

void myDisplayImage::AddArRobot(ArRobot *robot){
	myrobot = robot;
	//printf("WILL START MY THREAD NOW\n");
	//go();
}

int myDisplayImage::ReturnXLimit(){
	return XLimit;
	
}
int myDisplayImage::ReturnYLimit(){
	return YLimit;
}


void myDisplayImage::task(){
	//have the thread wait
	//when coniditon is signals move down

	int x = myrobot->getX();
	int y = myrobot->getY();
	double theta = myrobot->getTh();


	UpdateRobotFOVonDisplay(x,y,theta); 
	FieldofViewPixels(XLimit,YLimit);
	if(ContinueThread == true){
	go();
	}
	else if(ContinueThread == false){
		//exit out of this task function
		printf("I have stopped computing FOV Pixels\n\n\n\n\n\n\n");
	}
	else{
		printf("ERROR IN MYDISPLAY TASK CONTINUETHREAD CONDITION\n");
	}
}
void myDisplayImage::StartRecurrentTask(){
	//printf("I WILL START MY THREAD NOW!!!\n");
	ContinueThread = true;
	go();
}
void myDisplayImage::StopRecurrentTask(){
	ContinueThread = false;
	printf("Stopping Display image\n");
}

void myDisplayImage::FOVDipslayThread(){//may no longer use...depends

	//while(ContinueThread == true){
		UpdateRobotFOVonDisplay(myrobot->getX(),myrobot->getY(),myrobot->getTh()); 
		FieldofViewPixels(XLimit,YLimit);
	//	boost::this_thread::sleep(boost::posix_time::milliseconds(500));
	//}

}

void myDisplayImage::print(){
							
				
	/*if(!image){//need valid image to display
		cout << "NO IMAGE\nNO IMAGE\nNO IMAGE\n";
		return;
	}*/
	
	//datalock.tryLock();

	cvNamedWindow( "Robot Overview", 1) ;
	cvMoveWindow("Robot Overview",441,0); // 1152 - xsize of image 
	cvShowImage( "Robot Overview", image );
	cvWaitKey(1);
	//datalock.unlock();
		
}

void myDisplayImage::map_parse(string location){

	int tlx, tly;
	int brx, bry;
	int datapoint;
	int lineNum;

	//myDisplayImage graph;

	//assuming we got some arguments
	string line;
	ifstream myfile(location.c_str() );
	//cout << "trying to open file\n";

		if (myfile.is_open()){

		//cout << "able to open file\n";
		//datalock.tryLock();
		//initialization

		//for TL
		myfile >> line;
		while(line != "MinPos:"){
			myfile >> line;
		}
		myfile >> tlx >> bry;
		Blx = tlx;
		Bly = bry;
		//cout << "min pos " << tlx << " " << bry << endl;
		
		//for BR
		myfile >> line;
		while(line != "MaxPos:"){
			myfile >> line;
		}
		myfile >> brx >> tly;
		Trx = brx;
		Try = tly;
		//cout << "max pos " << brx << " " << tly << endl;

		//for NumPoint
		myfile >> line;
		while(line != "NumPoints:"){
			myfile >> line;
		}
		myfile >> datapoint ;

		//for Resolution
		myfile >> line;
		while(line != "Resolution:"){
			myfile >> line;
		}
		myfile >> resolution ;
		//cout << "\nResolution: " << resolution << endl;

		//for lineNum
		myfile >> line;
		while(line != "NumLines:"){
			myfile >> line;
		}
		myfile >> lineNum;

		tlx = -4170; //most min bottom position: bottom left x
		bry = -10957; // most min bottom position: bottom left y

		brx = 10048; // most max position: top right x
		tly = 3558; // most max position: top right y
		//need to intialize our array
		offsetX = tlx/resolution; //original
		offsetY = bry/resolution;// original


		
		int XPixelLimit = (brx - tlx)/resolution + 1; //image->width
		int YPixelLimit = (tly - bry)/resolution + 1; //image->height
		printf("SIZE OF IMAGE = %d %d\n", XPixelLimit, YPixelLimit);
		CvSize size = cvSize(XPixelLimit,YPixelLimit);
		XLimit = XPixelLimit/20; // xsize of value map
		YLimit = YPixelLimit/20; // ysize of value map
		//printf("Value Map Limites = [%d %d]\n",XLimit, YLimit);
		image = cvCreateImage(size,8,4);
		image->origin = 1;
	
		
		//initializing values to 0
		for(int i = 0; i < image->width; i++){
			for(int j = 0; j < image->height; j++){	
				CvScalar px = cvGetAt(image,j,i);
				px.val[0] = 0; //<- blue
				px.val[1] = 0; //<- green
				px.val[2] = 0; //<- red
				cvSetAt(image, px ,j,i);
			}
		}//end initialization

		myrow = (brx - tlx )/resolution+1;
		mycol = (tly - bry)/resolution +1;
	


		//for forbidden areas
		myfile >> line;
		while(line!= "Cairn:"){ // if not cairin line, get next word
			myfile >> line;
		}

		//Just found cairin
		while(line == "Cairn:"){//when i find cairn do the following
			for(int i = 1;i<=7;i++){ // cycles through the line
				myfile >> line;
				//cout << "the next word is " << line << "\n";
			}			
			int minx, miny, maxx, maxy;
			myfile >> minx >> miny >> maxx >> maxy;
			//cout << "min x = " << minx << "\n";
			//cout << "max x = " << maxx << "\n";
			//cout << "min y = " << miny << "\n";
			//cout << "max y = " << maxy << "\n";
		//	cout << minx << "\n" << miny << "\n" << maxx << "\n" << maxy << "\n";
		setobstaclepoints(minx, miny, maxx, maxy, tlx, bry); // tlx = blx , bly = bry 
		myfile >> line; //this should be cairin
		} // end of cairin lines

		//cout << "End of carin lines\n";
		//for data
		myfile >> line;
		while(line != "DATA"){
		myfile >> line;
		}
		signed int xblah,yblah;

		while (! myfile.eof() )	{ //
			
			myfile >> xblah >> yblah;

			//cout << "X: " << x << endl;
			//cout << "Y: " << y << endl;
			//normalize the points
			//cout << "Map-parse -x: " << xblah << ", " << tlx << endl;
			//cout << "Map-parse -y: " << yblah << ", " << bry << endl;

			xblah = ( xblah- tlx )/resolution ;
			yblah = ( yblah- bry )/resolution ;

			CvScalar px = cvGetAt(image,yblah,xblah);
			
			px.val[0] = INVALIDCELL; //<- blue
			px.val[1] = INVALIDCELL; //<- green
			px.val[2] = INVALIDCELL; //<- red
			cvSetAt(image, px , yblah,xblah);// throws the pixel back into the image to see
		}
		
		myfile.close();

		
	}
	else{//unable to open file so throw back empty whatever
	}

}


void myDisplayImage::setobstaclepoints(int minx, int miny, int maxx, int maxy, int tlx, int bry){
	//input should be (small, big, small, big); // else error is in the double for loop
	//makes obstacles white on dislay image
	minx = ( minx - tlx )/resolution; 
	miny = ( miny - bry )/resolution; 
	maxx = ( maxx - tlx )/resolution; 
	maxy = ( maxy - bry )/resolution; 


   // cout << "making my obstacle!!!!!!";
	for(int i = maxy; i >= miny; i--){ 
		for(int j = minx; j <= maxx; j++){ 
			//cout << "(" << i << "," << j << ")" << "\n";
			CvScalar py = cvGetAt(image,i,j); // may have to be (j , i)
			py.val[0] = 255; //<- blue
			py.val[1] = 255; //<- green
			py.val[2] = 255; //<- red
			cvSetAt(image, py , i,j);// throws the pixel back into the image to see
		} // end of j loop
	} // end of i loop

}// of of obstacle function


void myDisplayImage::DrawFieldOfView(int robot_X,int robot_Y, double robot_theta){

	    
		curPos.x = robot_X /resolution - offsetX;
		curPos.y = robot_Y /resolution - offsetY;
		curPos.angle = robot_theta;

		leg1.x = (double)curPos.x + scalarOut * (cos((robot_theta - coneAngle/2)* PI /180));
		leg1.y = (double)curPos.y + scalarOut * (sin((robot_theta - coneAngle/2)* PI /180));

		leg2.x = (double)curPos.x + scalarOut * (cos((robot_theta + coneAngle/2)* PI /180));
		leg2.y = (double)curPos.y + scalarOut * (sin((robot_theta + coneAngle/2)* PI /180));

		cvLine(image,cvPoint(curPos.x,curPos.y),cvPoint(leg1.x,leg1.y),cvScalar(100,100,100),1,8,0);
		cvLine(image,cvPoint(curPos.x,curPos.y),cvPoint(leg2.x,leg2.y),cvScalar(100,100,100),1,8,0);
		cvLine(image,cvPoint(leg1.x,leg1.y),cvPoint(leg2.x,leg2.y),cvScalar(100,100,100),1,8,0);
		
		int avgx = (curPos.x + leg1.x + leg2.x)/3;
		int avgy = (curPos.y + leg1.y + leg2.y)/3;
	
		int temp_count;
		temp_count = 0;
		floodfill(CV_RGB(120,50,0),avgx,avgy,temp_count);
		
		//print();
/*
		int count;
		count = 0;
		
		removefloodfill(CV_RGB(0,0,0), avgx,avgy, tempv, count);

		datalock.tryLock();
		PixelFovList = tempv;
		datalock.unlock();

		cvLine(image,cvPoint(curPos.x,curPos.y),cvPoint(leg1.x,leg1.y),cvScalar(0,0,0),1,8,0);
		cvLine(image,cvPoint(curPos.x,curPos.y),cvPoint(leg2.x,leg2.y),cvScalar(0,0,0),1,8,0);
		cvLine(image,cvPoint(leg1.x,leg1.y),cvPoint(leg2.x,leg2.y),cvScalar(0,0,0),1,8,0);
			*/
	//print();
}

void myDisplayImage::RemoveFieldOfView(int robot_X,int robot_Y, double robot_theta){

		curPos.x = robot_X /resolution - offsetX;
		curPos.y = robot_Y /resolution - offsetY;
		curPos.angle = robot_theta;

		leg1.x = (double)curPos.x + scalarOut * (cos((robot_theta - coneAngle/2)* PI /180));
		leg1.y = (double)curPos.y + scalarOut * (sin((robot_theta - coneAngle/2)* PI /180));

		leg2.x = (double)curPos.x + scalarOut * (cos((robot_theta + coneAngle/2)* PI /180));
		leg2.y = (double)curPos.y + scalarOut * (sin((robot_theta + coneAngle/2)* PI /180));

		int avgx = (curPos.x + leg1.x + leg2.x)/3;
		int avgy = (curPos.y + leg1.y + leg2.y)/3;

		int count;
		count = 0;
		
		removefloodfill(CV_RGB(0,0,0), avgx,avgy, tempv, count);
		//myfile << "Size of Tempv = " << tempv.size() << " " << tempv[0].size() << endl;
		//printf("Size of Temp v = %d %d \n", tempv.size(), tempv[0].size());
		datalock.tryLock();
		PixelFovList = tempv;
		tempv.clear();
		datalock.unlock();

		cvLine(image,cvPoint(curPos.x,curPos.y),cvPoint(leg1.x,leg1.y),cvScalar(0,0,0),1,8,0);
		cvLine(image,cvPoint(curPos.x,curPos.y),cvPoint(leg2.x,leg2.y),cvScalar(0,0,0),1,8,0);
		cvLine(image,cvPoint(leg1.x,leg1.y),cvPoint(leg2.x,leg2.y),cvScalar(0,0,0),1,8,0);

		//print();

}






void myDisplayImage::floodfill(CvScalar color, int x,int y, int& tcount){
	CvScalar px = cvGetAt(image,y,x);

	if (px.val[0] == boundary || 
		px.val[0] == color.val[0]  &&
		px.val[1] == color.val[1]  &&
		px.val[2] == color.val[2] )
		return;

	if(y>=0 && y < image->height){//making sure within y scope
		if(x>=0 && x < image->width){//making sure within x scope
			cvSetAt(image,color,y,x);// throws the pixel back into the image to see
		//	cout << "temp count" << tcount << "\n";
			tcount++;
		}
	}

	floodfill(color, x-1, y,tcount);
	floodfill(color, x+1, y,tcount);
	floodfill(color, x, y-1,tcount);
	floodfill(color, x, y+1,tcount);
}

void myDisplayImage::removefloodfill(CvScalar color, int x,int y, vector<vector<int>>& fovlist, int& count){
		vector<int> temp;
		int var_int;
		for(int i=0;i < 2; i++){ // j_index
			temp.push_back(var_int);
		}
	
	CvScalar px = cvGetAt(image,y,x);
	
	if (px.val[0] == boundary || 
		px.val[0] == color.val[0]  &&
		px.val[1] == color.val[1]  &&
		px.val[2] == color.val[2] )
		return;

	if(y >= 0 && y < image->height){//making sure within y scope
		if(x >= 0 && x < image->width){//making sure within x scope
			cvSetAt(image,color,y,x);// throws the pixel back into the image to see
			fovlist.push_back(temp); // add a 1x2
			 fovlist[count][0] = x; // place x 
			 fovlist[count][1] = y; // place y
			 //myfile << "Pixel Locations = " << x << " " << y << endl; // what pixels are in the fov
		 	count++;
		}
	}

	removefloodfill(color, x-1, y, fovlist, count);
	removefloodfill(color, x+1, y, fovlist, count);
	removefloodfill(color, x, y-1, fovlist, count);
	removefloodfill(color, x, y+1, fovlist, count);
}


//This function gets the list of pixels that the FOV sees and returns the list occupancy grids
//in the value map. The output is the list of occupancy grids that need to be updated.

void myDisplayImage::FieldofViewPixels(int x_size, int y_size){
//These are the pixels that are seens by the robot. These are then translated to 
//value map locations and these locations need to be updated. 

	ValueFovList.clear();
	DoubleFovList.clear();
	double var = 0;
	deque<double> temp;

	//printf("PixelFOVList size = (%d %d)\n", PixelFovList.size(), PixelFovList[0].size());

	if(PixelFovList.size() == 0){
		return;
	}
	for(unsigned int j=0;j < PixelFovList[0].size(); j++){ // j_index // the inputs should be 16 and 19 later on 
		temp.push_back(var);
	}
	for(unsigned int i=0;i< PixelFovList.size();i++){ // i_index
		ValueFovList.push_back(temp);
		DoubleFovList.push_back(temp);
	}

/*testing
		SingleFovList.clear();
	int var2;
	for(int i =0; i < PixelFovList.size();i++){
		SingleFovList.push_back(var2);
	}
//end testing*/

	for(unsigned int i = 0; i < PixelFovList.size();i++){	
		ValueFovList[i][0] = PixelFovList[i][0]/x_size; // 
		ValueFovList[i][1] = PixelFovList[i][1]/y_size; // 
	
		//stringstream converter; // create stringstream
		//converter << ValueFovList[i][0] << ValueFovList[i][1]; //combine two integer numbers into one string number
		//SingleFovList[i] = atoi(converter.str().c_str());

		double num1 = myLog(ValueFovList[i][0],(XLimit-1)); 
		double num2 = myLog(ValueFovList[i][1],(YLimit-1));
		double sum = num1 + num2;

		DoubleFovList[i][0] = sum; 
		DoubleFovList[i][1] = i;
		//myfile << "ValueFovList = " << ValueFovList[i][0] << " " << ValueFovList[i][1];
		//myfile << "  DoubleFov List = (" << DoubleFovList[i][0] << " , " << DoubleFovList[i][1] << ")" << endl;	
		//myfile << " SingleFovList = " << SingleFovList[i] << endl;
	}

//	printf("Going to merge sort\n");
//	deque<deque<double>> OriginalUnsortedList = DoubleFovList;
	merge_sort(DoubleFovList);
/*	printf("Done using MergeSort\n");
/*	myfile << "After sort \n";
	for(int i = 0; i < DoubleFovList.size(); i++){
		myfile << "Unsorted list = " << OriginalUnsortedList[i][0] << " and " << OriginalUnsortedList[i][1];
		myfile << " Sorted list = " << DoubleFovList[i][0] << " and " << DoubleFovList[i][1] << endl;
	}*/

	double compareint, currentint;
	int count = 0; 
	compareint = DoubleFovList[0][0];
	IndexLocs.clear();
	IndexPercents.clear();
	LocationFovList.clear();
	FovLocationsSeen.clear();

	for(unsigned int i = 0; i < DoubleFovList.size();i++){
		currentint = DoubleFovList[i][0];
		//myfile << i << " Comparing " << currentint << " with " << DoubleFovList[i];
		if(compareint != currentint){
			//myfile << "  The Number " << compareint << " appears " << count << endl; 
			IndexLocs.push_back(DoubleFovList[i-1][1]);
			IndexPercents.push_back(count);
			count = 1;
			compareint = currentint;
		}
		else if(currentint == compareint){
			count++;
			//myfile << " count = " << count << endl;
		}
		else{
			printf("ERROR IN FIELDOFVIEWPIXELS\n\n");
		}

		if(i == DoubleFovList.size() - 1){
			//myfile << "  The Number " << compareint << " appears " << count << endl; 
			IndexLocs.push_back(DoubleFovList[i-1][1]);
			IndexPercents.push_back(count);
		}

	}//end of for loop

	FovStruct tempstruct;
	for(unsigned int i = 0; i < IndexLocs.size();i++){
	//	double percentage;
		IndexPercents[i] = IndexPercents[i]/400;
		LocationFovList.push_back(ValueFovList[IndexLocs[i]]); // [0] = x , [1] = y
		myfile << "Grid location ("<< ValueFovList[IndexLocs[i]][0] << "," << ValueFovList[IndexLocs[i]][1];
		myfile << ") With Percetage " << IndexPercents[i] << endl;
		FovLocationsSeen.push_back(tempstruct);
		FovLocationsSeen[i].percentage = IndexPercents[i];
		FovLocationsSeen[i].x = (int)LocationFovList[i][0];
		FovLocationsSeen[i].y = (int)LocationFovList[i][1];
	}
		myfile << endl << endl;

}

deque<FovStruct> myDisplayImage::ReturnFovLocationsSeen(){
	return FovLocationsSeen;
}

deque<deque<double>> myDisplayImage::ReturnLocationFovList(){
	return LocationFovList;
}


void myDisplayImage::UpdateRobotFOVonDisplay(int x, int y, double theta){
	//draw the fov on the display image
	if(DrawFovFirst == true){
		RemoveFieldOfView(x, y,theta); //remove fov
		DrawFieldOfView(x, y, theta); // draw fov
		print(); // print to screen
	}
	else if(DrawFovFirst == false){
		DrawFieldOfView(x, y, theta); // draw fov
		print(); // print to screen
		RemoveFieldOfView(x, y,theta); //remove fov
		//DrawFovFirst = true; //my flag  this section will not be called again
	}
	else{
		printf("ERROR IN UPDATE ROBOT FOV ON DISPLAY FUNCTION!!\n");
	}
}

void myDisplayImage::DrawThisLocationGREEN(int x, int y){
cvCircle(image, cvPoint(x,y),1,CV_RGB(0,250,0),1);
//print();
}

void myDisplayImage::DrawThisLocationBLUE(int x, int y){
cvCircle(image, cvPoint(x,y),1,CV_RGB(0,0,250),1);
//print();
}

void myDisplayImage::DrawThisLocationRED(int x, int y){
cvCircle(image, cvPoint(x,y),1,CV_RGB(250,0,0),1);
//print();
}


void myDisplayImage::DEBUG_drawthesepoints(int x, int y){

cvCircle(image, cvPoint(x,y),1,CV_RGB(250,0,0),9);
//cvCircle(image, cvPoint(110,90),1,CV_RGB(250,0,0),1);
//cvCircle(image, cvPoint(110,110),1,CV_RGB(250,0,0),1);
//cvCircle(image, cvPoint(70,170),1,CV_RGB(250,0,0),1);
print();
}

		


//=======================================================================================
deque<deque<double>> myDisplayImage::mymerge(deque<deque<double>> &list1, deque<deque<double>> &list2){
	deque<deque<double>> result;
	//printf("Merging lists. Size = %d %d\n", list1.size(),list1[0].size());

	while(list1.size() > 0 || list2.size() > 0){ // size will return 1
		if (list1.size() > 0 && list2.size() > 0){
			if (list1[0].front() <= list2[0].front()){
				result.push_back(list1[0]);
				list1.pop_front();
			}
			else{
				result.push_back(list2[0]);
				list2.pop_front();
			}
		}
		else if(list1.size() > 0){
			result.push_back(list1[0]);
			list1.pop_front();
		}
		else if(list2.size() > 0){		
			result.push_back(list2[0]);
			list2.pop_front();
		}
	}

return result;
}
deque <deque<double>> myDisplayImage::merge_split(deque<deque<double>> list){
	int size = list.size(); // will have to switch
	if (size == 1){
		return list;
	}
	
	int half = size/2;
	deque<deque<double>> list1,list2;
	

	for(int i = 0; i < half; i++){
		list1.push_back(list[i]);
	}
	for(int j = half; j <= size-1; j++){
		list2.push_back(list[j]);
	}


	list1 = merge_split(list1);
	list2 = merge_split(list2);

return mymerge(list1, list2);
}
void myDisplayImage::merge_sort(deque<deque<double>> &list){
list = merge_split(list);
}
//========================================================================================

double myDisplayImage::myLog(double number, int base){
	double result = log(number)/log((double)base); // log_base_(number)
	return result;
}