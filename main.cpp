// System or project includes
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <algorithm>
// SDK required includes
#include "sample2.h"
#include "sx11.h"
#include "sx11f.h"
//Open CV includes
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
// Aria Includes
#include "Aria.h"
#include "ArNetworking.h"
#include "Arnl.h"
//#include "ArLocalizationTask.h" // for patrol bot only
//#include "ArDocking.h" // for patrol bot only
#include "ArSonarLocalizationTask.h" // for people and p3at bot
//for the console window
//#include <stdio.h>  //already included in project
#include <io.h>
#include <fcntl.h>
#include <CommDlg.h>
//other or extra includes
#include <time.h>
//My code incldues
#include "mapvar.h"
#include "myclientinfo.h"
#include "myserverinfo.h"

//version 2 includes
#include "myDisplayImage.h"
#include "GridMap.h"
#include "CreateTasks.h"
#include "mystructs.h"
#include "tasks.h"
#include "Auction.h"
#include "EvaluateTask.h"
#include "MoveToLocation.h"
#include "mystructs.h"
#include "Vision.h"
#include <list>

#include <boost/thread.hpp>

using namespace std;
using std::cout;
using std::string;


#define PI 3.14
//Camera Definitions
#define CV_LOAD_IMAGE_UNCHANGED  -1
/* 8 bit, gray */
#define CV_LOAD_IMAGE_GRAYSCALE   0
/* 8 bit unless combined with CV_LOAD_IMAGE_ANYDEPTH, color */
#define CV_LOAD_IMAGE_COLOR       1
/* any depth, if specified on its own gray */
#define CV_LOAD_IMAGE_ANYDEPTH    2
/* by itself equivalent to CV_LOAD_IMAGE_UNCHANGED
   but can be modified with CV_LOAD_IMAGE_ANYDEPTH */
#define CV_LOAD_IMAGE_ANYCOLOR    4


#define CVX_RED		CV_RGB(0xff,0x00,0x00)
#define CVX_GREEN	CV_RGB(0x00,0xff,0x00)
#define CVX_BLUE	CV_RGB(0x00,0x00,0xff)
#define CVX_WHITE	CV_RGB(0xff,0xff,0xff)
#define CVX_	CV_RGB(0xff,0xff,0xff)
#define CVCONTOUR_APPROX_LEVEL 2

#define RED_PIXEL 220
#define REDNESS 1.5
#define BLUE_PIXEL 110
#define BLUENESS .25
#define GREEN_PIXEL 220
#define GREENNESS 1.3
#define MAX_LABELS 10000
#define MIN_TARGET_SIZE_IN_THE_IMAGE 14000

void CameraFindObject(MoveToLocation *Move,bool *objectnotfound);
IplImage* doPyrDown(IplImage* , int );
bool Objectfoundcheck(int x);
bool CheckPathStateNextMove(MoveToLocation *move,myServerInfo *server, int &word); // no longer used
void NoConfirmBidWins(EvaluateTask *myEvaluateTask, myServerInfo *myServer);


ArPose MoveToValidLocation();
void PopulateValidLocation();
vector<ArPose> ValidLocation;
/***************************************/
void logOptions(const char *progname)
{
  ArLog::log(ArLog::Normal, "Usage: %s [options]\n", progname);
  ArLog::log(ArLog::Normal, "[options] are any program options listed below, or any ARNL configuration");
  ArLog::log(ArLog::Normal, "parameters as -name <value>, see params/arnl.p for list.");
  ArLog::log(ArLog::Normal, "For example, -map <map file>.");
  Aria::logOptions();
}

bool gyroErrored = false;
const char* getGyroStatusString(ArRobot* robot)
{
  if(!robot || !robot->getOrigRobotConfig() || robot->getOrigRobotConfig()->getGyroType() < 2) return "N/A";
  if(robot->getFaultFlags() & ArUtil::BIT4)
  {
    gyroErrored = true;
    return "ERROR/OFF";
  }
  if(gyroErrored)
  {
    return "OK but error before";
  }
  return "OK";
}



LRESULT WINAPI WndProc (HWND, UINT, UINT, LONG);    // Window procedure
ECODE FrameGrabber (DWORD);                 // Frame grabber procedure
void ErrAbort (ECODE);                      // Error handler
void SaveBMP (BOOL);                        // File save procedure

/***************************************
 * Globals
 ***************************************/
HWND hwnd;                                  //Main window handle
LPBITMAPINFO lpbmi;                         //pointer to DIB info structure
LPVOID lpvbits;                             //pointer to DIB data
int current;                                //index of current buffer

BOOL is_recording = TRUE; // ADDED TO ENSURE THAT THE
IplImage* img; // global image.
void FormatConvertor();



/***************************************/




