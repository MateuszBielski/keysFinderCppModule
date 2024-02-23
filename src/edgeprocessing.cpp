#include "edgeprocessing.h"


#include <filesystem>
//#include <fstream>
//#include <iostream>
namespace fs = std::filesystem;

using namespace cv;
using std::cout;

void EdgeProcessing::LoadParameters(inih::INIReader& r)
{

    try {
        edgeDetectInitialRadius = r.Get<unsigned>("section1", "edgeDetectRadius");
    } catch(std::exception& e) {}
    try {
        minWhiteLevel = r.Get<uchar>("section1", "minWhiteLevel");
    } catch(std::exception& e) {}
    try {
        maxBalckLevel = r.Get<uchar>("section1", "maxBalckLevel");
    } catch(std::exception& e) {}
    try {
        blackWhiteRatioMax = r.Get<float>("section1", "blackWhiteRatioMax");
    } catch(std::exception& e) {}
    try {
        otherChannelsRatioMax = r.Get<float>("section1", "otherChannelsRatioMax");
    } catch(std::exception& e) {}
}
void EdgeProcessing::LoadImageBW(string fileName)
{
    if(!fs::exists(fileName)) {
        src = CreateGhost();
        return;
    }

    src = imread(fileName,IMREAD_COLOR);
//    uchar * dataBegin = src.data;
//    const uchar * dataEnd = src.dataend;
//
//    size_t dataSize = (dataEnd - dataBegin) / sizeof(uchar);
//    ushort nuChannels = src.channels();
//    size_t nuPixels = dataSize / nuChannels;

}
void EdgeProcessing::MakeBlackAndWhiteIfGreenBackground()
{
    unsigned green = 0;
    src.forEach<Vec3b>([&,this](Vec3b &pix, const int * position) -> void {
        if(IsGreen(pix)) {
            green++;
        }
    });
    if(!green)return;
    src.forEach<Vec3b>([this](Vec3b &pix, const int * position) -> void {
        if(IsGreen(pix)) {
            MakeWhite(pix);
        } else{
            MakeBlack(pix);
        }
    });
}
vector<Rect> EdgeProcessing::DivideImageIntoSquareChunks()
{
    unsigned lastWidth = src.cols % edgeDetectInitialRadius;
    unsigned lastHeight = src.rows % edgeDetectInitialRadius;
    unsigned numbOfHorizontalSquares = (src.cols - lastWidth) / edgeDetectInitialRadius;
    unsigned numbOfVerticalSquares = (src.rows - lastHeight) / edgeDetectInitialRadius;

    unsigned numbOfHorizontalChunks = numbOfHorizontalSquares;
    unsigned numbOfVerticalChunks = numbOfVerticalSquares;

    if(lastWidth > 0)++numbOfHorizontalChunks;
    if(lastHeight > 0)++numbOfVerticalChunks;

    vector<Rect> chunks { numbOfHorizontalChunks * numbOfVerticalChunks};

    unsigned fromLeft = 0, fromTop = 0, width , height;

    unsigned w, h;
    //rows
    for(h = 0 ; h < numbOfVerticalSquares; h++ ) {
        width  = height = edgeDetectInitialRadius;
        //cols
        for(w = 0; w < numbOfHorizontalSquares; w++) {
            chunks.at(h * numbOfHorizontalChunks + w) = Rect(fromLeft, fromTop, width, height);
            fromLeft += width;
        }
        //last col
        if(lastWidth > 0) chunks.at(h * numbOfHorizontalChunks + w) = Rect(fromLeft, fromTop, lastWidth, height);

        fromTop += height;
        fromLeft = 0;
    }
    //last row
    if(lastHeight > 0) {
        for(w = 0; w < numbOfHorizontalSquares; w++) {
            chunks.at(h * numbOfHorizontalChunks + w) = Rect(fromLeft, fromTop, width, lastHeight);
            fromLeft += width;
        }
        //last cell
        if(lastWidth > 0) chunks.at(h * numbOfHorizontalChunks + w) = Rect(fromLeft, fromTop, lastWidth, lastHeight);
    }
    return chunks;
}
vector<Rect> EdgeProcessing::SelectWithBlackAndWhitePixels(vector<Rect>& allChunks)
{
    vector<Rect> bwChunks;
    for(auto& ch : allChunks) {
        Mat chunk;
        try {
            chunk = Mat {src, ch};
        } catch(std::exception& e) {
            cout<<"\n"<<e.what();
            cout<<"\nsrc.cols = "<<src.cols<<" ch.x + ch.width = "<<ch.x + ch.width;
            cout<<"\nsrc.rows = "<<src.rows<<" ch.y + ch.height = "<<ch.y + ch.height<<std::flush;
            continue;
        }
        unsigned white = 0, black = 0;

        chunk.forEach<Vec3b>([ &, this](Vec3b &p, const int * position) -> void {
            if(IsBlack(p)) {
                black++;
                return;
            }
            if(IsWhite(p))white++;
        });
        if(black && white)bwChunks.push_back(ch);
    }

    return bwChunks;
}
void EdgeProcessing::TrimToImageBorder(Rect& rect)
{
    if(rect.x < 0)rect.x = 0;
    if(rect.y < 0)rect.y = 0;
    int rightExceed = rect.x + rect.width - src.cols;
    int downExceed =  rect.y + rect.height - src.rows;
    if(rightExceed > 0)rect.width -= rightExceed;
    if(downExceed > 0)rect.height -= downExceed;
    if(rect.width < 0) rect.width = 0;
    if(rect.height < 0) rect.height = 0;
}
void EdgeProcessing::ForEachPixOfSourceImageInsideRect(Rect& rect, std::function<void(Vec3b&, const int *)> const& Func)
{
    unsigned x, y;
    int pos[2];
    for(y = 0; y < rect.height ; y++) {
        pos[1] = rect.y + y;
        for(x = 0; x < rect.width ; x++) {
            pos[0] = rect.x + x;
            auto& pix = src.at<Vec3b>(pos[1], pos[0]);
            Func(pix, pos);
        }

    }
}

