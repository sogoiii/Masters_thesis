#include "DataLogger.h"




DataLogger::DataLogger(){

	string GPLoggerDir = "./GPLogger.txt";
	GPLogger.open(GPLoggerDir.c_str());
	if(GPLogger.fail()){
		printf("%s failed to open\n",GPLoggerDir);
	}
	
	string GMLoggerDir = "./GMlogger.txt";
	GMLogger.open(GMLoggerDir.c_str());
	if(GMLogger.fail()){
		printf("%s failed to open\n",GMLoggerDir);
	}


}

void DataLogger::OpenCreateFile(string filedir, ofstream newstream){ // not used
	newstream.open(filedir.c_str() );
	if(newstream.fail()){
	//	printf( "%s failed to open\n",filedir);
		//exit(-1);
	}

}

void DataLogger::CloseStreams(){
	GPLogger.close();
}