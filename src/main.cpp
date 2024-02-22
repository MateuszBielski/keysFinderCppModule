#include <stdio.h>
#include "edgeprocessing.h"
#include "ini/ini.h"

int main(int argc, char **argv)
{
    inih::INIReader configurationParameters {"config.ini"};
    const auto& imagePath = configurationParameters.Get<std::string>("section1", "imagePath");

    EdgeProcessing ep;
    ep.LoadParameters(configurationParameters);
    ep.LoadImageBW(imagePath);
  
    ep.FindEdgePixels();
    ep.FindBreakingPoints();
    return 0;
}