vector<Rect> EdgeProcessing::FindBlackEqualWhiteInNeighborhood(vector<Rect>& selected)
{
    vector<Rect> centeredChunks;
    unsigned numChunks = 0;
    for(auto& sel : selected) {
        Rect rect {sel};
        bool needMorePrecision = true;
        bool lessThanSecondCycle = true;
        bool useNewRectangle;
        int numWhite = 0, numBlack = 0;
        int prevCenter_x = round(sel.x + sel.width / 2.0);
        int prevCenter_y = round(sel.y + sel.height / 2.0);
        int centerDiff_x = 10;
        int centerDiff_y = 10;
        ushort numCycles = 0;
        int blackWhiteDiff;
        int prevBlackWhiteDiff;
        float blackWhiteRatioCurrent;
        if(numChunks == 4) {
            int r = 1;
        }
        while(lessThanSecondCycle || needMorePrecision ) {
            useNewRectangle = false;
            float centerWhite_x = 0.0, centerWhite_y = 0.0,
                  centerBlack_x = 0.0, centerBlack_y = 0.0;
            numWhite = 0;
            numBlack = 0;
            auto AddBlacksAddWhites = [ &, this](Vec3b &p, const int * absolutePosition) -> void {
                if(IsBlack(p)) {
                    ++numBlack;
                    centerBlack_x += absolutePosition[0];
                    centerBlack_y += absolutePosition[1];
                    return;
                }
                if(IsWhite(p)) {
                    ++numWhite;
                    centerWhite_x += absolutePosition[0];
                    centerWhite_y += absolutePosition[1];
                }
            };
            ForEachPixOfSourceImageInsideRect(rect,AddBlacksAddWhites);

            if(!numWhite || !numBlack)break;

            centerWhite_x /= numWhite;
            centerWhite_y /= numWhite;
            centerBlack_x /= numBlack;
            centerBlack_y /= numBlack;
            float center_x = (centerWhite_x + centerBlack_x)/2.0;
            float center_y = (centerWhite_y + centerBlack_y)/2.0;
            float new_xf = center_x - sel.width / 2;
            float new_yf = center_y - sel.height / 2;
            int new_x = static_cast<int>(round(new_xf));
            int new_y = static_cast<int>(round(new_yf));
            centerDiff_x = abs(round(center_x) - prevCenter_x);
            centerDiff_y = abs(round(center_y) - prevCenter_y);
            if(centerDiff_x < 2 && centerDiff_y < 2) {
                needMorePrecision = false;
                useNewRectangle = true;
            }
            blackWhiteDiff = abs(numBlack - numWhite);
            blackWhiteRatioCurrent = (float)blackWhiteDiff / (numBlack + numWhite);
            if(!needMorePrecision && blackWhiteRatioCurrent > blackWhiteRatioMax) {
                useNewRectangle = false;
                break;
            }
            Rect newRect {new_x, new_y, sel.width, sel.height};
            TrimToImageBorder(newRect);

            rect = newRect;
            prevCenter_x = center_x;
            prevCenter_y = center_y;

            lessThanSecondCycle = numCycles < 2 ? true : false;
            ++numCycles;
        }
        numChunks++;
        if(useNewRectangle && rect.width && rect.height)centeredChunks.push_back(rect);
    }
    return centeredChunks;
}
vector<Vec2i> EdgeProcessing::ArrangeInOrder(vector<Vec2i>& notOrderly)
{
    vector<int> indices(src.cols,-1);
    unsigned which = 0;
    for(auto& p : notOrderly)
    {
        indices[p[0]] = which++;
    }
    
    vector<Vec2i> orderly;
    for(int i : indices)
    {
        if(i > -1){
            Vec2i p = notOrderly[i];
            orderly.push_back(p);
        }
    }
    
    return orderly;
}
void EdgeProcessing::ShowSelectedChunks(vector<Rect>& selected)
{
    Mat redColored = src.clone();
    unsigned x, y, num = 0;
    for(auto& sel : selected) {
        Mat chunk {redColored, sel};

        chunk.forEach<Vec3b>([ &, this](Vec3b &pix, const int * position) -> void {
            if(IsBlack(pix)) {
                pix[2] = 190;
            } else {
                pix[0] = 40;
                pix[1] = 40;
            }
            num++;
        });
    }
    imshow("original Image", redColored);
    waitKey(0);
}
void EdgeProcessing::ShowLinesBetweenPoints(vector<Vec2i>& points)
{
    Mat imgWithLine = src.clone();
    Vec3b color {0,255,0};
    Vec2i p0 = *(points.begin());
    Vec2i p1;
    unsigned nuPoints = points.size();
    unsigned i = 1;
    for(i ; i < nuPoints ; i++) {
        p1 = points[i];
        line(imgWithLine,p0,p1,color);
        p0 = p1;
    }
    imshow("line", imgWithLine);
    waitKey(0);

}
void EdgeProcessing::FindEdgePixels()
{
    MakeBlackAndWhiteIfGreenBackground();
//    imshow("black White Image", src);
//    waitKey(0);
    vector<Rect> chunks = DivideImageIntoSquareChunks();
    vector<Rect> chunksOnEdge = SelectWithBlackAndWhitePixels(chunks);
//    ShowSelectedChunks(chunksOnEdge);
    vector<Rect> chunksAccurateOnEdge = FindBlackEqualWhiteInNeighborhood(chunksOnEdge);
    
    vector<Vec2i> pointsNotSorted = GetCentresOfRectangles(chunksAccurateOnEdge);
//    ShowLinesBetweenPoints(pointsNotSorted);
    vector<Vec2i> pointsOrderly = ArrangeInOrder(pointsNotSorted);
    
    ShowLinesBetweenPoints(pointsOrderly);
//    ShowSelectedChunks(chunksAccurateOnEdge);
    auto chunksSize = chunks.size();
    auto chunksOnEdgeSize = chunksOnEdge.size();

}
void EdgeProcessing::FindBreakingPoints()
{
}

