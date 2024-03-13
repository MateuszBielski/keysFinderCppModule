#ifndef EdgeProcessing_H
#define EdgeProcessing_H

#include <iostream>
#include <memory>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include "ini/ini.h"
#include "timemeasure.h"

#define colorCheckFg 0b00000001
#define borderCheckFg 0b00000100
#define sortedBorderFg 1 >> 3
#define blackFg 0b00000001
#define whiteFg 0b00000011

using std::string, std::list , std::vector, std::map, std::function;
using cv::Mat, cv::Rect, cv::Vec3b, cv::Vec2i, cv::TickMeter;

class EdgeProcessing
{
//enum class pixUchar{zero, black, white, border};
private:
    Mat src;
    Mat srcUchar;

    unsigned edgeDetectInitialRadius = 10;
    ushort shortSideDivider1 = 3;
    ushort shortSideDivider2 = 3;
    uchar minWhiteLevel = 255;
    uchar maxBalckLevel = 0;
    float blackWhiteRatioMax = 0.25;
    float otherChannelsRatioMax = 0.3;
    
    TimeMeasure timeMeasure;
    
//    TickMeter timeRecorder;
    double timeArrangeInOrder = 0.0;
    double timeFindBlackEqualWhite = 0.0;
    double timeSelectWithBlackAndWhitePixels = 0.0;
    unsigned whichMeasurment = 0;
    map<string,double> times;

    void MakeBlackAndWhiteIfGreenBackground();
    vector<Rect> DivideImageIntoSquareChunks();//--
    list<Rect> DivideRectIntoSquaresAndRest(list<Rect>& src, ushort shortSideDivider);
    Mat RecognizeWhiteAndBlack();
    vector<Rect> SelectWithBlackAndWhitePixels(vector<Rect>& );//--
    list<Rect> SelectFromSrcUcharWithNotTheSamePixels(list<Rect>& );
    vector<Rect> FindBlackEqualWhiteInNeighborhood(vector<Rect>& );//--
    list<Rect> CenterRectsOnBorderAndRemoveSpots(list<Rect>& );
    vector<Vec2i> GetCentresOfRectangles(list<Rect>& );
    void SetFlagsOnSrcUchar(vector<Vec2i>& points,int flag);
    vector<Vec2i> GetBlackPixBorderingWithWhite(vector<Rect>& );
    vector<Vec2i> ArrangeInOrderOld(vector<Vec2i>& );
    vector<Vec2i> ArrangeInOrder(vector<Vec2i>& );
    void ShowSelectedChunks(list<Rect>& );
    void ShowLinesBetweenPoints(vector<Vec2i>& );
    void TrimToImageBorder(Rect& );
    void ForEachPixOfSourceImageInsideRect(Rect& , function<void(Vec3b&, const int *)> const& lambda);
    void ForEachPixOfUcharSourceInsideRect(Rect& , function<void(uchar&, const int *)> const& lambda);
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
    void FindEdgePixelsOld();
    void FindBreakingPoints();

protected:

};
using spEdgeProcessing = std::shared_ptr<EdgeProcessing>;
using upEdgeProcessing = std::unique_ptr<EdgeProcessing>;
#endif // EdgeProcessing_H
