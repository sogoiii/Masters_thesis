#pragma once

#include "Aria.h"
#include <iostream>
using namespace std;
#include <vector>
//#include "StdAfx.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

//#include "sample2.h"
//#include "sx11.h"
//#include "sx11f.h"

#define MAX_LABELS 10000


class Vision // :public ArRecurrentTask
{
private:
    int ordernumber;
    int itemnumber;
    int quantity;
	//friend ostream& operator<<(ostream& os, const Order& order);
	//long red_pixel;
	//long redness;
	//long min_target_size_in_the_image;
public:
	void set_values(long, long, long);
	Vision();
	~Vision();
	//void task();
	CvScalar getRandomColor(int);
	IplImage* labels2psuedoColor(IplImage const*);
	std::vector<CvConnectedComp> labelBlobs(IplImage const* const , IplImage* );
	double Color_Intensity(double, double, double);
//	IplImage* doPyrDown(IplImage* in, int filter);
//	double calculateFitness();
	void StartRecurrentTask();
	void StopRecurrentTask();
	bool ContinueThread;

	int MIN_TARGET_SIZE_IN_THE_IMAGE;
	int RED_PIXEL;
	int REDNESS;
	int BLUE_PIXEL;
	int BLUENESS;
	int GREEN_PIXEL;
	int GREENNESS;

	//ECODE FrameGrabber (DWORD);

    //Order(void);
    //Order(int on, int in, int q);
    //~Order(void);
};
