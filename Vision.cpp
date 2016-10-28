#include "StdAfx.h"
#include "Vision.h"

//void Vision::set_values(long Red_Pixel, long Redness, long Min_Target_Size_In_The_Image)
//{
//	red_pixel = Red_Pixel;
//	redness = Redness;
//	min_target_size_in_the_image = Min_Target_Size_In_The_Image;
//}
Vision::Vision(){

	/*MIN_TARGET_SIZE_IN_THE_IMAGE  = 100000;
	REDNESS =  5;
	RED_PIXEL = 220;
	REDNESS = 5;
	BLUE_PIXEL = 110;
	BLUENESS = 3;
	GREEN_PIXEL = 220;
	GREENNESS = 2;
	ContinueThread = false;*/
/*
	LRESULT WINAPI WndProc (HWND, UINT, UINT, LONG);    // Window procedure
ECODE FrameGrabber (DWORD);                 // Frame grabber procedure
void ErrAbort (ECODE);                      // Error handler
void SaveBMP (BOOL);                        // File save procedure
*/
//ECODE FrameGrabber (DWORD);                 // Frame grabber procedure
//ECODE ecode;
//char str[80];
//char szAppName [] = "X11 Sample2";
	    // Initialize frame grabber
  //  if (ecode = FrameGrabber (FG_INIT)) {
      //  sprintf (str, "Initialization failed; error %d", ecode);
    //    MessageBox (hwnd, str, szAppName, MB_OK | MB_ICONSTOP);
     //   SendMessage (hwnd, WM_DESTROY, 0, 0L);
      //  return 0;
 //   }
 //   UpdateWindow (hwnd);
//	FrameGrabber (FG_CHAN1); // ## Calling framegrabber specific to channel 1 ##

}
Vision::~Vision(){}
/*void Vision::task(){

	double area_of_redness = 0;
//	area_of_redness = calculateFitness();

	printf("Area of redness fintess value = %d\n",area_of_redness);

	if(ContinueThread == true){
		go();
	}
	else if(ContinueThread == false){
		// do nothing
	}
	else{
		printf("ERROR IN MYDISPLAY TASK CONTINUETHREAD CONDITION\n");
	}

}*/

void Vision::StartRecurrentTask(){
	ContinueThread = true;
	printf("Starting Vision\n");
//	go();
}

void Vision::StopRecurrentTask(){
	ContinueThread = false;
	printf("Stopping vision\n");
}

CvScalar Vision::getRandomColor(int seed) 
{
  srand(seed);
  return cvScalar(rand() % 255, rand() % 255, rand() % 255);
}

