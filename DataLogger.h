#ifndef _DataLogger_
#define _DataLogger_


#include <iostream>
#include <fstream>
#include <string>
using namespace std;


//Note I have to Manually close the files at the end of main!!!!!!
class DataLogger { 
	
public:
	
	DataLogger();

	ofstream GPLogger; //Global PositionLogger
	ofstream GMLogger; //GridMap logger

void OpenCreateFile(string filedir, ofstream newstream);
protected:
	//Member functions
	void CloseStreams();
	//Member Objects
};

#endif