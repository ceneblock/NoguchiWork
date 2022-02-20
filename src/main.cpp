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

#include <dirent.h>
#include <sys/stat.h>

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


struct HSV_values
{
  uint8_t H_min;
  uint8_t S_min;
  uint8_t V_min;
  uint8_t H_max;
  uint8_t S_max;
  uint8_t V_max;

  HSV_values()
  {
    H_min = 0;
    S_min = 0;
    V_min = 0;
    H_max = 0;
    S_max = 0;
    V_max = 0;
  };
};

map<string, HSV_values> colors;

vector<vector<int>> newPoints;


Point getContours(Mat imgDil, Mat img, string label)
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

      putText(img, label , {boundRect[x].x, boundRect[x].y - 5}, FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1);
      cout << "Putting a bounding box: " << to_string(boundRect[x].x) << " and " << to_string(boundRect[x].y) << endl;
    }
    ++x;
  }
  return myPoint;
}

void findColor(Mat img, map<string, HSV_values> colors)
{
  Mat imgHSV;
  cvtColor(img, imgHSV, COLOR_BGR2HSV);
  
  int x = 0;
  for( auto myColor : colors )
  {
    Scalar lower(myColor.second.H_min, myColor.second.S_min, myColor.second.V_min), 
           upper(myColor.second.H_max, myColor.second.S_max, myColor.second.V_max);
    Mat mask;
    inRange(imgHSV, lower, upper, mask);
    
    Point myPoint = getContours(mask, img, myColor.first);

    /*
    if(myPoint.x != 0 && myPoint.y != 0)
    {
      newPoints.push_back({myPoint.x, myPoint.y, x});
    }
    */
    ++x;
  }
}

void processImage(string fileName, string inputDirectory, string outputDirectory, map<string, HSV_values> colors)
{
  Mat img = imread(inputDirectory + "/" + fileName);

  Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;

  //Preprocessing image
  cvtColor(img, imgGray, COLOR_BGR2GRAY);
  GaussianBlur(imgGray, imgBlur, Size(3,3), 3, 0);
  Canny(imgBlur, imgCanny, 25, 75);

  Mat kernel = getStructuringElement(MORPH_RECT, Size(3,3));
  dilate(imgCanny, imgDil, kernel);
  findColor(img, colors);
  
  imwrite(outputDirectory + "/" + fileName, img);
}

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


  rapidjson::Value data;
  if(json.getValue("labels", data))
  {
    cerr << "No \"labels\" found. This means no output will occur\n";
  }

  //loop through all the labels
  for (rapidjson::SizeType x = 0; x < data.Size(); ++x)
  {
    string label;
    HSV_values hsv;
    rapidjson::Value::ConstMemberIterator itr;
   
    itr = data[x].FindMember("name");
    if(itr == data[x].MemberEnd())
    {
      cerr << "Cowardly refusing to look at data that doesn't have a \"name\"\n";
      continue;
    }
    else
    {
      label = data[x]["name"].GetString();
    }
    itr = data[x].FindMember("values");
    if(itr == data[x].MemberEnd())
    {
      cerr << "Cowardly refusing to look at data that doesn't have a \"values\"\n";
      continue;
    }
    else
    {
      itr = data[x]["values"].FindMember("H_min");
      if(itr == data[x]["values"].MemberEnd())
      {
        cerr << "No value found for H_min, defaulting to 0";
      }
      else
      {
        hsv.H_min = data[x]["values"]["H_min"].GetInt();
      }
      
      itr = data[x]["values"].FindMember("S_min");
      if(itr == data[x]["values"].MemberEnd())
      {
        cerr << "No value found for S_min, defaulting to 0";
      }
      else
      {
        hsv.S_min = data[x]["values"]["S_min"].GetInt();
      }

      itr = data[x]["values"].FindMember("V_min");
      if(itr == data[x]["values"].MemberEnd())
      {
        cerr << "No value found for V_min, defaulting to 0";
      }
      else
      {
        hsv.V_min = data[x]["values"]["V_min"].GetInt();
      }

      itr = data[x]["values"].FindMember("H_max");
      if(itr == data[x]["values"].MemberEnd())
      {
        cerr << "No value found for H_max, defaulting to 0";
      }
      else
      {
        hsv.H_max = data[x]["values"]["H_max"].GetInt();
      }
      
      itr = data[x]["values"].FindMember("S_max");
      if(itr == data[x]["values"].MemberEnd())
      {
        cerr << "No value found for S_max, defaulting to 0";
      }
      else
      {
        hsv.S_max = data[x]["values"]["S_max"].GetInt();
      }

      itr = data[x]["values"].FindMember("V_max");
      if(itr == data[x]["values"].MemberEnd())
      {
        cerr << "No value found for V_max, defaulting to 0";
      }
      else
      {
        hsv.V_max = data[x]["values"]["V_max"].GetInt();
      }

    }
     std::pair<std::map<string,HSV_values>::iterator,bool> ret;
     ret = colors.insert ( std::pair<string,HSV_values>(label, hsv));
     if (ret.second==false) 
     {
      std::cerr << "Object with label " << label << " already exists\n";
     }
  }

  /**
   * End JSON
   */

  /**
   * Open the input dir
   */

  struct dirent *ent;
  struct stat states;
  DIR *inputdir = opendir(imagesDirectory.c_str());
  if(inputdir == NULL)
  {
    cerr << "Unable to open " << imagesDirectory << endl;
    return EXIT_FAILURE;
  }
	while((ent=readdir(inputdir)) != NULL)
  {
    if(string(ent->d_name).compare(".") == 0 || string(ent->d_name).compare("..") == 0)
    {
      continue;
    }
    else
    {
		  processImage(string(ent->d_name), imagesDirectory, outputDirectory, colors);
    }
	}

	closedir(inputdir);



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