int PASCAL WinMain (HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR lpszCmdLine, int nCmdShow)
{
	srand((unsigned)time(0));//Seed Random Variable

    MSG msg;
    WNDCLASS wndclass;
    ECODE ecode;
    char szAppName [] = "X11 Sample2";
    char str[80];

    // Quit if another instance is already running
    if (FindWindow (szAppName, NULL) != NULL) {
        sprintf (str, "Only one instance of this application is allowed to run.");
        MessageBox (NULL, str, szAppName, MB_OK | MB_ICONSTOP);
        return 0;
    }

    // Register window class
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInst;
    wndclass.hIcon = LoadIcon (hInst, "ICONX11");
    wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground = HBRUSH(GetStockObject (WHITE_BRUSH));
    wndclass.lpszMenuName = "Sample2";
    wndclass.lpszClassName = szAppName;
    RegisterClass (&wndclass);

    /*  // Create program window
    hwnd = CreateWindow (szAppName, szAppName,
                         WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^WS_SIZEBOX,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         XACT_NTSC + GetSystemMetrics (SM_CXBORDER) * 2,
                         YACT_NTSC + GetSystemMetrics (SM_CYCAPTION) +
                         GetSystemMetrics (SM_CYMENU) +
                         GetSystemMetrics (SM_CYBORDER) * 2,
                         NULL, NULL, hInst, NULL);

    ShowWindow (hwnd, SW_SHOWNORMAL);
    SetWindowPos (hwnd, HWND_TOP, 0, 0,
                  XACT_NTSC + GetSystemMetrics (SM_CXBORDER) * 2,
                  YACT_NTSC + GetSystemMetrics (SM_CYCAPTION) +
                  GetSystemMetrics (SM_CYMENU), SWP_NOMOVE);
    */ // End Create Program Window

	// My Console Window
	    AllocConsole();

    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;

	//End my Console Window
	UpdateWindow (hwnd);



// Initialize Aria and Arnl global information
  Aria::init();
  Arnl::init();

  // The robot object
  ArRobot robot;

  // Parse the command line arguments.
  ArArgumentParser parser(&__argc, __argv);

  // Set up our simpleConnector, to connect to the robot and laser
  //ArSimpleConnector simpleConnector(&parser);
  ArRobotConnector robotConnector(&parser, &robot);

  // Connect to the robot
  if (!robotConnector.connectRobot())
  {
    ArLog::log(ArLog::Normal, "Error: Could not connect to robot... exiting");
    Aria::exit(3);
  }

  // Set up where we'll look for files. Arnl::init() set Aria's default
  // directory to Arnl's default directory; addDirectories() appends this
  // "examples" directory.
  char fileDir[1024];
  ArUtil::addDirectories(fileDir, sizeof(fileDir), Aria::getDirectory(),
			 "examples");

  // To direct log messages to a file, or to change the log level, use these  calls:
  //ArLog::init(ArLog::File, ArLog::Normal, "log.txt", true, true);
  //ArLog::init(ArLog::File, ArLog::Verbose);

  // Add a section to the configuration to change ArLog parameters
  ArLog::addToConfig(Aria::getConfig());

  // set up a gyro (if the robot is older and its firmware does not
  // automatically incorporate gyro corrections, then this object will do it)
  ArAnalogGyro gyro(&robot);

  // Our networking server
  ArServerBase server;

  // Set up our simpleOpener, used to set up the networking server
  ArServerSimpleOpener simpleOpener(&parser);

  // Load default arguments for this computer (from /etc/Aria.args, environment
  // variables, and other places)
  parser.loadDefaultArguments();

  // Parse arguments
  if (!Aria::parseArgs() || !parser.checkHelpAndWarnUnparsed()){
    logOptions(__argv[0]);
    Aria::exit(1);
  }

  // This causes Aria::exit(9) to be called if the robot unexpectedly
  // disconnects
  ArGlobalFunctor1<int> shutdownFunctor(&Aria::exit, 9);
  robot.addDisconnectOnErrorCB(&shutdownFunctor);

  // Create an ArSonarDevice object (ArRangeDevice subclass) and
  // connect it to the robot.
  ArSonarDevice sonarDev;
  robot.addRangeDevice(&sonarDev);

  // This object will allow robot's movement parameters to be changed through
  // a Robot Configuration section in the ArConfig global configuration facility.
  ArRobotConfig robotConfig(&robot);

  // Include gyro configuration options in the robot configuration section.
  robotConfig.addAnalogGyro(&gyro);

  // Start the robot thread.
  robot.runAsync(true);

    /* Create and set up map object */

  // Set up the map object, this will look for files in the examples
  // directory (unless the file name starts with a /, \, or .
  // You can take out the 'fileDir' argument to look in the program's current directory
  // instead.
  // When a configuration file is loaded into ArConfig later, if it specifies a
  // map file, then that file will be loaded as the map.
  ArMap map(fileDir);
  // set it up to ignore empty file names (otherwise if a configuration omits
  // the map file, the whole configuration change will fail)
  map.setIgnoreEmptyFileName(true);
  // ignore the case, so that if someone is using MobileEyes or
  // MobilePlanner from Windows and changes the case on a map name,
  // it will still work.
  map.setIgnoreCase(true);

    /* Create localization and path planning threads */
  ArPathPlanningTask pathTask(&robot, &sonarDev, &map);
  ArLog::log(ArLog::Normal, "Creating sonar localization task");
  ArSonarLocalizationTask locTask(&robot, &sonarDev, &map);


    /* Start the server */

  // Open the networking server
  if (!simpleOpener.open(&server, fileDir, 240)){
    ArLog::log(ArLog::Normal, "Error: Could not open server.");
    exit(2);
  }

  /* Add additional range devices to the robot and path planning task (so it
     avoids obstacles detected by these devices) */

  // Add IR range device to robot and path planning task (so it avoids obstacles
  // detected by this device)
  robot.lock();
  ArIRs irs;
  robot.addRangeDevice(&irs);
  pathTask.addRangeDevice(&irs, ArPathPlanningTask::CURRENT);

  // Add bumpers range device to robot and path planning task (so it avoids obstacles
  // detected by this device)
  ArBumpers bumpers;
  robot.addRangeDevice(&bumpers);
  pathTask.addRangeDevice(&bumpers, ArPathPlanningTask::CURRENT);

  // Add range device which uses forbidden regions given in the map to give virtual
  // range device readings to ARNL.  (so it avoids obstacles
  // detected by this device)
  ArForbiddenRangeDevice forbidden(&map);
  robot.addRangeDevice(&forbidden);
  pathTask.addRangeDevice(&forbidden, ArPathPlanningTask::CURRENT);

  // This is the place to add a range device which will hold sensor data
  // and delete it appropriately to replan around blocked paths.
  ArGlobalReplanningRangeDevice replanDev(&pathTask);


  robot.unlock();




  // "Custom" commands. You can add your own custom commands here, they will
  // be available in MobileEyes' custom commands (enable in the toolbar or
  // access through Robot Tools)
  ArServerHandlerCommands commands(&server);


  // These provide various kinds of information to the client:
  ArServerInfoRobot serverInfoRobot(&server, &robot);
  ArServerInfoSensor serverInfoSensor(&server, &robot);
  ArServerInfoPath serverInfoPath(&server, &robot, &pathTask);
//  serverInfoPath.addSearchRectangleDrawing(&drawings);
  serverInfoPath.addControlCommands(&commands);

  // Provides localization info and allows the client (MobileEyes) to relocalize at a given
  // pose:
  ArServerInfoLocalization serverInfoLocalization(&server, &robot, &locTask);
  ArServerHandlerLocalization serverLocHandler(&server, &robot, &locTask);

  // If you're using MobileSim, ArServerHandlerLocalization sends it a command
  // to move the robot's true pose if you manually do a localization through
  // MobileEyes.  To disable that behavior, use this constructor call instead:
  // ArServerHandlerLocalization serverLocHandler(&server, &robot, true, false);
  // The fifth argument determines whether to send the command to MobileSim.

  // Provide the map to the client (and related controls):
  ArServerHandlerMap serverMap(&server, &map);

  // These objects add some simple (custom) commands to 'commands' for testing and debugging:
  ArServerSimpleComUC uCCommands(&commands, &robot);                   // Send any command to the microcontroller
  ArServerSimpleComMovementLogging loggingCommands(&commands, &robot); // configure logging
  ArServerSimpleComLogRobotConfig configCommands(&commands, &robot);   // trigger logging of the robot config parameters
//  ArServerSimpleServerCommands serverCommands(&commands, &server);     // monitor networking behavior (track packets sent etc.)




//==================================================================================

  // Action to slow down robot when localization score drops but not lost.
  ArActionSlowDownWhenNotCertain actionSlowDown(&locTask);
  pathTask.getPathPlanActionGroup()->addAction(&actionSlowDown, 140);

  // Action to stop the robot when localization is "lost" (score too low)
  ArActionLost actionLostPath(&locTask, &pathTask);
  pathTask.getPathPlanActionGroup()->addAction(&actionLostPath, 150);


  // service that allows the client to monitor the communication link status
  // between the robot and the client.
  ArServerHandlerCommMonitor handlerCommMonitor(&server);

  // service that allows client to change configuration parameters in ArConfig
  ArServerHandlerConfig handlerConfig(&server, Aria::getConfig(),
				      Arnl::getTypicalDefaultParamFileName(),
				      Aria::getDirectory());



//==================================================================================


  // Mode To go to a goal or other specific point:
  ArServerModeGoto modeGoto(&server, &robot, &pathTask, &map,
			    locTask.getRobotHome(),
			    locTask.getRobotHomeCallback());


  // Mode To stop and remain stopped:
  ArServerModeStop modeStop(&server, &robot);


  // Teleoperation modes To drive by keyboard, joystick, etc:
  ArServerModeRatioDrive modeRatioDrive(&server, &robot);
//  ArServerModeDrive modeDrive(&server, &robot);            // Older mode for compatability



  // Prevent normal teleoperation driving if localization is lost using
  // a high-priority action, which enables itself when the particular mode is
  // active.
  // (You have to enter unsafe drive mode to drive when lost.)
  ArActionLost actionLostRatioDrive(&locTask, &pathTask, &modeRatioDrive);
  modeRatioDrive.getActionGroup()->addAction(&actionLostRatioDrive, 110);

  // Add drive mode section to the configuration, and also some custom (simple) commands:
  modeRatioDrive.addToConfig(Aria::getConfig(), "Teleop settings");
  modeRatioDrive.addControlCommands(&commands);

  // Wander mode (also prevent wandering if lost):
  ArServerModeWander modeWander(&server, &robot);
  ArActionLost actionLostWander(&locTask, &pathTask, &modeWander);
  modeWander.getActionGroup()->addAction(&actionLostWander, 110);


  // This provides a small table of interesting information for the client
  // to display to the operator. You can add your own callbacks to show any
  // data you want.
  ArServerInfoStrings stringInfo(&server);
  Aria::getInfoGroup()->addAddStringCallback(stringInfo.getAddStringFunctor());




//==================================================================================

  // Display gyro status if gyro is enabled and is being handled by the firmware (gyro types 2, 3, or 4).
  // (If the firmware detects an error communicating with the gyro or IMU it
  // returns a flag, and stops using it.)
  // (This gyro type parameter, and fault flag, are only in ARCOS, not Seekur firmware)
  if(robot.getOrigRobotConfig() && robot.getOrigRobotConfig()->getGyroType() > 1)
 {
    Aria::getInfoGroup()->addStringString(
        "Gyro/IMU Status", 10,
          new ArGlobalRetFunctor1<const char*, ArRobot*>(&getGyroStatusString, &robot)
      );
  }

  // Make Stop mode the default (If current mode deactivates without entering
  // a new mode, then Stop Mode will be selected)
// modeStop.addAsDefaultMode();
    // TODO move up near where stop mode is created?


  // create a pose storage class, this will let the program keep track
  // of where the robot is between runs...  after we try and restore
  // from this file it will start saving the robot's pose into the
  // file
  ArPoseStorage poseStorage(&robot);
  /// if we could restore the pose from then set the sim there (this
  /// won't do anything to the real robot)... if we couldn't restore
  /// the pose then just reset the position of the robot (which again
  /// won't do anything to the real robot)
 // if (poseStorage.restorePose("robotPose"))
//   serverLocHandler.setSimPose(robot.getPose());
 // else
 //   robot.com(ArCommands::SIM_RESET);

  /* File transfer services: */

#ifdef WIN32
  // Not implemented for Windows yet.
  ArLog::log(ArLog::Normal, "Note, file upload/download services are not implemented for Windows; not enabling them.");
#else
  // This block will allow you to set up where you get and put files
  // to/from, just comment them out if you don't want this to happen
  // /*
  ArServerFileLister fileLister(&server, fileDir);
  ArServerFileToClient fileToClient(&server, fileDir);
  ArServerFileFromClient fileFromClient(&server, fileDir, "/tmp");
  ArServerDeleteFileOnServer deleteFileOnServer(&server, fileDir);
  // */
#endif

    /* Load configuration values, map, and begin! */

  // When parsing the configuration file, also look at the program's command line options
  // from the command-line argument parser as well as the configuration file.
  // (So you can use any argument on the command line, namely -map.)
  Aria::getConfig()->useArgumentParser(&parser);

  // Read in parameter files.
  ArLog::log(ArLog::Normal, "Loading config file %s into ArConfig...", Arnl::getTypicalParamFileName());
  if (!Aria::getConfig()->parseFile(Arnl::getTypicalParamFileName())){
    ArLog::log(ArLog::Normal, "Trouble loading configuration file, exiting");
    Aria::exit(5);
  }

  // Warn about unknown params.
  if (!simpleOpener.checkAndLog() || !parser.checkHelpAndWarnUnparsed()){
   logOptions(__argv[0]);
    Aria::exit(6);
  }

  // Warn if there is no map
  if (map.getFileName() == NULL || strlen(map.getFileName()) <= 0){
    ArLog::log(ArLog::Normal, "");
    ArLog::log(ArLog::Normal, "### No map file is set up, you can make a map with the following procedure");
    ArLog::log(ArLog::Normal, "   0) You can find this information in README.txt or docs/SonarMapping.txt");
    ArLog::log(ArLog::Normal, "   1) Start up Mapper3Basic");
    ArLog::log(ArLog::Normal, "   2) Go to File->New");
    ArLog::log(ArLog::Normal, "   3) Draw a line map of your area (make sure it is to scale)");
    ArLog::log(ArLog::Normal, "   4) Go to File->Save on Robot");
    ArLog::log(ArLog::Normal, "   5) In MobileEyes, go to Tools->Robot Config");
    ArLog::log(ArLog::Normal, "   6) Choose the Files section");
    ArLog::log(ArLog::Normal, "   7) Enter the path and name of your new .map file for the value of the Map parameter.");
    ArLog::log(ArLog::Normal, "   8) Press OK and your new map should become the map used");
    ArLog::log(ArLog::Normal, "");
  }

  // Print a log message notifying user of the directory for map files
  ArLog::log(ArLog::Normal, "");
  ArLog::log(ArLog::Normal,
	     "Directory for maps and file serving: %s", fileDir);

  ArLog::log(ArLog::Normal, "See the ARNL README.txt for more information");
  ArLog::log(ArLog::Normal, "");

  // Do an initial localization of the robot. It tries all the home points
  // in the map, as well as the robot's current odometric position, as possible
  // places the robot is likely to be at startup.   If successful, it will
  // also save the position it found to be the best localized position as the
  // "Home" position, which can be obtained from the localization task (and is
  // used by the "Go to home" network request).
  //locTask.localizeRobotAtHomeBlocking();

  // Start the networking server's thread
  //server.runAsync(); // is called below somehwere
  robot.enableMotors();

  /*ArActionAvoidFront avoidFrontNear("Avoid Front Near", 225, 0);
  ArActionAvoidFront avoidFrontFar;

  robot.addAction(&avoidFrontNear, 50);
  robot.addAction(&avoidFrontFar, 49);*/

   ArKeyHandler *keyHandler;
  if ((keyHandler = Aria::getKeyHandler()) == NULL)
  {
    keyHandler = new ArKeyHandler;
    Aria::setKeyHandler(keyHandler);
    robot.lock();
    robot.attachKeyHandler(keyHandler);
    robot.unlock();
    puts("Server running. To exit, press escape.");
  }

/////////////////////////////////////////////////// END OF ARIA CODE

      // Initialize frame grabber
    if (ecode = FrameGrabber (FG_INIT)) {
        sprintf (str, "Initialization failed; error %d", ecode);
        MessageBox (hwnd, str, szAppName, MB_OK | MB_ICONSTOP);
        SendMessage (hwnd, WM_DESTROY, 0, 0L);
        return 0;
    }
    UpdateWindow (hwnd);
	FrameGrabber (FG_CHAN1); // ## Calling framegrabber specific to channel 1 ##


/////////////////////////////////////////////////// CAMERA INITIALIZATIONS

	//ArPose posLoc = ArPose(4020,-5542,90);//remove when done!!!
	ArPose posLoc = ArPose(1520,-1014,0); // patrol in angello august
	locTask.setRobotPose(posLoc);
	locTask.forceUpdatePose(posLoc);
	double temp_num = locTask.getLocalizationScore();
	printf("Localization Score = %f\n",temp_num);
	while(temp_num < .25){ // localizes the robot to posloc ArPose below // .45 was original
		ArPose posLoc = ArPose(1520,-1014,0); // patrol in angello august
		locTask.setRobotPose(posLoc);
		locTask.forceUpdatePose(posLoc);
		ArUtil::sleep(7000);
		temp_num = locTask.getLocalizationScore();
		printf("While Location score = %f\n",temp_num);
	}

	  //create logging files
	  ofstream LogMovingToLocations;
	  string directoryone = "./LogMovingToLocations.txt";
	  LogMovingToLocations.open(directoryone.c_str());
	  if(LogMovingToLocations.fail()){
		cout << directoryone << " Failed to open LogMovingToLocations" << endl;
	  }

	  //end logging file

	string MAPFILE = "C:/Program Files/MobileRobots/ARNL/examples/Angello_august.map"; //location of map file
	string SelfRobotName = "p3at";
	mySync Sync;

	myDisplayImage DisplayImage; //Visual display of where robot is
	DisplayImage.map_parse(MAPFILE); //Read map file and create display window
	DisplayImage.AddArRobot(&robot); //add robot class to display image (only calls getx,gety,getth)

		GridMap myValueMap(&DisplayImage); //my value map
		myValueMap.CreateGridMap(DisplayImage.ReturnXLimit(),DisplayImage.ReturnYLimit());  //size of my value map
		myValueMap.BeginUpdateMapValues();  //Starts the updateGridMapValue thread

		CreateTasks myCreateTasks(&robot);  //create task class
		myCreateTasks.AddSelfRobotName(SelfRobotName); // THIS LINE IS IMPORTANT!!! NEEDS TO BE DIFFERENT FOR EVERY ROBOT
		myCreateTasks.AddGridMap(&myValueMap); //add grid map to create tasks
		myCreateTasks.AddDisplayImage(&DisplayImage); // Add display image to create tasks

		EvaluateTask myEvaluateTask(&robot); //Evaluate tasks class
		myEvaluateTask.AddCreateTasks(&myCreateTasks); // add create tasks to evaluate tasks
		myEvaluateTask.AddGridmap(&myValueMap); // add grid map to evaluate tasks
		myEvaluateTask.AddPathTask(&pathTask);
		myEvaluateTask.AddSelfRobotName(SelfRobotName);

		//  DataLogger myDataLogger; //creates files that save specific data

		MoveToLocation myMove; //Initialize our move to class
		myMove.AddPathPlanning(&pathTask); // add pathplan to mymove class
		//  myMove.AddDataLogger(&myDataLogger); //adds ofstreams to this class

		Auction myAuction(&robot);    //Initialize Auction class
		myAuction.AddGridMap(&myValueMap);  //add value map to auction so it can grab values

		// Open the Server
		if(!server.open(9999)){
		printf("Could not open server port\n");
		exit(1);
		}
		server.runAsync(); // server runs in its own async thread

		myServerInfo myServer(&server); // my server helper class
		myServer.AddEvaluateTask(&myEvaluateTask); // add evaluate task to my sever helper class
		myServer.AddPathPlanning(&pathTask); // add pathtask to my server helper class
		myServer.AddGridMap(&myValueMap);
		myServer.AddAuctionClass(&myAuction);  //Add auction to server class
		myServer.AddSelfRobotName(SelfRobotName);





		//Start Talk to P3at Bot
		ArClientBase p3atclient; // people client
		string hostname2 = "192.168.25.199"; // IP of people bot - if not connecting check ip address
		while(!p3atclient.blockingConnect(hostname2.c_str(),9999)){ //halts here untill connection is made
			printf("Could not connect to P3at\n");
		}
		printf("Connected To P3at\n");
		myClientInfo myP3atClient(&p3atclient, "p3at");//my client helper class Unique p3at name
		myP3atClient.AddCreateTask(&myCreateTasks);// add create tasks to patrolclient
		myP3atClient.AddEvaluateTask(&myEvaluateTask); // add evaluatetask to people client
		myP3atClient.AddThisPhysicalRobotName(SelfRobotName); //unique line must be changed for every robot
		myP3atClient.AddmySync(&Sync);
		p3atclient.runAsync();	//client runs in its own async thread
		//End Talk to P3at Bot

			//Start Talk to People Bot
		ArClientBase peopleclient; // people client
		string hostname0 = "192.168.25.186"; // IP of people bot - if not connecting check ip address
		while(!peopleclient.blockingConnect(hostname0.c_str(),9999)){ //halts here untill connection is made
			printf("Could not connect to PeopleBot\n");
		}
		printf("Connected To People bot\n");
		myClientInfo myPeopleClient(&peopleclient, "people");//my client helper class Unique people name
		myPeopleClient.AddCreateTask(&myCreateTasks);// add create tasks to patrolclient
		myPeopleClient.AddEvaluateTask(&myEvaluateTask); // add evaluatetask to people client
		myPeopleClient.AddThisPhysicalRobotName(SelfRobotName); //unique line must be changed for every robot
		myPeopleClient.AddmySync(&Sync);
		peopleclient.runAsync();	//client runs in its own async thread
		//End Talk to People Bot

		//	Start Talking to Patrol bot
			ArClientBase patrolclient; // patrol client
		string hostname1 = "192.168.25.113"; // IP of patrol bot - if not connecting check ip address
		/*	while(!patrolclient.blockingConnect(hostname1.c_str(),9999)){ //halts here untill connection is made
			printf("Could not connect to self (Patrol Bot)\n");
		}//*/
		printf("Connected To Patrol Bot\n");
		myClientInfo myPatrolClient(&patrolclient, "patrol");//my client helper class Unique patrol name
		myPatrolClient.AddCreateTask(&myCreateTasks); // add create tasks to patrolclient
		myPatrolClient.AddEvaluateTask(&myEvaluateTask); // add evaluatetask to patrol client
		myPatrolClient.AddThisPhysicalRobotName(SelfRobotName); //unique line must be changed for every robot
		myPatrolClient.AddmySync(&Sync);
		patrolclient.runAsync();	//client runs in its own async thread
		// End Talking to Patrol bot

		myPeopleClient.AddAuction(&myAuction,0); // 0 = people // also starts ArRecurrent Thread
		myPatrolClient.AddAuction(&myAuction,1); // 1 = patrol // also starts ArRecurrent Thread
		myP3atClient.AddAuction(&myAuction,2); // 2 = p3at // also starts ArRecurrent Thread


	ArUtil::sleep(1000); // static wait to minimize code desync
		boost::thread* AreWeReadyThread = new boost::thread(boost::bind(&Auction::AreWeReady, &myAuction, 2, "patrol", "p3at", "people"));// 0 = people, 1 = patrol, 2 = p3at // unique line for every robot
		myPatrolClient.StatusReadyGo(1);// tell patrol that robotID is ready
		myPeopleClient.StatusReadyGo(1);// tell people that robotID is ready
		myP3atClient.StatusReadyGo(1); // tell p3at that robotID is ready

		//static
		myAuction.StatusReadyGoWait(); //wait utill all the robots are aready
		myValueMap.BeginUpdateMapValues();  //Starts the updateGridMapValue thread


		PopulateValidLocation(); //THIS WILL WORK VECTOR OF ARPOSES
		bool conflict = false;
		bool objectnotfound = true; // object not found is true
	/*	printf("Entering camera code now!!!\n");
		while(1){
			//myValueMap.StartRecurrentTask(); // draws and updates grid map of seen locations in own thead
		 //   boost::thread* DisplayThread = new boost::thread(boost::bind(&GridMap::WrappedFovDisplay, &myValueMap));
		ArUtil::sleep(1000);
		printf("Camera will start now!\n");
		ArUtil::sleep(1000);
		boost::thread CameraThread(CameraFindObject, &myMove, &objectnotfound); //camera processing in own thread
		myMove.WaitMovePathCondition(); // pauses main thread  ArPathPlan goal reached or failed call the signal
		printf("CAMERA CAMERA CAMERA CAMERA == STOPPED!\n");
		ArUtil::sleep(10000);
		ArUtil::sleep(10000);
		}*/


		while(objectnotfound){
StartOfAlgorithm: // start of algorithm
			//MoveToValidLocation();
			//ArUtil::sleep(2000);
			//printf("Beginign of my Algorithm\n");
			//goto ThisWillWORK;//for debuging only



			myCreateTasks.CreateSelfTasks(robot.getX(),robot.getY());
			myCreateTasks.ChoosePossibleLocationList(myValueMap.MyGridMap, myValueMap.MyGridMap[0].size(), myValueMap.MyGridMap.size(), 4);
		//	myPeopleClient.SendCurrentTaskList(myCreateTasks.getSelfCretedTasks());
		//	myPatrolClient.SendCurrentTaskList(myCreateTasks.getSelfCretedTasks());
			myP3atClient.SendCurrentTaskList(myCreateTasks.getSelfCretedTasks());

			myEvaluateTask.DEBUG_ClearSimilarLocations(); // here to get rid of an error but it works so keep it
		  //boost::thread* ReceivedTaskCheckThread = new boost::thread(boost::bind(&mySync::ReceivedTaskCheck, &Sync, 2, "people", "patrol", "p3at"));//unique line for every robot
		  //Sync.WaitReceivedTasksCondition();
		//	printf("Waiting to send and recieve tasks\n");
		//	ArUtil::sleep(3000);

			ArUtil::sleep(2000);
			myEvaluateTask.chooseMyBid(myValueMap.MyGridMap); //SELFNOTE i delete tasklist in this function

			conflict = myEvaluateTask.SecondPathPlanCheck();
			if(conflict == true){//exchange value mapes with other robot
				printf("ENTERING SECONDPATHPLANCHECK()!!!!!!!!!!!!!!!!!!!!!!!\n");
				vector<tasks> similarlocs;
				similarlocs = myEvaluateTask.GrabSimilarLocations();

				if(similarlocs[0].robotSimilarLocation.compare("people") == 0){ // need to exhcang with conflictrobot
					printf("i should exchange maps with people\n");
				//	myPeopleClient.ExchangeValueMaps(myValueMap.MyGridMap);
					myEvaluateTask.CenterLocationComputation();	// compute distance to center location
				//	myPeopleClient.SendConflictLocationInformation(myEvaluateTask.GrabSimilarLocations());
					//upon receving distances... return to other robot and declare who is closer and hence
					// the owner of that object
					myEvaluateTask.WaitDistanceValueCondition(); // wait for the distance value to be received. signal is called in serverinfo.
					myEvaluateTask.WhoOwnConflictLocation(); //

					//then compute costs for similarlocations
					myEvaluateTask.FillCostsAndInformation(myEvaluateTask.GrabSimilarLocations());
					//add the similar task to CurrentTaskList
					myEvaluateTask.PushCurrentTaskList();

					//starting from the new contention global posiiton, create new tasks
					vector<tasks> contentionlocation = myEvaluateTask.GrabSimilarLocations();
					printf("Creating secondary TASKS\n");
					myCreateTasks.CreateSelfTasks(contentionlocation[0].x_global, contentionlocation[0].y_global);
					myCreateTasks.ChoosePossibleLocationList(myValueMap.MyGridMap, myValueMap.MyGridMap[0].size(), myValueMap.MyGridMap.size(), 2);

					printf("SENDING secondary TASKS to P3at and People\n");
				//	myPeopleClient.SendCurrentTaskList(myCreateTasks.getSelfCretedTasks());
					myP3atClient.SendCurrentTaskList(myCreateTasks.getSelfCretedTasks());

				}
				else if(similarlocs[0].robotSimilarLocation.compare("patrol") == 0){
					printf("i should exchange maps with patrol\n");
			//		myPatrolClient.ExchangeValueMaps(myValueMap.MyGridMap);
					myEvaluateTask.CenterLocationComputation();	// compute distance to center location
			//		myPatrolClient.SendConflictLocationInformation(myEvaluateTask.GrabSimilarLocations());
					//upon receving distances... return to other robot and declare who is closer and hence
					// the owner of that object
					myEvaluateTask.WaitDistanceValueCondition(); // wait for the distance value to be received. signal is called in serverinfo.
					myEvaluateTask.WhoOwnConflictLocation(); //

					//then compute costs for similarlocations
					myEvaluateTask.FillCostsAndInformation(myEvaluateTask.GrabSimilarLocations());
					//add the similar task to CurrentTaskList
					myEvaluateTask.PushCurrentTaskList();

					//starting from the new contention global posiiton, create new tasks
					vector<tasks> contentionlocation = myEvaluateTask.GrabSimilarLocations();
					printf("Creating secondary TASKS\n");
					myCreateTasks.CreateSelfTasks(contentionlocation[0].x_global, contentionlocation[0].y_global);
					myCreateTasks.ChoosePossibleLocationList(myValueMap.MyGridMap, myValueMap.MyGridMap[0].size(), myValueMap.MyGridMap.size(), 2);

					printf("SENDING secondary TASKS to P3at and Patrol\n");
				//	myPatrolClient.SendCurrentTaskList(myCreateTasks.getSelfCretedTasks());
					myP3atClient.SendCurrentTaskList(myCreateTasks.getSelfCretedTasks());
				}
				else if(similarlocs[0].robotSimilarLocation.compare("p3at") == 0){
					printf("i should exchange maps with p3at\n");
					myP3atClient.ExchangeValueMaps(myValueMap.MyGridMap);
					myEvaluateTask.CenterLocationComputation();
					myP3atClient.SendConflictLocationInformation(myEvaluateTask.GrabSimilarLocations());
					//upon receving distances... return to other robot and declare who is closer and hence
					//the owner of that object
					myEvaluateTask.WaitDistanceValueCondition(); // wait for the distance value to be received. signal is called in serverinfo.
					myEvaluateTask.WhoOwnConflictLocation(); //

					//then compute costs for similarlocations
					myEvaluateTask.FillCostsAndInformation(myEvaluateTask.GrabSimilarLocations());
					//add the similar task to CurrentTaskList
					myEvaluateTask.PushCurrentTaskList();

					//starting from the new contention global posiiton, create new tasks
					vector<tasks> contentionlocation = myEvaluateTask.GrabSimilarLocations();
					printf("Creating secondary TASKS\n");
					myCreateTasks.CreateSelfTasks(contentionlocation[0].x_global, contentionlocation[0].y_global);
					myCreateTasks.ChoosePossibleLocationList(myValueMap.MyGridMap, myValueMap.MyGridMap[0].size(), myValueMap.MyGridMap.size(), 2);

					printf("SENDING secondary TASKS to P3at and P3at I SHOULD NEVER SEE THIS\n");
					myP3atClient.SendCurrentTaskList(myCreateTasks.getSelfCretedTasks());
				}
				else{printf("SIMILARLOCS COMPARE FAILED!!!!!!\n");}

			  //ArUtil::sleep(5000);
			  //boost::thread* ReceivedTaskCheckThread = new boost::thread(boost::bind(&mySync::ReceivedTaskCheck, &Sync, 2, "people", "patrol", "p3at"));//unique line for every robot
			  //Sync.WaitReceivedTasksCondition();
				goto ComputeBid; // go to computemybid

			}//*/
			if(conflict == false){ // if only exists to remove the Compiler error C2360
ComputeBid:
			vector <tasks> mybids = myEvaluateTask.computeMyBid();
			for(unsigned int i=0; i < mybids.size();i++){
				if(mybids[i].robotcreator.compare("people") == 0){
					printf("PLACING BID ON ROBOT %s for loc = %d %d\n", mybids[i].robotcreator.c_str(),mybids[i].x_index, mybids[i].y_index);
					mybids[i].robotbidder = SelfRobotName; // should be the name of this robot
				//	myPeopleClient.SendBid(mybids[i]);
				}
				else if(mybids[i].robotcreator.compare("patrol") == 0){
					printf("PLACING BID ON ROBOT %s for loc = %d %d\n", mybids[i].robotcreator.c_str(),mybids[i].x_index, mybids[i].y_index);
					mybids[i].robotbidder = SelfRobotName; // should be the name of this robot
					//myPatrolClient.SendBid(mybids[i]);
				}
				else if(mybids[i].robotcreator.compare("p3at") == 0){
					printf("PLACING BID ON ROBOT %s for loc = %d %d\n", mybids[i].robotcreator.c_str(),mybids[i].x_index, mybids[i].y_index);
					mybids[i].robotbidder = SelfRobotName; // should be the name of this robot
					myP3atClient.SendBid(mybids[i]);
				}
			} // end of for loop
		} // end of if statment


	  //boost::thread CatchThread(NoConfirmBidWins, &myEvaluateTask, &myServer);
	  //NoConfirmBidWins(&myEvaluateTask, &myServer);

		printf("WAITING WAITING WAITING WAITING For Server\n");
		myServer.WaitServerMoveNowCondition(); // Server Recieves

		int xt = myValueMap.ReturnXglobal(myServer.CurrentTaskExport.x_index,myServer.CurrentTaskExport.y_index);
		int yt = myValueMap.ReturnYglobal(myServer.CurrentTaskExport.x_index,myServer.CurrentTaskExport.y_index);
		printf("Moving To Global Position = %d %d\n",  xt , yt);
		LogMovingToLocations << "Moving to " << myServer.CurrentTaskExport.x_index << " ";
		LogMovingToLocations << myServer.CurrentTaskExport.y_index << " ";
		LogMovingToLocations << xt << " " << yt; // logging locations it will move to
		LogMovingToLocations << " With Bid = " << myServer.CurrentTaskExport.bid << endl;

		myMove.AddLocationToMoveTo(xt,yt);
		myMove.NowMove();	//tells the robot to move to a location
		myValueMap.StartRecurrentTask(); // draws and updates grid map of seen locations in own thread
		//boost::thread* DisplayThread = new boost::thread(boost::bind(&GridMap::WrappedFovDisplay, &myValueMap));
		boost::thread CameraThread(CameraFindObject, &myMove, &objectnotfound); //camera processing in own thread
		myMove.WaitMovePathCondition(); // pauses main thread  ArPathPlan goal reached or failed call the signal
		myValueMap.StopRecurrentTask(); // stops drawing and updating grid map
		CameraThread.interrupt(); // the thread should be closed by now, here just incase something happens
		//DisplayThread.interrupt();
		if(objectnotfound == true){ // object not found
			printf("Object found == FALSE\n");
		}
		else if(objectnotfound == false){ // Object is found
			printf("Object found == TRUE\n");
			goto TargetFound; // object found, exit out of switch statement
		}

		if(myMove.GetPathState() == 1){// goal reached go to state 0
			printf("Restarting Algorithm\n");
			myServer.resetMovingNow();
			goto StartOfAlgorithm;
		}
		else if (myMove.GetPathState() == 2){//failed to get to goal -- go to state 4
			printf("GOING TO CANTGOTOLOCATION\n");
			goto CantGoToLocation;
		}

CantGoToLocation:
		printf("CANT MOVE TO ORIGINAL LOCATION **********************\n");
		vector<vector<int>> neighbors = myValueMap.Get8neighbors(myServer.CurrentTaskExport.x_index,myServer.CurrentTaskExport.y_index); // does not cause C2360 error...odd

		for(unsigned int i = 0;i < (neighbors.size() - 1);i++){
			myMove.AddLocationToMoveTo(neighbors[i][0],neighbors[i][1]); //Have move class know what location to move to
			LogMovingToLocations << "Attempting to move:" << i <<": " << neighbors[i][0] << " " << neighbors[i][1] << endl; // logging locations it will move to
			myMove.NowMove(); //tells the robot to move to a location
			myValueMap.StartRecurrentTask(); // draws and updates grid map of seen locations in own thead
		 //   boost::thread* DisplayThread = new boost::thread(boost::bind(&GridMap::WrappedFovDisplay, &myValueMap));
		    boost::thread CameraThread(CameraFindObject, &myMove, &objectnotfound); //camera processing in own thread
			myMove.WaitMovePathCondition(); // pauses main thread  ArPathPlan goal reached or failed call the signal
			myValueMap.StopRecurrentTask(); // stops drawing and updating grid map
			CameraThread.interrupt(); // the thread should be closed by now, here just incase something happens
			//DisplayThread.interrupt();
			if(objectnotfound == true){ // object not found
				printf("Object found == FALSE\n");
			}
			else if(objectnotfound == false){ // Object is found
				printf("Object found == TRUE\n");
				pathTask.endPathPlanToLocalPose(false);//pathTask.cancelPathPlan();
				goto TargetFound; // object found, exit out of switch statement and into next while below
			}

			if(myMove.GetPathState() == 1){// goal reached go to state 0
				printf("GOING TO CASE 0\n");
				myServer.resetMovingNow();
				goto StartOfAlgorithm;
			}
			else if (myMove.GetPathState() == 2){//failed to get to goal --
				if(neighbors.size() == i){
					printf("NOPE, TRIED THEM ALL... GO TO THISWILLWORK!!!\n");
					goto ThisWillWORK;
				}
				printf("Neighbor location not possible, check next loction\n");
			}

		}//end of for loop

ThisWillWORK:
		//Moving to neighbors has failed, now go to a random valid locaiton.
		ArUtil::sleep(10000);	//DEBUG LINE SO I CNA READ OUTPUT!!s
		ArPose ValidPose;
		printf("THIS WILL WORK BEFORE FOR LOOP!!!\n");
		for(unsigned j = 0; j < 5;j++){
			printf("THE INDEX OF MY VALID LOCATION FUNCTION IS %d!!!!\n",j);
			ValidPose = MoveToValidLocation();
			myMove.AddLocationToMoveTo(ValidPose.getX(),ValidPose.getY()); //Have move class know what location to move to
			LogMovingToLocations << "Attempting to move:" << j <<": " << ValidPose.getX() << " " <<ValidPose.getY() << endl; // logging locations it will move to
			myMove.NowMove(); //tells the robot to move to a location
			myValueMap.StartRecurrentTask(); // draws and updates grid map of seen locations in own thead
		 //   boost::thread* DisplayThread = new boost::thread(boost::bind(&GridMap::WrappedFovDisplay, &myValueMap));
		    boost::thread CameraThread(CameraFindObject, &myMove, &objectnotfound); //camera processing in own thread
			myMove.WaitMovePathCondition(); // pauses main thread  ArPathPlan goal reached or failed call the signal
			myValueMap.StopRecurrentTask(); // stops drawing and updating grid map
			CameraThread.interrupt(); // the thread should be closed by now, here just incase something happens
			if(objectnotfound == true){ // object not found
				printf("Object found == FALSE\n");
				//CameraThread.interrupt(); // the thread should be closed by now, here just incase something happens
				//DisplayThread.interrupt();
			}
			else if(objectnotfound == false){ // Object is found
				printf("Object found == TRUE\n");
				pathTask.endPathPlanToLocalPose(false);//pathTask.cancelPathPlan();
				//CameraThread.interrupt(); // the thread should be closed by now, here just incase something happens
				//DisplayThread.interrupt();
				goto TargetFound; // object found, exit out of switch statement and into next while below
			}

			double temp_num = locTask.getLocalizationScore();
			printf("Localization Score = %f\n",temp_num);
			//while(temp_num < 0.2){ // localizes the robot to posloc ArPose below
				robot.moveTo(ArPose(ValidPose.getX(), ValidPose.getY(), robot.getTh()));
				locTask.forceUpdatePose(ArPose(ValidPose.getX(), ValidPose.getY(), robot.getTh()));
			//	temp_num = locTask.getLocalizationScore();
			//}

			if(myMove.GetPathState() == 1){// goal reached go to state 0
				printf("Restarting Algorithm\n");
				myServer.resetMovingNow();
				goto StartOfAlgorithm;

			}
			else if (myMove.GetPathState() == 2){//failed to get to goal --
				printf("Valid Location Failed Try next Valid Location\n");
				//i++;
				if(j == 5){
					printf("I SHOULD NEVER EVER EVER SEE THIS!!! IN MAIN \n");
				}
			}
		}// end of for loop





	} // end of Large While Algorithm loop

TargetFound: // i found the target and i leave the while
	robot.deactivateActions(); //leave this in
	while(1){
		printf("P3at has found target!\n");
		robot.remUserTask("ReviewBids"); //remove recurring user task
		robot.waitForRunExit();
	}



	////////////////////////////////////////////// THESE MUST BE THE LAST LINES IN MAIN()
    // Main message loop
    do {
        //process Windows messages first
        if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage (&msg);
            DispatchMessage (&msg);
        } else if (ecode = FrameGrabber (FG_GRAB)) {		//grab a frame if no messages in queue
			ErrAbort (ecode);
        }
    } while (msg.message != WM_QUIT);
	DestroyIcon (wndclass.hIcon);
	DestroyCursor (wndclass.hCursor);
	robot.waitForRunExit(); // for aria
    Aria::exit(0);			// for aria
    return msg.wParam;

}

