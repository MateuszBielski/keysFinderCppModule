#include <stdio.h>
#include "edgeprocessing.h"
#include "ini/ini.h"

int main(int argc, char **argv)
{
    inih::INIReader r {"config.ini"};
    const auto& imagePath = r.Get<std::string>("section1", "imagePath");

    EdgeProcessing ep;
    ep.LoadImageBW(imagePath);
    
    try {
        ep.edgeDetectInitialRadius = r.Get<unsigned>("section1", "edgeDetectRadius");
    }catch(std::exception& e) {}
    try {
        ep.minWhiteLevel = r.Get<uchar>("section1", "minWhiteLevel");
    }catch(std::exception& e) {}
    try {
        ep.maxBalckLevel = r.Get<uchar>("section1", "maxBalckLevel");
    }catch(std::exception& e) {}
    try {
        ep.blackWhiteRatioMax = r.Get<float>("section1", "blackWhiteRatioMax");
    }catch(std::exception& e) {}
    
    ep.FindEdgePixels();
    ep.FindBreakingPoints();
    return 0;
}
