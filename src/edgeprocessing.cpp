#include "edgeprocessing.h"


#include <filesystem>
//#include <fstream>
//#include <iostream>
namespace fs = std::filesystem;

using namespace cv;
using std::cout;

void EdgeProcessing::LoadImageBW(string fileName)
{
    if(!fs::exists(fileName)) return;

    src = imread(fileName,IMREAD_COLOR);
//    uchar * dataBegin = src.data;
//    const uchar * dataEnd = src.dataend;
//
//    size_t dataSize = (dataEnd - dataBegin) / sizeof(uchar);
//    ushort nuChannels = src.channels();
//    size_t nuPixels = dataSize / nuChannels;

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
        for(unsigned h = 0 ; h < chunk.rows ; h++) {
            for(unsigned w = 0 ; w < chunk.cols ; w++) {
//                auto pix = chunk.at<Vec3b>(h * chunk.cols + w);
                auto pix = chunk.at<Vec3b>(h,w);
                if(
                    pix[0] <= maxBalckLevel &&
                    pix[1] <= maxBalckLevel &&
                    pix[2] <= maxBalckLevel
                )black++;
                if(
                    pix[0] >= minWhiteLevel &&
                    pix[1] >= minWhiteLevel &&
                    pix[2] >= minWhiteLevel
                )white++;
            }
        }
        if(black && white)bwChunks.push_back(ch);
    }

    return bwChunks;
}
void EdgeProcessing::ShowSelectedChunks(vector<Rect>& selected)
{
    auto redColored(src);
    unsigned x, y, num = 0;
    for(auto& sel : selected) {
        
        bool isBlack = true;
        for(y = sel.y; y < (sel.y + sel.height) ; y++ )
        {
            for(x = sel.x ; x < (sel.x + sel.width) ; x++)
            {
                
                auto& pix = src.at<Vec3b>(y,x);
//                Vec3b pixTemp = pix;
                if(
                    pix[0] <= maxBalckLevel &&
                    pix[1] <= maxBalckLevel &&
                    pix[2] <= maxBalckLevel
                )isBlack = true;
                if(
                    pix[0] >= minWhiteLevel &&
                    pix[1] >= minWhiteLevel &&
                    pix[2] >= minWhiteLevel
                )isBlack = false;
                if(isBlack){
                    pix[2] = 190;
                }else{
                    pix[0] = 40;
                    pix[1] = 40;
                }
            }
        }
        num++;
    }
    imshow("original Image", redColored);
    waitKey(0);
}

void EdgeProcessing::FindEdgePixels()
{
    vector<Rect> chunks = DivideImageIntoSquareChunks();

    vector<Rect> chunksOnEdge = SelectWithBlackAndWhitePixels(chunks);
    ShowSelectedChunks(chunksOnEdge);
//    vector<Point>
    auto chunksSize = chunks.size();
    auto chunksOnEdgeSize = chunksOnEdge.size();

    //w obrębie przesuwać
//    Point square[edgeDetectInitialRadius * edgeDetectInitialRadius];
}
void EdgeProcessing::FindBreakingPoints()
{
}