void NoConfirmBidWins(EvaluateTask *myEvaluateTask, myServerInfo *myServer){
///version 1f
	int CatchCount = 0;
	bool CatchBool;
	CatchStart: // created while loop
	if(CatchCount < 7){
		CatchCount++;
		printf("Waiting %d\n",CatchCount);
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		//ArUtil::sleep(1000);
		CatchBool = myServer->ReturnConfirmTaskBool();
		if(CatchBool == false){
			goto CatchStart; // means movenowsignal has not been called yet
		}
		else if(CatchBool == true){
			myServer->SignalServerMoveNowCondition();
			goto CatchEnd;//means movenowsignal has been called this function is not needed anymore
		}
	}
	else if (CatchCount >= 7){
		//goto CatchEnd; // movenowsignals has been ignored or no confirm received move to a location
		//grab the saved self location
		//get max utility location from self
		vector<tasks> mynewloc = myEvaluateTask->ReturnMyOwnBids();
		mynewloc = myEvaluateTask->GrabMaxBid(mynewloc,1);

		if(mynewloc.size() == 0){
			goto CatchStart;
		}
		//put values into server.currentTaskExport variable
		myServer->CurrentTaskExport = mynewloc[0];
		// move to CatchEnd:
		goto CatchEnd;
	}
	CatchEnd:;
	myServer->SignalServerMoveNowCondition();

}

