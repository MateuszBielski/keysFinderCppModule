#include "edgeprocessing.h"


#include <filesystem>
//#include <fstream>
//#include <iostream>
namespace fs = std::filesystem;

using namespace cv;
using std::cout, std::flush, std::move, std::to_string;

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
    try {
        shortSideDivider1 = r.Get<ushort>("section1", "shortSideDivider1");
    } catch(std::exception& e) {}
    try {
        shortSideDivider2 = r.Get<ushort>("section1", "shortSideDivider2");
    } catch(std::exception& e) {}
}
void EdgeProcessing::LoadImageBW(string fileName)
{
    timeMeasure.functionMeasureStart(__FUNCTION__);
    if(!fs::exists(fileName)) {
        src = CreateGhost();
        return;
    }

    src = imread(fileName,IMREAD_COLOR);

    srcUchar = Mat::zeros(src.rows,src.cols,CV_8UC1);

    timeMeasure.functionMeasureStop();
}
void EdgeProcessing::MakeBlackAndWhiteIfGreenBackground()
{
    timeMeasure.functionMeasureStart(__FUNCTION__);
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
    timeMeasure.functionMeasureStop();
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
list<Rect> EdgeProcessing::DivideRectIntoSquaresAndRest(list<Rect>& sources, ushort shortSideDivider)
{
    timeMeasure.functionMeasureStart(__FUNCTION__);

    list<Rect> result;


    for(auto& source : sources) {

        unsigned shortSide = source.width > source.height ? source.height : source.width;
        unsigned longSide = source.height + source.width - shortSide;
        unsigned restShortSide = shortSide % shortSideDivider;
        unsigned squareSide = (shortSide - restShortSide) / shortSideDivider;
        if (!squareSide) {
            continue;
        }
        unsigned lastWidth = source.width % squareSide;
        unsigned lastHeight = source.height % squareSide;
        unsigned numbOfHorizontalSquares = (source.width - lastWidth) / squareSide;
        unsigned numbOfVerticalSquares = (source.height - lastHeight) / squareSide;

        unsigned numbOfHorizontalChunks = numbOfHorizontalSquares;
        unsigned numbOfVerticalChunks = numbOfVerticalSquares;

        if(lastWidth > 0)++numbOfHorizontalChunks;
        if(lastHeight > 0)++numbOfVerticalChunks;

        vector<Rect> chunks { numbOfHorizontalChunks * numbOfVerticalChunks};

        unsigned fromLeft = source.x , fromTop = source.y , width , height;

        unsigned w, h;
        //rows
        for(h = 0 ; h < numbOfVerticalSquares; h++ ) {
            width  = height = squareSide;
            //cols
            for(w = 0; w < numbOfHorizontalSquares; w++) {
                chunks.at(h * numbOfHorizontalChunks + w) = Rect(fromLeft, fromTop, width, height);
                fromLeft += width;
            }
            //last col
            if(lastWidth > 0) chunks.at(h * numbOfHorizontalChunks + w) = Rect(fromLeft, fromTop, lastWidth, height);

            fromTop += height;
            fromLeft = source.x;
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
        for(auto& chunk : chunks)result.push_back(move(chunk));
    }

    timeMeasure.functionMeasureStop();
    return result;
}
vector<Rect> EdgeProcessing::SelectWithBlackAndWhitePixels(vector<Rect>& allChunks)
{
    timeMeasure.functionMeasureStart(__FUNCTION__);

    vector<Rect> bwChunks;
    for(auto& rect : allChunks) {
        unsigned white = 0, black = 0;
        ForEachPixOfSourceImageInsideRect(rect,[ &, this](Vec3b &p, const int * position) -> void {
            if(IsBlack(p)) {
                srcUchar.at<uchar>(position[1],position[0]) |= blackFg;
                black++;
                return;
            }
            if(IsWhite(p)) {
                srcUchar.at<uchar>(position[1],position[0]) |= whiteFg;
                white++;
            }
        });
        if(black && white)bwChunks.push_back(rect);
    }
//    timeRecorder.stop();
//    timeSelectWithBlackAndWhitePixels = timeRecorder.getTimeMilli();
    timeMeasure.functionMeasureStop();
    return bwChunks;
}
list<Rect> EdgeProcessing::SelectFromSrcUcharWithNotTheSamePixels(list<Rect>& allRects)
{
    timeMeasure.functionMeasureStart(__FUNCTION__);

    list<Rect> bwChunks;
    uchar color;

    for(auto rect : allRects) {
        unsigned white = 0, black = 0;
        ForEachPixOfUcharSourceInsideRect(rect,[&, this](uchar& p, const int * position) -> void {
            //color = srcUchar.at<uchar>(position[1],position[0]) << 6;
            color = p << 6;
            color >>= 6;
            if(color == whiteFg) {
                white++;
                return;
            }
            if(color == blackFg) {
                black++;
            }
        });
        if(black && white)bwChunks.push_back(rect);
    }

    timeMeasure.functionMeasureStop();
    return bwChunks;
}
Mat EdgeProcessing::RecognizeWhiteAndBlack()
{
    timeMeasure.functionMeasureStart(__FUNCTION__);
    Mat result = Mat::zeros(src.rows,src.cols,CV_8UC1);
    Rect rect(0,0,src.cols,src.rows);
    ForEachPixOfSourceImageInsideRect(rect,[ &, this](Vec3b &p, const int * position) -> void {
        if(IsBlack(p)) {
            result.at<uchar>(position[1],position[0]) |= blackFg;
            return;
        }
        if(IsWhite(p)) {
            result.at<uchar>(position[1],position[0]) |= whiteFg;
        }
    });

    timeMeasure.functionMeasureStop();
    return result;
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
void EdgeProcessing::ForEachPixOfUcharSourceInsideRect(Rect&  rect, std::function<void(uchar&, const int *)> const& Func)
{
    unsigned x, y;
    int pos[2];
    for(y = 0; y < rect.height ; y++) {
        pos[1] = rect.y + y;
        for(x = 0; x < rect.width ; x++) {
            pos[0] = rect.x + x;
            uchar& pix = srcUchar.at<uchar>(pos[1], pos[0]);
            Func(pix, pos);
        }

    }
}
list<Rect> EdgeProcessing::CenterRectsOnBorderAndRemoveSpots(list<Rect>& selected)
{
    timeMeasure.functionMeasureStart(__FUNCTION__);
    list<Rect> centeredChunks;
    unsigned numChunks = 0;
    for(auto& rect : selected) {
        bool needMorePrecision = true;
        bool lessThanSecondCycle = true;
        bool useNewRectangle;
        int numWhite = 0, numBlack = 0;
        int prevCenter_x = round(rect.x + rect.width / 2.0);
        int prevCenter_y = round(rect.y + rect.height / 2.0);
        int centerDiff_x = 10;
        int centerDiff_y = 10;
        ushort numCycles = 0;
        int blackWhiteDiff;
        int prevBlackWhiteDiff;
        float blackWhiteRatioCurrent;
        uchar color;
        if(numChunks == 4) {
            int r = 1;
        }
        while(lessThanSecondCycle || needMorePrecision ) {
            useNewRectangle = false;
            float centerWhite_x = 0.0, centerWhite_y = 0.0,
                  centerBlack_x = 0.0, centerBlack_y = 0.0;
            numWhite = 0;
            numBlack = 0;
            auto AddBlacksAddWhites = [ &, this](uchar& p, const int * absolutePosition) -> void {
                color = p << 6;
                color >>= 6;
                if(color == blackFg) {
                    ++numBlack;
                    centerBlack_x += absolutePosition[0];
                    centerBlack_y += absolutePosition[1];
                    return;
                }
                if(color == whiteFg) {
                    ++numWhite;
                    centerWhite_x += absolutePosition[0];
                    centerWhite_y += absolutePosition[1];
                }
            };
            ForEachPixOfUcharSourceInsideRect(rect,AddBlacksAddWhites);

            if(!numWhite || !numBlack)break;

            centerWhite_x /= numWhite;
            centerWhite_y /= numWhite;
            centerBlack_x /= numBlack;
            centerBlack_y /= numBlack;
            float center_x = (centerWhite_x + centerBlack_x)/2.0;
            float center_y = (centerWhite_y + centerBlack_y)/2.0;
            float new_xf = center_x - rect.width / 2;
            float new_yf = center_y - rect.height / 2;
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
                //skip spots
                useNewRectangle = false;
                break;
            }
            Rect newRect {new_x, new_y, rect.width, rect.height};
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
    timeMeasure.functionMeasureStop();
    return centeredChunks;
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
                //skip spots
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
    timeMeasure.functionMeasureStart(__FUNCTION__);
    uchar nodeFlag = borderCheckFg;
    SetFlagsOnSrcUchar(notOrderly, nodeFlag);

    Vec2i point = *notOrderly.begin();
    Vec2i transPoint(point[1],point[0]);
    auto notOrderlySize = notOrderly.size();
    vector<Vec2i> orderly(notOrderlySize);
    //determine directon for neighbour detection
    //direct to short, further border.
    //convention: incoming points are P(x,y), but Mat.at uses y,x
    bool shortSideIsVertical = src.cols > src.rows ? true : false;
    int distanceBetweenShortSides = shortSideIsVertical ? src.cols : src.rows;
    int coordAlongLongSide = shortSideIsVertical ? point[0] : point[1];
    bool isForward = (float)distanceBetweenShortSides / coordAlongLongSide > 2.0 ? true : false;
    //transposed convention
    Vec2i direction = shortSideIsVertical ? Vec2i(0,1) : Vec2i(1,0);
    if(!isForward) direction *= -1;
    
    srcUchar.at<uchar>(transPoint) |= nodeSortedFg;
    bool possibleOtherNode = true;
    int step = 1;
    Vec2i pointToCheck, centerBreak, leftBreak, rightBreak;
    Rect sourceBorders(0,0,src.cols,src.rows);
    unsigned whichNodeSorted = 0;
    orderly[whichNodeSorted++] = Vec2i(transPoint[1],transPoint[0]);
//    cout<<"\n"<<transPoint[1]<<", "<<transPoint[0];
    while(possibleOtherNode) {

        int distance = 1;
        bool nextNotFound = true;
        while(nextNotFound) {
            
            //In determined direction checking in distance 1 pix, 2 pix and so on...
//        straight
            pointToCheck = transPoint + direction * step * distance;

//            cout<<"\n"<<pointToCheck[1]<<", "<<pointToCheck[0];
            if(!InsideRect(pointToCheck,sourceBorders)){
                possibleOtherNode = false;
                break;
            }
                
            uchar& pointValC = srcUchar.at<uchar>(pointToCheck);
            if(pointValC & nodeFlag && !(pointValC & nodeSortedFg)) {
                nextNotFound = false;
                pointValC |= nodeSortedFg;
                transPoint = pointToCheck;
                distance = 0;
            }
            centerBreak = pointToCheck;
            //turn left
            Vec2i localLeft(-1 * direction[1],direction[0]);
            bool backLeft = true;
            for(int l = 1; l <= distance ; l++) {
                pointToCheck = centerBreak + localLeft * step * l;
                leftBreak = pointToCheck;
                if(!InsideRect(pointToCheck,sourceBorders)) {
                    backLeft = false;
                    break;
                }
                uchar& pointVal = srcUchar.at<uchar>(pointToCheck);
                if(pointVal & nodeFlag && !(pointVal & nodeSortedFg)) {
                    nextNotFound = false;
                    pointVal |= nodeSortedFg;
                    distance = 0;
                    transPoint = pointToCheck;
                }
            }
            //back and turn right
            Vec2i localRight = -1 * localLeft;
            bool backRight = true;
            for(int r = 1; r <= distance ; r++) {
                pointToCheck = centerBreak + localRight * step * r;
                rightBreak = pointToCheck;
                if(!InsideRect(pointToCheck,sourceBorders)) {
                    backRight = false;
                    break;
                }
                uchar& pointVal = srcUchar.at<uchar>(pointToCheck);
                if(pointVal & nodeFlag && !(pointVal & nodeSortedFg)) {
                    nextNotFound = false;
                    pointVal|= nodeSortedFg;
                    distance = 0;
                    transPoint = pointToCheck;
                }
            }
            Vec2i backDir = -1 * direction;
            //again left and turn left
            if(backLeft) {
                for(int l = 1 ; l <= distance ; l++) {
                    pointToCheck = leftBreak + backDir * step * l;
                    if(!InsideRect(pointToCheck,sourceBorders)) {
                        break;
                    }
                    uchar& pointVal = srcUchar.at<uchar>(pointToCheck);
                    if(pointVal & nodeFlag && !(pointVal & nodeSortedFg)) {
                        nextNotFound = false;
                        pointVal |= nodeSortedFg;
                        distance = 0;
                        transPoint = pointToCheck;
                    }
                }
            }
            //again right and turn right
            if(backRight) {
                for(int r = 1; r <= distance ; r++) {
                    pointToCheck = rightBreak + backDir * step * r;
                    if(!InsideRect(pointToCheck,sourceBorders)) {
                        break;
                    }
                    uchar& pointVal = srcUchar.at<uchar>(pointToCheck);
                    if(pointVal & nodeFlag && !(pointVal & nodeSortedFg)) {
                        nextNotFound = false;
                        pointVal |= nodeSortedFg;
                        distance = 0;
                        transPoint = pointToCheck;
                    }
                }
            }
            distance++;
            if(transPoint == pointToCheck) {
                orderly[whichNodeSorted++] = Vec2i(transPoint[1],transPoint[0]);
                nextNotFound = false;
            }
            if(!backLeft && !backRight) {
                possibleOtherNode = false;
                break;
            }
            if(distance == 150) {
                possibleOtherNode = false;
                break;
            }
        }
    }
    cout<<flush;
    orderly.resize(whichNodeSorted);

    //has started from border?
    timeMeasure.functionMeasureStop();
    return orderly;
}
vector<Vec2i> EdgeProcessing::ArrangeInOrderOld(vector<Vec2i>& notOrderly)
{
    timeMeasure.functionMeasureStart(__FUNCTION__);
    vector<int> indices(src.cols,-1);
    unsigned which = 0;
    for(auto& p : notOrderly) {
        indices[p[0]] = which++;
    }

    vector<Vec2i> orderly;
    for(int i : indices) {
        if(i > -1) {
            Vec2i p = notOrderly[i];
            orderly.push_back(p);
        }
    }
    timeMeasure.functionMeasureStop();
    return orderly;
}
void EdgeProcessing::ShowSelectedChunks(list<Rect>& selected)
{
    Mat redColored = src.clone();
    unsigned x, y, num = 0;
    for(auto& sel : selected) {
        Mat chunk {redColored, sel};

        chunk.forEach<Vec3b>([ &, this](Vec3b &pix, const int * position) -> void {
            if(IsBlack(pix)) {
                pix[2] = 190;
            } else {
                pix[0] = 41;
                pix[1] = 41;
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
    if(points.size()) {
        Vec2i p0 = *(points.begin());
        Vec2i p1;
        unsigned nuPoints = points.size();
        unsigned i = 1;
        for(i ; i < nuPoints ; i++) {
            p1 = points[i];
            line(imgWithLine,p0,p1,color);
            p0 = p1;
        }
    }
    imshow("line", imgWithLine);
    waitKey(0);

}
void EdgeProcessing::FindEdgePixels()
{
    MakeBlackAndWhiteIfGreenBackground();
//    imshow("black White Image", src);
//    waitKey(0);
    srcUchar = RecognizeWhiteAndBlack();
    list<Rect> imageRect {Rect (0,0,srcUchar.cols, srcUchar.rows)};
    list<Rect> bigChunks = DivideRectIntoSquaresAndRest(imageRect, shortSideDivider1);
    list<Rect> bigChunksOnEdge = SelectFromSrcUcharWithNotTheSamePixels(bigChunks);
//    ShowSelectedChunks(chunksOnEdge);
    list<Rect> chunksAccurateOnEdge = CenterRectsOnBorderAndRemoveSpots(bigChunksOnEdge);

//    ShowSelectedChunks(chunksAccurateOnEdge);
    list<Rect> smallChunks = DivideRectIntoSquaresAndRest(chunksAccurateOnEdge, shortSideDivider2);
    list<Rect> smallChunksOnEdge = SelectFromSrcUcharWithNotTheSamePixels(smallChunks);
    auto smallChunksSize = smallChunks.size();
    auto smallChunksOnEdgeSize = smallChunksOnEdge.size();
    chunksAccurateOnEdge = CenterRectsOnBorderAndRemoveSpots(smallChunksOnEdge);
//    ShowSelectedChunks(chunksAccurateOnEdge);
    vector<Vec2i> pointsNotSorted = GetCentresOfRectangles(chunksAccurateOnEdge);

    //vector<Vec2i> pointsNotSorted = GetBlackPixBorderingWithWhite(chunksAccurateOnEdge);
//    ShowLinesBetweenPoints(pointsNotSorted);
    vector<Vec2i> pointsOrderly = ArrangeInOrder(pointsNotSorted);
    timeMeasure.ShowMeasurments();

    //ShowSelectedChunks(chunksAccurateOnEdge);
    ShowLinesBetweenPoints(pointsOrderly);
}
void EdgeProcessing::FindEdgePixelsOld()
{
    MakeBlackAndWhiteIfGreenBackground();
//    imshow("black White Image", src);
//    waitKey(0);
    vector<Rect> chunks = DivideImageIntoSquareChunks();
    vector<Rect> chunksOnEdge = SelectWithBlackAndWhitePixels(chunks);
//    ShowSelectedChunks(chunksOnEdge);
    vector<Rect> chunksAccurateOnEdge = FindBlackEqualWhiteInNeighborhood(chunksOnEdge);

//    vector<Vec2i> pointsNotSorted = GetCentresOfRectangles(chunksAccurateOnEdge);
    vector<Vec2i> pointsNotSorted = GetBlackPixBorderingWithWhite(chunksAccurateOnEdge);
//    ShowLinesBetweenPoints(pointsNotSorted);
    vector<Vec2i> pointsOrderly = ArrangeInOrder(pointsNotSorted);
    cout<<"\ntime of SelectWithBlackAndWhitePixels: "<<timeSelectWithBlackAndWhitePixels<<"ms";
    cout<<"\ntime of FindBlackEqualWhite: "<<timeFindBlackEqualWhite<<"ms";
    cout<<"\ntime of ArrangeInOrder: "<<timeArrangeInOrder<<"ms";
    cout<<"\n"<<flush;
//    ShowSelectedChunks(chunksAccurateOnEdge);
    ShowLinesBetweenPoints(pointsOrderly);
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
bool EdgeProcessing::InsideRect(Vec2i& p, Rect& rect)
{
    bool insideRect = true;
    insideRect &= p[0] >= rect.y;
    insideRect &= p[0] <= rect.y + rect.height;
    insideRect &= p[1] >= rect.x;
    insideRect &= p[1] <= rect.x + rect.width;
    return insideRect;
}
bool EdgeProcessing::InsideRect(Vec2i& p, Mat& srcMat)
{
    Rect rect(0,0,srcMat.cols,srcMat.rows);
    return InsideRect(p, rect);
}
Mat EdgeProcessing::CreateGhost()
{
    Mat ghost(100,100,CV_8UC3);
    uchar randColor[3];//not inited make random
    ghost.forEach<Vec3b>([&, this](Vec3b &pix, const int * position) -> void {
        pix[0] = randColor[0];
        pix[1] = randColor[1];
        pix[2] = randColor[2];
    });
    return ghost;
}
vector<Vec2i> EdgeProcessing::GetCentresOfRectangles(list<Rect>& rects)
{
    vector<Vec2i> centres(rects.size());
    auto iter = centres.begin();
    for(auto& r : rects) {
        int x = (float)round(r.x + r.width / 2.0);
        int y = (float)round(r.y + r.height / 2.0);
        *iter++ = Vec2i { x,y };
        //centres.push_back(Vec2i {x,y});
    }
    return centres;
}
void EdgeProcessing::SetFlagsOnSrcUchar(vector<Vec2i>& points, int flag)
{
    for(auto& p : points) {
        srcUchar.at<uchar>(p[1],p[0]) |= flag;
    }
}
vector<Vec2i> EdgeProcessing::GetBlackPixBorderingWithWhite(vector<Rect>& rects)
{
    vector<Vec2i> result;
    auto EverySecond = [](int sizeSrc, vector<int>& dst)->void {
        int isOdd = sizeSrc%2;
        int half = (sizeSrc - isOdd) / 2;
        int everySecondSize = isOdd + half;
        dst.resize(everySecondSize);
        int i = 0;
        for(i; i < half; i++) dst[i] = 2*i;
        if(isOdd)dst[i] = 1 + 2*(i - 1);
    };
    /*
    for(auto& rect : rects) {

        vector<int> everySecond_x;
        vector<int> everySecond_y;
        EverySecond(rect.x,everySecond_x);
        EverySecond(rect.y,everySecond_y);



        uchar pix1 = 0;
        uchar pix2 = 0;
        //set color
        pix1 |= whiteFg;
        pix2 |= blackFg;
        bool isBorder = pix1 & borderCheckFg;
        pix1 |= borderCheckFg;
        isBorder = pix1 & borderCheckFg;

        auto CheckAndSetIfBorder = [](uchar& pix1, uchar& pix2)->bool {
            //https://www.geeksforgeeks.org/bitmasking-in-cpp/
            if (pix2 & borderCheckFg) return true;
            if (pix2 & borderCheckFg) return true;
            //if colors are different, set both as border
            if(pix1 != pix2) {
                pix1 |= borderCheckFg;
                pix2 |= borderCheckFg;
                return true;
            }
            return false;
        };
        map<int,int> swapedDirections;
        vector<Vec2i> directions {{-1,0},
            {1,0},
    //            ...

        };
        short i = 0;
        for(auto& d : directions) {
            int hashDir = 10 * d[0] + d[1];
            swapedDirections[hashDir] = i++;
        }

    //        directions[]
        auto NextPair = [](Vec2i& p1, Vec2i& p2, Vec2i& p0, directions)->bool {
            Vec2i currentDir = p2 - p1;
            bool nextPairAvaliable = true;

            bool insideRect = true;
            insideRect &= p2[0] >= rect.y;
            insideRect &= p2[0] <= rect.y + rect.height;
            insideRect &= p2[1] >= rect.x;
            insideRect &= p2[1] <= rect.x + rect.width;
            if(insideRect) {
                p1 += currentDir;
                p2 += currentDir;
                return nextPairAvaliable;
            }
            int hashCurrentDir = 10 * currentDir[0] + currentDir[1];
            directions[hashCurrentDir] =

        };
    //        Vec2i point1 =;
    //        Vec2i point2 =;
        Vec2i startPoint = point1;
        bool continueFindBorder = true;
        while(continueFindBorder) {
    //            direction = DetermineDirection(directionsOrder);
    //            continueFindBorder &= CheckAndSetIfBorder(srcUchar.at(point1),srcUchar(point2));
    //            continueFindBorder &= NextPair(point1,point2,startPoint,directionsOrder);
        }

    //        auto MoveByOnePix = [](Vec2i lastChecked, Vec2i whichNext, function<bool(uchar&, uchar&) Check)->void{
    //
    //        };
    //        -1,-1
    //        for(auto& evSecRow : everySecond_y)
    //        {
    //            for(auto& evSecCol : everySecond_x)
    //            {
    //                ul = srcUchar.at<uchar>(rect.y + evSecRow - 1,rect.x + evSecCol - 1);
    //                uc = srcUchar.at<uchar>(rect.y + evSecRow - 1,rect.x + evSecCol + 0);
    //                ur = srcUchar.at<uchar>(rect.y + evSecRow - 1,rect.x + evSecCol + 1);
    //                cl = srcUchar.at<uchar>(rect.y + evSecRow + 0,rect.x + evSecCol - 1);
    //                cc = srcUchar.at<uchar>(rect.y + evSecRow + 0,rect.x + evSecCol + 0);
    //                cr = srcUchar.at<uchar>(rect.y + evSecRow + 0,rect.x + evSecCol + 1);
    //
    //                if(ul != border ul != uc)srcUchar.at<uchar>()
    //            }
    //        }
    }
    */
//            auto AddBlacksAddWhites = [ &, this](Vec3b &p, const int * absolutePosition) -> void {
//    ForEachPixOfSourceImageInsideRect(rect,AddBlacksAddWhites);
    return result;
}
