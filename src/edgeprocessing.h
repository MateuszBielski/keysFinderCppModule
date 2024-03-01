#ifndef EdgeProcessing_H
#define EdgeProcessing_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include "ini/ini.h"

#define colorCheckFg 0b00000001
#define borderCheckFg 0b00000100
#define blackFg 0b00000001
#define whiteFg 0b00000011

using std::string, std::vector;
using cv::Mat, cv::Rect, cv::Vec3b, cv::Vec2i, cv::TickMeter;

class EdgeProcessing
{
//enum class pixUchar{zero, black, white, border};
private:
    Mat src;
    Mat srcUchar;

    TickMeter timeRecorder;
    unsigned edgeDetectInitialRadius = 10;
    uchar minWhiteLevel = 255;
    uchar maxBalckLevel = 0;
    float blackWhiteRatioMax = 0.25;
    float otherChannelsRatioMax = 0.3;
    double timeArrangeInOrder = 0.0;
    double timeFindBlackEqualWhite = 0.0;
    double timeSelectWithBlackAndWhitePixels = 0.0;

    void MakeBlackAndWhiteIfGreenBackground();
    vector<Rect> DivideImageIntoSquareChunks();
    vector<Rect> SelectWithBlackAndWhitePixels(vector<Rect>& );
    vector<Rect> FindBlackEqualWhiteInNeighborhood(vector<Rect>& );
    vector<Vec2i> GetCentresOfRectangles(vector<Rect>& );
    vector<Vec2i> GetBlackPixBorderingWithWhite(vector<Rect>& );
    vector<Vec2i> ArrangeInOrder(vector<Vec2i>& );
    void ShowSelectedChunks(vector<Rect>& );
    void ShowLinesBetweenPoints(vector<Vec2i>& );
    void TrimToImageBorder(Rect& );
    void ForEachPixOfSourceImageInsideRect(Rect& , std::function<void(Vec3b&, const int *)> const& lambda);
    bool IsBlack(const Vec3b &p);
    bool IsWhite(const Vec3b &p);
    bool IsGreen(const Vec3b &p);
    void MakeWhite(Vec3b &p);
    void MakeBlack(Vec3b &p);
    Mat CreateGhost();
public:
    void LoadParameters(inih::INIReader& config);
    void LoadImageBW(string );
    void FindEdgePixels();
    void FindBreakingPoints();

protected:

};
using spEdgeProcessing = std::shared_ptr<EdgeProcessing>;
using upEdgeProcessing = std::unique_ptr<EdgeProcessing>;
#endif // EdgeProcessing_H
