#include "tasks.h"



tasks::tasks(){

//robotcreator = "patrol";
taskConfirmFlag = 0;
taskSentConfirm = 0;
}

/*

void tasks::update_time(time_t* tvar){

	time(tvar);
	
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

	//printf(ctime(&tvar));
}
*/

void tasks::updatecreationtime(struct tm *update_time_tm_variable){

time_t rawtime;
time(&rawtime);
createdtime_tm = localtime(&rawtime); //  = localtime(&rawtime);

//printf("tm variable in update_time = %s", asctime(task_creation_time_tm));
//cout << "just want to see if i am in here!!! \n";


time_sec =  createdtime_tm->tm_sec;
time_min =  createdtime_tm->tm_min;
time_hour = createdtime_tm->tm_hour;
}

void tasks::UpdateCreateTime(){
	CreatedTime.setToNow();
}

int tasks::ReturnCreatedTimeMiliseconds(){
	return CreatedTime.mSecSince();
}

ArTime tasks::ReturnArTime(){
	return CreatedTime;
}