std::vector<CvConnectedComp> Vision::labelBlobs(IplImage const* const src, IplImage* dst) 
{
  cvSetZero(dst);
  int h = src->height;
  int w = src->width;
  // Initialize array of parent/child label associations
  int label_tree[MAX_LABELS];
  for (int i = 0; i < MAX_LABELS; ++i) 
  {
    label_tree[i] = 0;
  }
  int color = 0;
  // Iterate through every pixel
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; x++) {
      // If this is a foreground pixel, check the top and left neighbors to see
      // if they are different classes
      if (CV_IMAGE_ELEM(src, uchar, y, x) != 0) {
        int parent_label = 0, child_label = 0, top_label = 0, left_label = 0; // temp vars

        // Top
        if (y > 0) {
          top_label = CV_IMAGE_ELEM(dst, __int16, y-1, x);
        }
        // Left
        if (x > 0) {
          left_label = CV_IMAGE_ELEM(dst, __int16, y, x-1);
        }

        // If this foreground pixel has a top or left neighbor, assign parent
        // as the smaller of the two; if they differ, then update the union-find structure
        if (top_label != 0 || left_label != 0) {

          // Both top and left neighbors have same label => this pixel has same label
          if (top_label != 0 && top_label == left_label) {
            parent_label = child_label = top_label;

          // Top is the only valid label
          } else if (top_label != 0 && left_label == 0) {
            parent_label = child_label = top_label;

          // Left is the only valid label
          } else if (left_label != 0 && top_label == 0) {
            parent_label = child_label = left_label;

          // Top and left are different and top has smaller label => assign child_label as bigger of the two
          } else if (top_label != 0 && top_label < left_label) {
            parent_label = top_label;
            child_label = left_label;

          // Top and left are different and left has smaller label => assign child_label as bigger of the two
          } else if (left_label != 0 && left_label < top_label) {
            parent_label = left_label;
            child_label = top_label;
          }
        }

        // If parent and child not assigned, increment color and assign
        if (parent_label == 0 && child_label == 0) {
          CV_IMAGE_ELEM(dst, __int16, y, x) = ++color;

        // If parent and child are the same, set this pixel as same color
        } else if (parent_label == child_label && parent_label != 0) {
          label_tree[child_label] = parent_label;
          CV_IMAGE_ELEM(dst, __int16, y, x) = parent_label;
         
        // Parent and child are different, so go up the tree to find the root parent label
        } else {
          // Keep traversing the tree until you reach the root (i.e., the label's parent is itself)
          while (label_tree[parent_label] != parent_label) {
            parent_label = label_tree[parent_label];
          }
          
          label_tree[child_label] = parent_label;
          CV_IMAGE_ELEM(dst, __int16, y, x) = parent_label;
        }
      }
    }
  }

  long blob_sizes[MAX_LABELS]; // used to keep track of area of each blob
  int leftmost_corners[MAX_LABELS];   // used to construct
  int topmost_corners[MAX_LABELS];    //   rectangle
  int rightmost_corners[MAX_LABELS];  //   of interest
  int bottommost_corners[MAX_LABELS]; //   of each blob

  for (int i = 0; i < MAX_LABELS; ++i) {
    blob_sizes[i] = 0;
    leftmost_corners[i] = w;
    topmost_corners[i] = h;
    rightmost_corners[i] = 0;
    bottommost_corners[i] = 0;
  }


  // Second pass adjusts all labels
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int label = CV_IMAGE_ELEM(dst, __int16, y, x);
      if (label != 0) {
        // Find root of this tree
        while (label != label_tree[label]) {
          label = label_tree[label];
        }
        CV_IMAGE_ELEM(dst, __int16, y, x) = label;

        // Keep track of blob sizes
        ++blob_sizes[label];

        // Keep track of bounding box
        if (x < leftmost_corners[label])   { leftmost_corners[label] = x; }
        if (x > rightmost_corners[label])  { rightmost_corners[label] = x; }
        if (y < topmost_corners[label])    { topmost_corners[label] = y; }
        if (y > bottommost_corners[label]) { bottommost_corners[label] = y; }
      }
    }
  }

  // Construct vector of all valid blobs
  std::vector<CvConnectedComp> blobs;
  for (int i = 0; i < MAX_LABELS; ++i) 
  {
    if (blob_sizes[i] > 0) 
	{
      CvConnectedComp blob;
      blob.value = cvScalar(i);
      blob.area = blob_sizes[i];
      blob.rect = cvRect(
        leftmost_corners[i] // x
      , topmost_corners[i] // y
      , rightmost_corners[i] - leftmost_corners[i] // width
      , bottommost_corners[i] - topmost_corners[i] // height
      );
      blobs.push_back(blob);
    }
  }

  return blobs;
}