bool EdgeProcessing::IsBlack(const Vec3b& p)
{
    if(
        p[0] <= maxBalckLevel &&
        p[1] <= maxBalckLevel &&
        p[2] <= maxBalckLevel
    ) return true;
    return false;
}
bool EdgeProcessing::IsWhite(const Vec3b& p)
{
    if(
        p[0] >= minWhiteLevel &&
        p[1] >= minWhiteLevel &&
        p[2] >= minWhiteLevel
    )return true;
    return false;
}
bool EdgeProcessing::IsGreen(const Vec3b& p)
{
    if(!p[1])return false;
    float redDivGreen = (float)p[2] / p[1];
    float blueDivGreen = (float)p[0] / p[1];
    if(
        redDivGreen < otherChannelsRatioMax &&
        blueDivGreen < otherChannelsRatioMax
    )return true;
    return false;
}
void EdgeProcessing::MakeWhite(Vec3b& p)
{
    p[0] = 255;
    p[1] = 255;
    p[2] = 255;
}
void EdgeProcessing::MakeBlack(Vec3b& p)
{
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
}
Mat EdgeProcessing::CreateGhost()
{
    Mat ghost {100,100,CV_8UC1};
    uchar randColor[3];//not inited make random
    ghost.forEach<Vec3b>([&, this](Vec3b &pix, const int * position) -> void {
        pix[0] = randColor[0];
        pix[1] = randColor[1];
        pix[2] = randColor[2];
    });
    return ghost;
}
vector<Vec2i> EdgeProcessing::GetCentresOfRectangles(vector<Rect>& rects)
{
    vector<Vec2i> centres;
    for(auto& r : rects) {
        int x = (float)round(r.x + r.width / 2.0);
        int y = (float)round(r.y + r.height / 2.0);
        centres.push_back(Vec2i {x,y});
    }
    return centres;
}