bool CheckPathStateNextMove(MoveToLocation *move,myServerInfo *server, int &word){ // no longer used
	if(move->GetPathState() == 1){// goal reached go to state 0
		printf("GOING TO CASE 0\n");
		server->resetMovingNow();
		word = 0;
		return true;
	}
	else if (move->GetPathState() == 2){//failed to get to goal -- go to state 4
		printf("GOING TO CASE 3\n");
		word = 3;
		return false;
	}
	else{
		printf("ERROR ERROR IN MAIN CASE 3 GET PATH STATE!!!!!!!\n");
		printf("RESTARTING ALGORITHM\n");
		server->resetMovingNow();
		word = 0;
		return true;
	}
}



bool Objectfoundcheck(int x){
	if(x > 15000){
		return false;
	}
	else{
		return true;
	}
}

// Amirali Methods
void CameraFindObject(MoveToLocation *Move, bool *objectnotfound){
	//initializations
    Vision Target;
	int Max_Blob_Size = 0;
	int Next_Max = 0;
	double Redness, Blueness, Greenness;
	int blue, green, red;

	//printf("The CAMERA IS WORKING\n");
	while(Max_Blob_Size < MIN_TARGET_SIZE_IN_THE_IMAGE){
		FrameGrabber (FG_STOP); // Stop the Framegrabber
	//	FrameGrabber (FG_SAVE); // Save the Framegrabber
		FrameGrabber (FG_START); // Start the Framegraber
		boost::this_thread::sleep(boost::posix_time::milliseconds(1400));
		FormatConvertor();



	for (int h = 0; h < img->height; h++)
	{
		for (int w = 0; w < (img->width * 1); w++)
		{
			/*uchar * blueptr = new uchar;
			blueptr = (uchar *)(img->imageData + h*img->widthStep);
			blue = blueptr[w*img->nChannels + 0];
			delete blueptr;

			uchar * redptr = new uchar;
			redptr = (uchar *)(img->imageData + h*img->widthStep);
			red = redptr[w*img->nChannels + 2];
			delete redptr;

			uchar * greenptr = new uchar;
			greenptr = (uchar *)(img->imageData + h*img->widthStep);
			green = greenptr[w*img->nChannels + 1];
			delete greenptr;*/

			blue = ((uchar *)(img->imageData + h*img->widthStep))[w*img->nChannels + 0];
			green = ((uchar *)(img->imageData + h*img->widthStep))[w*img->nChannels + 1];
			red = ((uchar *)(img->imageData + h*img->widthStep))[w*img->nChannels + 2];
			//Pixel=cvGet2D(img,h,w); // get the (i,j) pixel value
			Redness = Target.Color_Intensity(red, blue, green);//Target.Color_Intensity(Pixel.val[2], Pixel.val[1], Pixel.val[0]);
			Blueness = Target.Color_Intensity(blue, red, green);//Target.Color_Intensity(Pixel.val[0], Pixel.val[2], Pixel.val[1]);
			Greenness = Target.Color_Intensity(green, red, blue);//Target.Color_Intensity(Pixel.val[1], Pixel.val[0], Pixel.val[2]);
			// Check the redness
			if (Redness != -1)
			{
				if (Redness < REDNESS)
				{
				cvSet2D(img,h,w,CV_RGB(0x00,0x00,0x00)); // set the (i,j) pixel value
				}
			}
			else // Pixel.val[1] + Pixel.val[0] == 0
			{
				if (red < RED_PIXEL) //(Pixel.val[2] < RED_PIXEL)
				{
					cvSet2D(img,h,w,CV_RGB(0x00,0x00,0x00)); // set the (i,j) pixel value
				}
			}
			// Check the blueness
			if (Blueness != -1)
			{
				if (Blueness < BLUENESS)
				{
				cvSet2D(img,h,w,CV_RGB(0x00,0x00,0x00)); // set the (i,j) pixel value
				}
			}
			else // Pixel.val[1] + Pixel.val[0] == 0
			{
				if (blue < BLUE_PIXEL) //Pixel.val[1] < BLUE_PIXEL
				{
					cvSet2D(img,h,w,CV_RGB(0x00,0x00,0x00)); // set the (i,j) pixel value
				}
			}
			// Check the greenness
			if (Greenness != -1)
			{
				if (Greenness < GREENNESS)
				{
				cvSet2D(img,h,w,CV_RGB(0x00,0x00,0x00)); // set the (i,j) pixel value
				}
			}
			else // Pixel.val[1] + Pixel.val[0] == 0
			{
				if (green < GREEN_PIXEL) //Pixel.val[1] < GREEN_PIXEL
				{
					cvSet2D(img,h,w,CV_RGB(0x00,0x00,0x00)); // set the (i,j) pixel value
				}
			}
		}
	}


	///////////////////////////////////////////////////////////////////


		IplImage* foreground_mask = cvCreateImage(cvSize(img->width, img->height), 8, 1);
		cvCvtColor(img, foreground_mask, CV_RGB2GRAY);
		foreground_mask = doPyrDown(foreground_mask, IPL_GAUSSIAN_5x5);

		IplImage* connected_components_img = cvCreateImage(cvGetSize(foreground_mask), IPL_DEPTH_16S, 1);
		vector<CvConnectedComp> blobs = Target.labelBlobs(foreground_mask, connected_components_img);

		Max_Blob_Size = 0;
		for(unsigned int i = 0; i < blobs.size() ; i++){
				if (blobs.at(i).area > Max_Blob_Size){
					Next_Max = Max_Blob_Size;
					Max_Blob_Size = blobs.at(i).area;
				}
		}

	//	cvReleaseImage(&img);
		cvReleaseImage(&foreground_mask);
		cvReleaseImage(&connected_components_img);
		if(Max_Blob_Size >= 10000){
			printf("Max_Blob_Size = %d\n", Max_Blob_Size);
		}
		if(Max_Blob_Size >=MIN_TARGET_SIZE_IN_THE_IMAGE){ // stop this thread and make main move forward also change a bool in main
				*objectnotfound = false;
				printf("Object found!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! in Calculate fitness\n");
				Move->SignalMovePathCondition();
				break;
		}

		//DisplayImage->FOVDipslayThread();

		if(Move->ContinueThread == true){} //stay in this functions while
		else if(Move->ContinueThread == false){break;} // path failed or reached has been called changin this var we exit out of this whole function
		else{printf("ERROR IN CALCULATEFINTESS!!!!!!!!!!!!!!\n");}
	//	boost::this_thread::sleep(boost::posix_time::milliseconds(250));

	}// end of while loop

}// end of calculate fitness

