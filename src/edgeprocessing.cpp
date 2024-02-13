#include "edgeprocessing.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <filesystem>
//#include <fstream>
//#include <iostream>
namespace fs = std::filesystem;

using namespace cv;

void EdgeProcessing::LoadImageBW(string fileName)
{
    if(!fs::exists(fileName)) return;
        
    Mat src = imread(fileName,IMREAD_COLOR);
}
