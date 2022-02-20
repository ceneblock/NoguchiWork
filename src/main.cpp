#include <apr_general.h>
#include <apr_getopt.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <json.h>

#include "config.h"

using namespace std;
using namespace cv;

static const apr_getopt_option_t opt_option[] = {
    /* long-option, short-option, has-arg flag, description */
    { "config", 'c', TRUE, "input file" },      /* -c name or --config name */
    { "help", 'h', FALSE, "show help" },    /* -h or --help */
    { NULL, 0, 0, NULL }, /* end (a.k.a. sentinel) */
  };



/*
vector<vector<int>> newPoints;

//Yellow, Green, Orange, Pink
vector<vector<int>> myColors {
  {58, 215, 183, 179, 255, 255},
};

Point getContours(Mat imgDil, Mat img)
{
    
  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;
  
  findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
  
  //draws contours
  //drawContours(img, contours, -1, Scalar(255,0,255), 2);

  Point myPoint(0,0);
  vector<vector<Point>> conPoly(contours.size());
  vector<Rect> boundRect(contours.size());
  string objectType = "";
  int x = 0;
  for(auto contour : contours)
  {
    int area = contourArea(contour);
    cout << "Area: " << area << endl;


    if(area > 10)
    {
      float peri = arcLength(contour, true);
      approxPolyDP(contour, conPoly[x], 0.02 * peri, true);
      //drawContours(img, conPoly, x, Scalar(255, 0, 255), 2);
      
      cout << "conPoly size: " << conPoly[x].size() << endl;
      boundRect[x] = boundingRect(conPoly[x]);
      rectangle(img, boundRect[x].tl(), boundRect[x].br(), Scalar(0,255,0), 1);

      int objCor = (int)conPoly[x].size();
      float aspRatio = 0;
      myPoint.x = boundRect[x].x + (boundRect[x].width / 2);
      myPoint.y = boundRect[x].y;

      putText(img, "Red" , {boundRect[x].x, boundRect[x].y - 5}, FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1);
      cout << "Putting a bounding box: " << to_string(boundRect[x].x) << " and " << to_string(boundRect[x].y) << endl;
    }
    ++x;
  }
  return myPoint;
}

void findColor(Mat img)
{
  Mat imgHSV;
  cvtColor(img, imgHSV, COLOR_BGR2HSV);
  
  int x = 0;
  for( auto myColor : myColors )
  {
    Scalar lower(myColor.at(0), myColor.at(1), myColor.at(2)), 
           upper(myColor.at(3), myColor.at(4), myColor.at(5));
    Mat mask;
    inRange(imgHSV, lower, upper, mask);

    imshow(to_string(x), mask);
    Point myPoint = getContours(mask, img);

    if(myPoint.x != 0 && myPoint.y != 0)
    {
      newPoints.push_back({myPoint.x, myPoint.y, x});
    }
    ++x;
  }
}
*/

void showHelp()
{
  cout << endl << endl << endl;
  cout << PACKAGE_NAME << endl;
  for(int x = 0; x < strlen(PACKAGE_NAME); ++x)
  {
    cout << "-";
  }
  cout << endl;

  /**
   * @TODO: use iomanip
   */
  int x = 0;
  while(opt_option[x].name != NULL)
  {
    cout << "--" << opt_option[x].name << " \t| -" << (char)opt_option[x].optch;
    if(opt_option[x].has_arg)
    {
      cout << " OPTION";
    }
    cout << " \t| " << opt_option[x].description << endl;
    ++x;
  }
  cout << endl << endl << endl;
}
int main(int argc, char *argv[])
{

  string jsonLocation = "";
  bool haveJson = false;

  string imagesDirectory = "./";  
  string outputDirectory = "./";  

  /**
   * APR crap
   */
  apr_initialize();
  apr_status_t rv;
  apr_getopt_t *opt;
  apr_pool_t *pool;
  int optch;
  const char *optarg;

  rv = apr_pool_create(&pool, NULL);
  if(rv != APR_SUCCESS)
  {
    cerr << "Error in apr_pool_create\n";
  }

  rv = apr_getopt_init(&opt, pool, argc, argv);
  if(rv != APR_SUCCESS)
  {
    cerr << "Error in apt_getopt_init\n";
  }
  

  /* parse the all options based on opt_option[] */
  while ((rv = apr_getopt_long(opt, opt_option, &optch, &optarg)) == APR_SUCCESS) 
  {
    switch(optch)
    {
      case 'c':
        if(optarg)
        {
          haveJson = true;
          jsonLocation = optarg;
        }
        else
        {
          cerr << "No input file specified!";
          showHelp();
        }
        break;
      case 'h':
        showHelp();
    }

  }
  if (rv != APR_EOF) {
      printf("bad options\n");
  }
 
  if(!haveJson)
  {
    cerr << "Missing JSON file!\n";
    apr_terminate();
    return EXIT_FAILURE;
  }

  apr_terminate();

  /**
   * End APR crap
   */

  /**
   * Begin JSON
   */

  JSON json(jsonLocation.c_str());

  if(json.parse() == true)
  {
    cerr << "Invalid JSON!\n";
    return EXIT_FAILURE;
  }

  if(json.getString("images", imagesDirectory))
  {
    cerr << "No \"images\" specified, defaulting to current dirrectory\n";
  } 

  if(json.getString("output", outputDirectory))
  {
    cerr << "No \"output\" specified, defaulting to current dirrectory\n";
  } 


  /**
   * End JSON
   */




  /*
  string path = argv[1];
  //string path = "Resources/shapes.png";

  //Read in the image
  Mat img = imread(path);
  Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;

  //Preprocessing image
  cvtColor(img, imgGray, COLOR_BGR2GRAY);
  GaussianBlur(imgGray, imgBlur, Size(3,3), 3, 0);
  Canny(imgBlur, imgCanny, 25, 75);

  Mat kernel = getStructuringElement(MORPH_RECT, Size(3,3));
  dilate(imgCanny, imgDil, kernel);

  findColor(img);

  //getContours(imgDil, img);
      

  //Show the image
  imshow("Image", img);
  imwrite("shapes_out.png", img);
  imshow("Image Gray", imgGray);
  imshow("Image Blur", imgBlur);
  imshow("Image Canny", imgCanny);
  imshow("Image Dil", imgDil);

  //wait forever
  waitKey(0);
  */

  return EXIT_SUCCESS;
}