IplImage* doPyrDown(IplImage* in, int filter)
{
	// Best to make sure input image is divisible by two. //IPL_GAUSSIAN_5x5
	//
	assert( in->width%2 == 0 && in->height%2 == 0 );
	IplImage* out = cvCreateImage(
	cvSize( in->width/2, in->height/2 ), in->depth, in->nChannels );
	cvPyrDown( in, out );
	return( out );
};

void FormatConvertor()
{
	img = cvCreateImage(cvSize(lpbmi->bmiHeader.biWidth, lpbmi->bmiHeader.biHeight), IPL_DEPTH_8U, 3);
	cvSetData( img, lpvbits, lpbmi->bmiHeader.biWidth * 3);
	cvFlip(img, img, 0);
	//cvNamedWindow( "Example1", CV_WINDOW_AUTOSIZE );
	//cvMoveWindow("Example1", 0, 0); // moves the window on screen
	//cvShowImage( "Example1", img );
	//cvWaitKey(1000);
	//cvDestroyWindow( "Example1" );
	//IplImage* psuedo_color_blob_img = cvCreateImage(cvGetSize(foreground_mask), IPL_DEPTH_8U, 3);

}









// Window procedure
LRESULT WINAPI WndProc (HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;
    ECODE ecode;
    static HMENU hmenu;

    switch (message) {
        case WM_CREATE:
            hmenu = GetMenu (hwnd);
            CheckMenuItem (hmenu, FG_SCALE8, MF_CHECKED);
            CheckMenuItem (hmenu, FG_COLOR, MF_CHECKED);
            CheckMenuItem (hmenu, FG_CHAN0, MF_CHECKED);
            CheckMenuItem (hmenu, FG_NTSC, MF_CHECKED);
            return 0;

        case WM_PAINT:
            BeginPaint (hwnd, &ps);
            //display bitmap
            hdc = GetDC (hwnd);
            GetClientRect (hwnd, (LPRECT) &rect);
            SetDIBitsToDevice (hdc, rect.left, rect.top,
                               rect.right - rect.left + 1,
                               rect.bottom - rect.top + 1,
                               0, 0, 0,
                               (UINT) lpbmi->bmiHeader.biHeight,
                               lpvbits, lpbmi,
                               DIB_RGB_COLORS);
            ReleaseDC (hwnd, hdc);
            //
            EndPaint (hwnd, &ps);
            break;

		case WM_COMMAND:
            //process menu commands
            switch (wParam) {
                case FG_EXIT:
                    FrameGrabber (FG_STOP);
                    FrameGrabber (FG_QUIT);
                    PostMessage (hwnd, WM_QUIT, 0, 0L);
                    break;

                case FG_STOP:
                    FrameGrabber (FG_STOP);
                    break;

                case FG_START:
                    FrameGrabber (FG_START);
                    break;

                case FG_SCALE + SCALE2:
                case FG_SCALE + SCALE4:
                case FG_SCALE + SCALE6:
                case FG_SCALE + SCALE8:
                    FrameGrabber (FG_STOP);
                    if ((ecode = FrameGrabber (wParam))) {
                        ErrAbort (ecode);
                    } else {
                        //check selected menu item
                        CheckMenuItem (hmenu, FG_SCALE2, MF_UNCHECKED);
                        CheckMenuItem (hmenu, FG_SCALE4, MF_UNCHECKED);
                        CheckMenuItem (hmenu, FG_SCALE6, MF_UNCHECKED);
                        CheckMenuItem (hmenu, FG_SCALE8, MF_UNCHECKED);
                        CheckMenuItem (hmenu, wParam, MF_CHECKED);
                    }
                    FrameGrabber (FG_START);
                    break;

                case FG_CHAN0:
                case FG_CHAN1:
                case FG_CHAN2:
                case FG_CHAN3:
                case FG_CHAN4:
                    FrameGrabber (wParam);
                    //check selected menu item
                    CheckMenuItem (hmenu, FG_CHAN0, MF_UNCHECKED);
                    CheckMenuItem (hmenu, FG_CHAN1, MF_UNCHECKED);
                    CheckMenuItem (hmenu, FG_CHAN2, MF_UNCHECKED);
                    CheckMenuItem (hmenu, FG_CHAN3, MF_UNCHECKED);
                    CheckMenuItem (hmenu, FG_CHAN4, MF_UNCHECKED);
                    CheckMenuItem (hmenu, wParam, MF_CHECKED);
                    break;

                case FG_COLOR:
                case FG_MONO:
                    FrameGrabber (FG_STOP);
                    if ((ecode = FrameGrabber (wParam))) {
                        ErrAbort (ecode);
                    } else {
                        //check selected menu item
                        CheckMenuItem (hmenu, FG_MONO, MF_UNCHECKED);
                        CheckMenuItem (hmenu, FG_COLOR, MF_UNCHECKED);
                        CheckMenuItem (hmenu, wParam, MF_CHECKED);
                    }
                    FrameGrabber (FG_START);
                    break;

				case FG_NTSC:
				case FG_PALSECAM:
					FrameGrabber (FG_STOP);
                    if ((ecode = FrameGrabber (wParam))) {
                        ErrAbort (ecode);
                    } else {
                        //check selected menu item
                        CheckMenuItem (hmenu, FG_NTSC, MF_UNCHECKED);
                        CheckMenuItem (hmenu, FG_PALSECAM, MF_UNCHECKED);
                        CheckMenuItem (hmenu, wParam, MF_CHECKED);
                    }
                    FrameGrabber (FG_START);
                    break;

                case FG_SAVE:
                    FrameGrabber (FG_STOP);
                    FrameGrabber (FG_SAVE);
                    FrameGrabber (FG_START);
                    break;
			}
			return 0;

        case WM_DESTROY:
            FrameGrabber (FG_QUIT);
			DestroyMenu (hmenu);
            PostQuitMessage (0);
            return 0;
    }

    return DefWindowProc (hwnd, message, wParam, lParam);
}