IplImage* Vision::labels2psuedoColor(IplImage const* src) 
{
  // Initialize destination image
  IplImage* dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
  cvSetZero(dst);

  int h = src->height;
  int w = src->width;

  // Cache labels and corresponding CvScalar for speed
  int previous_label = 0;
  CvScalar color;

  for (int y = 0; y < h; ++y) 
  {
    for (int x = 0; x < w; ++x) 
	{
      int label = CV_IMAGE_ELEM(src, __int16, y, x);
      if (label != 0) 
	  {
        // Use cached color if possible to avoid thousands of redundant
        // calls to rand() and srand() in getRandomColor()
        if (label != previous_label) 
		{
          color = getRandomColor(label);
          previous_label = label;
        }
        CV_IMAGE_ELEM(dst, uchar, y, x*3) = (int)color.val[0];
        CV_IMAGE_ELEM(dst, uchar, y, x*3+1) = (int)color.val[1];
        CV_IMAGE_ELEM(dst, uchar, y, x*3+2) = (int)color.val[2];
      }
    }
  }
  return dst;
}
double Vision::Color_Intensity(double Base, double RGB1 ,double RGB2)
{
	double temp;
	temp = RGB1 + RGB2;
	if (temp != 0)
		return ((2*Base)/(RGB1 + RGB2));
	else
		return -1;
}
/*
double Vision::calculateFitness()
{
		//			FrameGrabber (FG_STOP);
		//		FrameGrabber (FG_SAVE);
			//	FrameGrabber (FG_START);
  //  Vision Target;
	int Max_Blob_Size = 0;
	int Next_Max = 0;
	int i;
	IplImage* img = cvLoadImage("image.bmp");
	//img = doPyrDown(img, IPL_GAUSSIAN_5x5);

///////////////////////////////////////////////////////////////
	
	CvScalar R_Pixel, G_Pixel, B_Pixel, Pixel;
	double R_Value, G_Value, B_Value;
	double Redness, Blueness, Greenness;
	// we need at least one Black Pixel
	cvSet2D(img,0,0,CV_RGB(0x00,0x00,0x00));
	for (int h = 0; h < img->height; h++)
	{
		for (int w = 0; w < (img->width * 1); w++)
		{
			Pixel=cvGet2D(img,h,w); // get the (i,j) pixel value
			Redness = Color_Intensity(Pixel.val[2], Pixel.val[1], Pixel.val[0]); 
			Blueness = Color_Intensity(Pixel.val[0], Pixel.val[2], Pixel.val[1]);
			Greenness = Color_Intensity(Pixel.val[1], Pixel.val[0], Pixel.val[2]);
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
				if (Pixel.val[2] < RED_PIXEL)
				{
					cvSet2D(img,h,w,CV_RGB(0x00,0x00,0x00)); // set the (i,j) pixel value
				}
			}
			// Check the blueness
			//if (Blueness != -1)
			//{
			//	if (Blueness < BLUENESS)
			//	{
			//	cvSet2D(img,h,w,CV_RGB(0x00,0x00,0x00)); // set the (i,j) pixel value
			//	}
			//}
			//else // Pixel.val[1] + Pixel.val[0] == 0
			//{
			//	if (Pixel.val[1] < BLUE_PIXEL)
			//	{
			//		cvSet2D(img,h,w,CV_RGB(0x00,0x00,0x00)); // set the (i,j) pixel value
			//	}
			//}
			// Check the greenness
			//if (Greenness != -1)
			//{
			//	if (Greenness < GREENNESS)
			//	{
			//	cvSet2D(img,h,w,CV_RGB(0x00,0x00,0x00)); // set the (i,j) pixel value
			//	}
			//}
			//else // Pixel.val[1] + Pixel.val[0] == 0
			//{
			//	if (Pixel.val[1] < GREEN_PIXEL)
			//	{
			//		cvSet2D(img,h,w,CV_RGB(0x00,0x00,0x00)); // set the (i,j) pixel value
			//	}
			//}
		} // end of for loop
	} // end of for loop


	//cvNamedWindow( "Example1", CV_WINDOW_AUTOSIZE );
	//cvMoveWindow("Example1", 0, 0); // moves the window on screen
	//cvShowImage( "Example1", img );
	//cvWaitKey(0);
	//cvDestroyWindow( "Example1" );

///////////////////////////////////////////////////////////////////

	//IplImage* foreground_mask = cvLoadImage("C:\\Users\\amirali\\Desktop\\Pretty_Girl_23.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* foreground_mask = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	cvCvtColor(img, foreground_mask, CV_RGB2GRAY);
	foreground_mask = doPyrDown(foreground_mask, IPL_GAUSSIAN_5x5);
	//cvShowImage("image:",foreground_mask);
    //cvWaitKey();

	IplImage* connected_components_img = cvCreateImage(cvGetSize(foreground_mask), IPL_DEPTH_16S, 1);
	vector<CvConnectedComp> blobs = labelBlobs(foreground_mask, connected_components_img);
	//printf("Blob Size: %d\n",blobs.size());
	for (int i = 0; i < blobs.size() ; i++)
		{
			if (blobs.at(i).area > Max_Blob_Size)
			{
				Next_Max = Max_Blob_Size;
				Max_Blob_Size = blobs.at(i).area;
			}
		}
	//cout << "Max Area" << Max_Blob_Size << endl;
	//cout << "Next Max Area" << Next_Max << endl;
	//cin >> i;
	if (Max_Blob_Size > MIN_TARGET_SIZE_IN_THE_IMAGE)
	{
		cout << "Object found!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" <<endl;
	}
	IplImage* psuedo_color_blob_img = cvCreateImage(cvGetSize(foreground_mask), IPL_DEPTH_8U, 3);
    psuedo_color_blob_img = labels2psuedoColor(connected_components_img); // for improved display
	//cvShowImage("image:",psuedo_color_blob_img);
    //cvWaitKey();
	//cvDestroyWindow("Image:");
	//cin >> i;
	return Max_Blob_Size;
}
IplImage* Vision::doPyrDown(IplImage* in, int filter) 
{
	// Best to make sure input image is divisible by two. //IPL_GAUSSIAN_5x5
	//
	assert( in->width%2 == 0 && in->height%2 == 0 );
	IplImage* out = cvCreateImage(
	cvSize( in->width/2, in->height/2 ), in->depth, in->nChannels );
	cvPyrDown( in, out );
	return( out );
};
*/