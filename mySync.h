#ifndef MySync_H
#define MySync_H

#include "Aria.h" 
#include "ArNetworking.h"
#include <boost/thread.hpp>
using namespace std;


// oct 27 
class mySync
{ 
public:

	mySync(); 
	~mySync(void); 
	int ReturnRobotIDFromString(string robotstring); //Critical function

	//RecivedTasks Functions
	void ChangeReceivedTaskBool(int robotID);
	bool ReturnReceivedTaskBool(int robotID);
	void SignalReceivedTasksCondition();
	void WaitReceivedTasksCondition();
	void ReceivedTaskCheck(int numrobots, string robot1, string robot2, string robot3);
	//END RecievedTasks Fucntions
	
protected: 

	//RecievedTasks Variables
	bool PeopleReceivedTasks;
	bool PatrolReceivedTasks;
	bool P3atReceivedTasks;
	//END RecievedTasks Variables

	ArCondition ReceivedTasksCondition;

	
};


#endif // MySync_H