#include "mapvar.h"
#include <iostream>
#include <time.h>
using std::cout;

//oct 22 2010
mapvar::mapvar()
{
	//time(&current_time);
	visibility = 0;
	visibility_count = 0;
	obstacle = 0; //a counter
	obstacle_percentage = 0;
	isObstacle = false;
	value = 1;
	distance = 1;
	cost = 0;
	utility = 0;
	robotorigin = "Patrol";
	bid = 1;
	//time(&task_creation_time);
	//my_string = "hey";
	task_creation_string = "Initialized";

	double tempdouble;
	tempdouble = 1.0;
	//received_bids.push_back(tempdouble);
	bidvector.clear();
	bidvectorbidder.clear();


	//boost::thread* UpdateValueThread = new boost::thread(boost::bind(&mapvar::UpdateValue,this));
			
}

mapvar::~mapvar(){}

void mapvar::update_time(time_t* tvar){ // no longer used
printf("I SHOULD NOT SEE THIS IN UPDATE_TIME\n");
	/*time(tvar);
	
	char *cstr, *p, *tempt;
	string timenow;
	int tcount;
	tcount = 0;
	timenow = ctime(tvar);
	cstr = new char [timenow.size()+1];
	strcpy(cstr,timenow.c_str());
	p=strtok(cstr," ");
	while(p!=NULL){
		//printf("%s\n",p);
		if(tcount == 3){
			tempt = p;
		}
		p=strtok(NULL, " ");
		tcount++;
	}
	//printf("mapvar_update_time = %s \n", tempt);
	task_creation_string = tempt;

	//printf(ctime(&tvar));*/
}


void mapvar::update_time(struct tm *update_time_tm_variable){

time_t rawtime;
time(&rawtime);
task_creation_time_tm = localtime(&rawtime); //  = localtime(&rawtime);

//printf("tm variable in update_time = %s", asctime(task_creation_time_tm));
//cout << "just want to see if i am in here!!! \n";


time_sec = task_creation_time_tm->tm_sec;
time_min = task_creation_time_tm->tm_min;
time_hour = task_creation_time_tm->tm_hour;
}

int mapvar::grab_value(){
	return value;
}

void mapvar::UpdateValue(){
	//boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
	value = value + 5;
}