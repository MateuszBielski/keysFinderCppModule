#ifndef EdgeProcessing_H
#define EdgeProcessing_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>


using std::string, std::vector;
using cv::Mat, cv::Rect;

class EdgeProcessing
{
private:
    Mat src;
    
    vector<Rect> DivideImageIntoSquareChunks();
    vector<Rect> SelectWithBlackAndWhitePixels(vector<Rect>& );
    void ShowSelectedChunks(vector<Rect>& );
public:
    unsigned edgeDetectInitialRadius = 10;
    uchar minWhiteLevel = 255;
    uchar maxBalckLevel = 0;
    
    void LoadImageBW(string );
    void FindEdgePixels();
    void FindBreakingPoints();

protected:

};
using spEdgeProcessing = std::shared_ptr<EdgeProcessing>;
using upEdgeProcessing = std::unique_ptr<EdgeProcessing>;
#endif // EdgeProcessing_H
