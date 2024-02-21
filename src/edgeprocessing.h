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
using cv::Mat, cv::Rect, cv::Vec3b;

class EdgeProcessing
{
private:
    Mat src;
    
    vector<Rect> DivideImageIntoSquareChunks();
    vector<Rect> SelectWithBlackAndWhitePixels(vector<Rect>& );
    vector<Rect> FindBlackEqualWhiteInNeighborhood(vector<Rect>& );
    vector<Rect> FindBlackEqualWhiteInNeighborhoodOld(vector<Rect>& );
    void ShowSelectedChunks(vector<Rect>& );
    void TrimToImageBorder(Rect& );
    void ForEachPixOfSourceImageInsideRect(Rect& , std::function<void(Vec3b&, const int *)> const& lambda);
    bool IsBlack(const Vec3b &p);
    bool IsWhite(const Vec3b &p);
public:
    unsigned edgeDetectInitialRadius = 10;
    uchar minWhiteLevel = 255;
    uchar maxBalckLevel = 0;
    float blackWhiteRatioMax = 0.25;
    
    void LoadImageBW(string );
    void FindEdgePixels();
    void FindBreakingPoints();

protected:

};
using spEdgeProcessing = std::shared_ptr<EdgeProcessing>;
using upEdgeProcessing = std::unique_ptr<EdgeProcessing>;
#endif // EdgeProcessing_H