/**********************************************************
 * Error Handler
 **********************************************************/
void ErrAbort (ECODE ecode)
{
    char str[60];

    sprintf (str, "Error %d. Abort application?", ecode);
    if (MessageBox (hwnd, str, "SX11 Error", MB_YESNO | MB_ICONSTOP) == IDYES) {
        SendMessage (hwnd, WM_DESTROY, 0, 0L);
    }
    return;
}

/**********************************************************
 * File save procedure
 **********************************************************/
void SaveBMP (BOOL color)
{
    OPENFILENAME ofn;
    BITMAPFILEHEADER bfh;
    HANDLE hfile;
    DWORD byteswritten;
    BOOL ecode = FALSE;
    long bmpsize = lpbmi->bmiHeader.biWidth *
                   lpbmi->bmiHeader.biHeight *
                   lpbmi->bmiHeader.biBitCount / 8;
    const long SIZE_BFH = sizeof (BITMAPFILEHEADER);
    const long SIZE_BIH = sizeof (BITMAPINFOHEADER);
    const long SIZE_CLR = 256 * sizeof (RGBQUAD);
    static char szFileName [_MAX_PATH] = "image.bmp";
    static char szTitleName [_MAX_FNAME + _MAX_EXT];
    static char * szFilter[] = {"Bitmap Files (*.BMP)\0*.bmp\0"};

    // Fill in the structure for File Save dialog
    ofn.lStructSize = sizeof (OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = szFilter[0];
    ofn.lpstrCustomFilter = NULL;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = _MAX_PATH;
    ofn.lpstrFileTitle = szTitleName;
    ofn.nMaxFileTitle = _MAX_FNAME + _MAX_EXT;
    ofn.lpstrDefExt = "bmp";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

    // Fill in the BITMAPFILEHEADER structure
    bfh.bfType = * (WORD *) "BM";
    bfh.bfOffBits = SIZE_BFH + SIZE_BIH + (color ? 0 : SIZE_CLR);
    bfh.bfSize =  bfh.bfOffBits + bmpsize;

	/*
    // Get file name
    if (!GetSaveFileName (&ofn)) {
        if (CommDlgExtendedError ()) {
            MessageBox (hwnd, "Error saving file.",
                        "SX11 error", MB_ICONSTOP | MB_OK);
        }
        return;
    }
	*/

    // Create a file
    if ((hfile = CreateFile (ofn.lpstrFile, GENERIC_WRITE, 0,
                             NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE,
                             NULL)) == INVALID_HANDLE_VALUE) {
        ecode = TRUE;
    } else {
        // Write BITMAPFILEHEADER to file
        if (!WriteFile (hfile, (LPCVOID) &bfh, SIZE_BFH,
                        &byteswritten, NULL)) {
            ecode = TRUE;
        } else {
            // Write BITMAPINFOHEADER to file
            if (!WriteFile (hfile, (LPCVOID) lpbmi,
                            SIZE_BIH, &byteswritten, NULL)) {
                ecode = TRUE;
            } else {
                // If monochrome image - write color table to file
                if (!color) {
                    if (!WriteFile (hfile,
                                    (LPCVOID) ((BYTE *) lpbmi + SIZE_BIH),
                                    SIZE_CLR, &byteswritten, NULL)) {
                        ecode = TRUE;
                    }
                }
                // Write bitmap data to file
                if (!ecode) {
                    if (!WriteFile (hfile, (LPCVOID) lpvbits, bmpsize,
                                    &byteswritten, NULL)) {
                        ecode = TRUE;
                    }
                }
            }
        }
    }
    CloseHandle (hfile);
    if (ecode) {
        MessageBox (hwnd, "Error saving file.", "SX11 error", MB_ICONSTOP | MB_OK);
    }
    return;
}

/**********************************************************
 * FrameGrabber (fgcommand)
 * Performs frame grabber functions depending on fgcommand:
 *      FG_INIT - initializes the frame grabber;
 *      FG_START - starts continuous acquisition;
 *      FG_GRAB - grabs a frame and requests its display;
 *      FG_STOP - stops acquisition;
 *      FG_SCALEn - rescales the image;
 *      FG_CHANn - selects input channel;
 *      FG_COLOR, FG_MONO - selects color mode;
 *      FG_QUIT - cleans up.
 *      FG_SAVE - saves the image into a file.
 *
 * Returns error code in case of an error,
 * or 0 if successful.
 *********************************************************/
ECODE FrameGrabber (DWORD fgcommand)
{
    PCI pci;
    static BUFFER buffer[2];
	static MODE mode_NTSC = {DEF_MODE_NTSC};
	static MODE mode_PAL = {DEF_MODE_PAL};
    static MODE mode = {DEF_MODE_NTSC};
    static HFG hfg;
    DWORD xsize, ysize;
    static DWORD timer0;
    DWORD timer;
    static DWORD frames = 0;
    static BOOL acqflag = FALSE;
    ECODE ecode;
    char str[80];
    DWORD status, dw1, dw2;

    switch (fgcommand) {

        case FG_INIT:
            //Dynamicaly link the library
			if (ecode = X11_DLLOpen ()) {
				return ecode;
			}
            //Initialize system
            if ((ecode = X11_InitSystem (&pci))) {
                return ecode;
            }
            //Get frame grabber handle
            if ((ecode = X11_GetHFG (&hfg, pci.PCIslot[0]))) {
                return ecode;
            }
            //Allocate 2 image buffers (1 frame each)
            if ((ecode = X11_AllocBuffer (&mode, &buffer[0], 1))) {
                return ecode;
            }
            if ((ecode = X11_AllocBuffer (&mode, &buffer[1], 1))) {
                return ecode;
            }
            //Set up globals
            lpbmi = buffer[current].frame[0].lpbmi;
            lpvbits = buffer[current].frame[0].lpvbits;

            //Set frame grabber mode
            if ((ecode = X11_SetMode (hfg, &mode))) {
                return ecode;
            }
            //Start acquisition
            current = 0;
            acqflag = TRUE;
            timer0 = clock();
            X11_StartAcquire (hfg, buffer[current].hbuf, AMODE_SINGLE);
            return 0;

        case FG_GRAB:
            if (acqflag) {
                //Check if frame ready
                if ((ecode = X11_GetStatus (hfg, &status))) {
                    return ecode;
                }
                if (status & STATUS_READY) {
                    X11_StopAcquire (hfg);
                    X11_ResetStatus (hfg, STATUS_READY);
                    //Set up global pointers
                    lpbmi = buffer[current].frame[0].lpbmi;
                    lpvbits = buffer[current].frame[0].lpvbits;
                    //Toggle active buffer index
                    current = (current ? 0 : 1);
                    //Start acquisition into alternative buffer
                    X11_StartAcquire (hfg, buffer[current].hbuf, AMODE_SINGLE);
                    //Display image
                    SendMessage (hwnd, WM_PAINT, 0, 0L);
                    //Update title bar
                    frames++;
                    timer = clock() - timer0;
                    sprintf (str, "X11 Sample2;   FPS=%4.1f\0", (float) frames * CLOCKS_PER_SEC / timer);
                    SetWindowText (hwnd, (LPCSTR) &str[0]);
				} else if (status & STATUS_ERROR) {					//error during acquisition
		            X11_StartAcquire (hfg, buffer[current].hbuf, AMODE_SINGLE);	//restart capture
				}
            }
            return 0;

        case FG_START:
            acqflag = TRUE;
            timer0 = clock();
            frames = 0;
            X11_StartAcquire (hfg, buffer[current].hbuf, AMODE_SINGLE);
            return 0;

        case FG_STOP:
            acqflag = FALSE;
            X11_StopAcquire (hfg);
            return 0;

        case FG_SCALE8:
        case FG_SCALE6:
        case FG_SCALE4:
        case FG_SCALE2:
            //set mode variable
            mode.scale = fgcommand - FG_SCALE;
            //get new image size
            if ((ecode = X11_GetImageSize(&mode, &xsize, &ysize))) {
                return ecode;
            }
            //set frame grabber mode
            if ((ecode = X11_SetMode (hfg, &mode))) {
                return ecode;
            }
            //allocate new buffers
            X11_FreeBuffer (buffer[0].hbuf);
            X11_FreeBuffer (buffer[1].hbuf);
            if ((ecode = X11_AllocBuffer (&mode, &buffer[0], 1))) {
                return ecode;
            }
            if ((ecode = X11_AllocBuffer (&mode, &buffer[1], 1))) {
                return ecode;
            }
            //set up global pointers
            lpbmi = buffer[current].frame[0].lpbmi;
            lpvbits = buffer[current].frame[0].lpvbits;
            //resize display window
            SetWindowPos (hwnd, HWND_TOP, 0, 0,
                      xsize + GetSystemMetrics (SM_CXBORDER) * 2,
                      ysize + GetSystemMetrics (SM_CYCAPTION) +
                      GetSystemMetrics (SM_CYMENU) *
                      ((mode.scale == SCALE2) ? 2 : 1), SWP_NOMOVE);
            //reset variables
            timer0 = clock();
            frames = 0;
            return 0;


        case FG_CHAN0:
        case FG_CHAN1:
        case FG_CHAN2:
        case FG_CHAN3:
        case FG_CHAN4:
            //select input channel
            switch (fgcommand - FG_CHAN) {
                case 0:
                    mode.input = MUX_0;
                    mode.advanced.LNOTCH = LNOTCH_OFF;
					mode.advanced.CCOMB = CCOMB_OFF;
                    break;
                case 1:
                    mode.input = MUX_1;
                    mode.advanced.LNOTCH = LNOTCH_ON;
					mode.advanced.CCOMB = CCOMB_ON;
                    break;
                case 2:
                    mode.input = MUX_2;
                    mode.advanced.LNOTCH = LNOTCH_ON;
					mode.advanced.CCOMB = CCOMB_ON;
                    break;
                case 3:
                    mode.input = MUX_3;
                    mode.advanced.LNOTCH = LNOTCH_ON;
					mode.advanced.CCOMB = CCOMB_ON;
                    break;
                case 4:
                    mode.input = MUX_4;
                    mode.advanced.LNOTCH = LNOTCH_ON;
					mode.advanced.CCOMB = CCOMB_ON;
                    break;
            }
            X11_SetMode (hfg, &mode);
            return 0;

        case FG_MONO:
            //set monochrome mode
            mode.color = COLOR_MONO;
            if ((ecode = X11_SetMode (hfg, &mode))) {
                return ecode;
            }
            //allocate new buffers
            X11_FreeBuffer (buffer[0].hbuf);
            X11_FreeBuffer (buffer[1].hbuf);
            if ((ecode = X11_AllocBuffer (&mode, &buffer[0], 1))) {
                return ecode;
            }
            if ((ecode = X11_AllocBuffer (&mode, &buffer[1], 1))) {
                return ecode;
            }
            //set up global pointers
            lpbmi = buffer[current].frame[0].lpbmi;
            lpvbits = buffer[current].frame[0].lpvbits;
            return 0;

        case FG_COLOR:
            //set color mode
            mode.color = COLOR_RGB;
            if ((ecode = X11_SetMode (hfg, &mode))) {
                return ecode;
            }
            //allocate new buffers
            X11_FreeBuffer (buffer[0].hbuf);
            X11_FreeBuffer (buffer[1].hbuf);
            if ((ecode = X11_AllocBuffer (&mode, &buffer[0], 1))) {
                return ecode;
            }
            if ((ecode = X11_AllocBuffer (&mode, &buffer[1], 1))) {
                return ecode;
            }
            //set up global pointers
            lpbmi = buffer[current].frame[0].lpbmi;
            lpvbits = buffer[current].frame[0].lpvbits;
            return 0;

		case FG_NTSC:
		case FG_PALSECAM:
			//keep old input and scale
			dw1 = mode.input;
			dw2 = mode.scale;
			mode = ((fgcommand == FG_NTSC) ? mode_NTSC : mode_PAL);
			mode.input = dw1;
			mode.scale = dw2;

            //get new image size
            if ((ecode = X11_GetImageSize(&mode, &xsize, &ysize))) {
                return ecode;
            }
            //set frame grabber mode
            if ((ecode = X11_SetMode (hfg, &mode))) {
                return ecode;
            }
            //allocate new buffers
            X11_FreeBuffer (buffer[0].hbuf);
            X11_FreeBuffer (buffer[1].hbuf);
            if ((ecode = X11_AllocBuffer (&mode, &buffer[0], 1))) {
                return ecode;
            }
            if ((ecode = X11_AllocBuffer (&mode, &buffer[1], 1))) {
                return ecode;
            }
            //set up global pointers
            lpbmi = buffer[current].frame[0].lpbmi;
            lpvbits = buffer[current].frame[0].lpvbits;
            //resize display window
            SetWindowPos (hwnd, HWND_TOP, 0, 0,
                      xsize + GetSystemMetrics (SM_CXBORDER) * 2,
                      ysize + GetSystemMetrics (SM_CYCAPTION) +
                      GetSystemMetrics (SM_CYMENU) *
                      ((mode.scale == SCALE2) ? 2 : 1), SWP_NOMOVE);
            //reset variables
            timer0 = clock();
            frames = 0;
            return 0;

        case FG_QUIT:

			is_recording = FALSE;

            acqflag = FALSE;
            X11_StopAcquire (hfg);
            //clean up
            X11_CloseSystem ();
			X11_DLLClose();
            return 0;

        case FG_SAVE:
           // SaveBMP (mode.color == COLOR_RGB);
            return 0;

        default:
            return 0;

    }
}
ArPose MoveToValidLocation(){
	//printf("Size of valid locations = %d\n", ValidLocation.size());

	ArPose myValidPose;
    int random_integer;
    int lowest = 1;
	int highest = ValidLocation.size();
    int range = (highest - lowest) + 1;
    /*for(int index = 0; index < 20; index++){
        random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));
        cout << random_integer << endl;
    } */
	random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));
	myValidPose = ValidLocation[random_integer];
	printf("My Random Index and Location = %d %f %f\n", random_integer, myValidPose.getX(),myValidPose.getY());

return myValidPose;

}
void PopulateValidLocation(){
	ValidLocation.push_back(ArPose(1611,-7111));
	ValidLocation.push_back(ArPose(1569,-6631));
	ValidLocation.push_back(ArPose(2070,-6610));
	ValidLocation.push_back(ArPose(2175,-7237));
	ValidLocation.push_back(ArPose(2738,-7258));
	ValidLocation.push_back(ArPose(3427,-7258));
	ValidLocation.push_back(ArPose(4075, -7362));
	ValidLocation.push_back(ArPose(4638,-7320));
	ValidLocation.push_back(ArPose(5474,-7362));
	ValidLocation.push_back(ArPose(6163,-7341));
	ValidLocation.push_back(ArPose(6163,-6485));
	ValidLocation.push_back(ArPose(5515,-6485));
	ValidLocation.push_back(ArPose(4847,-6485));
	ValidLocation.push_back(ArPose(4012,-6527));
	ValidLocation.push_back(ArPose(3198,-6469));
	ValidLocation.push_back(ArPose(2509,-6610));
	ValidLocation.push_back(ArPose(1882,-6610));
	ValidLocation.push_back(ArPose(2342,-5880));

	ValidLocation.push_back(ArPose(2342,-5880));
	ValidLocation.push_back(ArPose(3114,-5901));
	ValidLocation.push_back(ArPose(3866,-5859));
	ValidLocation.push_back(ArPose(4555,-5901));
	ValidLocation.push_back(ArPose(5369,-5963));
	ValidLocation.push_back(ArPose(6142,-6047));

	ValidLocation.push_back(ArPose(5369,-5191));
	ValidLocation.push_back(ArPose(4680,-5170));
	ValidLocation.push_back(ArPose(3866,-5149));
	ValidLocation.push_back(ArPose(3135,-5086));
	ValidLocation.push_back(ArPose(2446,-5086));

	ValidLocation.push_back(ArPose(2488,-4481));
	ValidLocation.push_back(ArPose(3114,-4481));
	ValidLocation.push_back(ArPose(4054,-4460));
	ValidLocation.push_back(ArPose(4722,-4918));
	ValidLocation.push_back(ArPose(5390,-4481));

	ValidLocation.push_back(ArPose(2217,-4021));
	ValidLocation.push_back(ArPose(2488,-4021));
	ValidLocation.push_back(ArPose(3240,-3480));
	ValidLocation.push_back(ArPose(3553,-3854));
	ValidLocation.push_back(ArPose(4158,-3875));
	ValidLocation.push_back(ArPose(4471,-3896));
	ValidLocation.push_back(ArPose(4973,-3917));

	ValidLocation.push_back(ArPose(2217,-3374));
	ValidLocation.push_back(ArPose(2885,-3374));
	ValidLocation.push_back(ArPose(3344,-3395));

	ValidLocation.push_back(ArPose(-58,-2852));
	ValidLocation.push_back(ArPose(484,-2852));
	ValidLocation.push_back(ArPose(1256,-2873));
	ValidLocation.push_back(ArPose(1903,-2852));
	ValidLocation.push_back(ArPose(2801,-2852));
	ValidLocation.push_back(ArPose(3302,-2894));

	ValidLocation.push_back(ArPose(3135,-2247));
	ValidLocation.push_back(ArPose(2467,-2205));
	ValidLocation.push_back(ArPose(1841,-2184));
	ValidLocation.push_back(ArPose(1152,-2163));
	ValidLocation.push_back(ArPose(713,-2141));

	ValidLocation.push_back(ArPose(-16,-2121));
	ValidLocation.push_back(ArPose(-79,-1641));
	ValidLocation.push_back(ArPose(442,-1620));
	ValidLocation.push_back(ArPose(1047,-2101));
	ValidLocation.push_back(ArPose(1528,-2059));
	ValidLocation.push_back(ArPose(2049,-2059));

	ValidLocation.push_back(ArPose(2864,-2101));
	ValidLocation.push_back(ArPose(2885,-1537));
	ValidLocation.push_back(ArPose(2342,-1495));
	ValidLocation.push_back(ArPose(1841,-1495));
	ValidLocation.push_back(ArPose(1298,-1474));
	ValidLocation.push_back(ArPose(755,-1432));
	ValidLocation.push_back(ArPose(45,-1412));

	ValidLocation.push_back(ArPose(-58,-931));
	ValidLocation.push_back(ArPose(567,-911));
	ValidLocation.push_back(ArPose(1068,-911));
	ValidLocation.push_back(ArPose(1715,-911));
	ValidLocation.push_back(ArPose(2404,-952));
	ValidLocation.push_back(ArPose(3054,-931));

	ValidLocation.push_back(ArPose(3699,-430));
	ValidLocation.push_back(ArPose(3156,-368));
	ValidLocation.push_back(ArPose(2592,-347));
	ValidLocation.push_back(ArPose(2091,-347));
	ValidLocation.push_back(ArPose(1548,-284));
	ValidLocation.push_back(ArPose(1047,-263));
	ValidLocation.push_back(ArPose(651,-253));
	ValidLocation.push_back(ArPose(24,-242));

	ValidLocation.push_back(ArPose(-100,592));
	ValidLocation.push_back(ArPose(442,613));
	ValidLocation.push_back(ArPose(1110,592));
	ValidLocation.push_back(ArPose(1841,550));
	ValidLocation.push_back(ArPose(2425,508));
	ValidLocation.push_back(ArPose(3073,446));
	ValidLocation.push_back(ArPose(3386,466));
	ValidLocation.push_back(ArPose(374,487));

}
